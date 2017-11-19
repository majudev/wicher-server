#include "control.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

#include "networking.h"

namespace spd = spdlog;

bool control_running = true;
int control_sock, control_clientsock;

void * control_handler(void *){
#ifdef SPDLOG_ENABLE_SYSLOG
        auto console = spd::syslog_logger("control", "Wicher-Server", LOG_PID);
#else
        auto console = spd::stdout_color_mt("control");
#endif
	console->info("[Control] Handler spawned");
	struct sockaddr_un server_addr;

        console->info("[Control] Creating control_socket for server");
        if((control_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == 0) {
                console->error("Error when creating control control_socket: control_socket: {}", strerror(errno));
                exit(EXIT_FAILURE);
        }
	
        console->info("[Control] Binding control to {}", Config::getSingleton()->control_path);
	memset(&server_addr, 0, sizeof(server_addr));
     	server_addr.sun_family = AF_UNIX;
     	strcpy(server_addr.sun_path, Config::getSingleton()->control_path.c_str());
        unlink(Config::getSingleton()->control_path.c_str());
     	if (bind(control_sock, (struct sockaddr *)&server_addr, SUN_LEN(&server_addr)) < 0) {
                console->error("[Control] Error when binding to control control_socket: bind: {}", strerror(errno));
                exit(EXIT_FAILURE);
        }
	
        console->info("[Control] Listening on {} (max waiting connections = 1)", Config::getSingleton()->control_path);
        if (listen(control_sock, 1) < 0){
                console->error("[Control] Error when listening on control_socket: listen: {}", strerror(errno));
                exit(EXIT_FAILURE);
        }
	
	while(control_running){
                console->debug("[Control] Waiting for connection");
                control_clientsock = accept(control_sock, NULL, NULL);
                if(control_clientsock >= 0){
                        console->debug("[Control] Got connection");
                        while(1){
                                char buffer[1025];
                                int n = read(control_clientsock, buffer, 1024);
                                if(n <= 0){
                                        console->debug("[Control] Error when receiving command");
                                        close(control_clientsock);
                                        break;
                                }else buffer[n] = '\0';
                                
                                if(!strcmp(buffer, "HALT")){
                                        console->info("[Control] HALT received");
                                        networking_shutdown();
                                        control_running = false;
                                        n = write(control_clientsock, "OK", 3);
                                        break;
                                }else if(!strcmp(buffer, "BYE")){
                                        console->debug("[Control] BYE received");
                                        n = write(control_clientsock, "OK", 3);
                                        break;
                                }else{
                                        console->debug("[Control] Unknown command received");
                                        n = write(control_clientsock, "Unknown command", 16);
                                        break;
                                }
                                close(control_clientsock);
                        }
                        console->debug("[Control] Connection loop ended");
                }
	}
	close(control_sock);
        
        console->debug("[Control] Loop ended, thread exit");
}
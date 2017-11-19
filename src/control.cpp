#include "control.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

namespace spd = spdlog;

void * control_handler(void *){
#ifdef SPDLOG_ENABLE_SYSLOG
        auto console = spd::syslog_logger("control", "Wicher-Server", LOG_PID);
#else
        auto console = spd::stdout_color_mt("control");
#endif
	console->info("Control handler spawned");
	int sock, clientsock;
	struct sockaddr_un server_addr;

        console->info("Creating control socket for server");
        if((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == 0) {
                console->error("Error when creating control socket: socket: {}", strerror(errno));
                exit(EXIT_FAILURE);
        }
	
        console->info("Binding control to {}", Config::getSingleton()->control_path);
	memset(&server_addr, 0, sizeof(server_addr));
     	server_addr.sun_family = AF_UNIX;
     	strcpy(server_addr.sun_path, Config::getSingleton()->control_path.c_str());
        unlink(Config::getSingleton()->control_path.c_str());
     	if (bind(sock, (struct sockaddr *)&server_addr, SUN_LEN(&server_addr)) < 0) {
                console->error("Error when binding to control socket: bind: {}", strerror(errno));
                exit(EXIT_FAILURE);
        }
	
        console->info("Listening control on {} (max waiting connections = 5)", Config::getSingleton()->control_path);
        if (listen(sock, 5) < 0){
                console->error("Error when listening on control socket: listen: {}", strerror(errno));
                exit(EXIT_FAILURE);
        }
	
	while(running){
                clientsock = accept(sock, NULL, NULL);
                if(clientsock >= 0){
                        while(1){
                                char buffer[1025];
                                int n = read(clientsock, buffer, 1024);
                                if(n < 0){
                                        close(clientsock);
                                        break;
                                }else buffer[n] = '\0';
                                if(!strcmp(buffer, "HALT")){
                                        console->info("HALT received");
                                        running = false;
                                        n = write(clientsock, "OK", 3);
                                        close(clientsock);
                                        break;
                                }else if(!strcmp(buffer, "BYE")){
                                        n = write(clientsock, "OK", 3);
                                        close(clientsock);
                                        break;
                                }else{
                                        n = write(clientsock, "Unknown command", 16);
                                        close(clientsock);
                                        break;
                                }
                        }
                }
	}
	
	close(sock);
}
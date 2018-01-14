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
#include "DBman.h"
#include "sha256.h"

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
        
        if(Config::getSingleton()->noadmin){
                console->info("[Control] Control disabled, exiting...");
        }

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
                                
                                std::string cmd_full(buffer);
                                std::vector<std::string> cmd;
                                int cmd_n = split(cmd_full, cmd, ' ');
                                
                                if(cmd_n == 1 && cmd[0] == "HALT"){
                                        console->info("[Control] HALT received");
                                        networking_shutdown();
                                        control_running = false;
                                        n = write(control_clientsock, "OK", 3);
                                        break;
                                }else if(cmd_n == 1 && cmd[0] == "BYE"){
                                        console->debug("[Control] BYE received");
                                        n = write(control_clientsock, "OK", 3);
                                        break;
                                }else if(cmd_n == 4 && cmd[0] == "AUTH" && cmd[1] == "REG"){ //AUTH REG {username} {password}
                                        console->debug("[Control] Received: AUTH REG {0} {1}", cmd[2], cmd[3]);
                                        SHA256 sha256;
                                        std::string passwd = sha256(cmd[3]);
                                        AuthDB::RegError res = DBman::getSingleton()->auth.reg(cmd[2].c_str(), passwd.c_str());
                                        char resp[32];
                                        switch(res){
                                            case AuthDB::REG_ALREADY_EXISTS:
                                                strcpy(resp, "User already exists!");
                                                break;
                                            case AuthDB::REG_INTERNAL_DB_ERROR:
                                                strcpy(resp, "Internal database error.");
                                                break;
                                            case AuthDB::REG_OK:
                                                strcpy(resp, "Registered successfuly.");
                                                break;
                                        }
                                        n = write(control_clientsock, resp, 32);
                                }else if(cmd_n == 3 && cmd[0] == "AUTH" && cmd[1] == "ACTIVATE"){ //AUTH ACTIVATE {username}
                                        console->debug("[Control] Received: AUTH ACTIVATE {0}", cmd[2]);
                                        bool res = DBman::getSingleton()->auth.make_active(cmd[2].c_str());
                                        char resp[32];
                                        switch(res){
                                            case true:
                                                strcpy(resp, "User activated.");
                                                break;
                                            case false:
                                                strcpy(resp, "Internal database error.");
                                                break;
                                        }
                                        n = write(control_clientsock, resp, 32);
                                }else if(cmd_n == 3 && cmd[0] == "AUTH" && cmd[1] == "DEACTIVATE"){ //AUTH DEACTIVATE {username}
                                        console->debug("[Control] Received: AUTH DEACTIVATE {0}", cmd[2]);
                                        bool res = DBman::getSingleton()->auth.make_inactive(cmd[2].c_str());
                                        char resp[32];
                                        switch(res){
                                            case true:
                                                strcpy(resp, "User deactivated.");
                                                break;
                                            case false:
                                                strcpy(resp, "Internal database error.");
                                                break;
                                        }
                                        n = write(control_clientsock, resp, 32);
                                }else if(cmd_n == 3 && cmd[0] == "AUTH" && cmd[1] == "ACTIVE"){ //AUTH ACTIVE {username}
                                        console->debug("[Control] Received: AUTH ACTIVE {0}", cmd[2]);
                                        bool res = DBman::getSingleton()->auth.is_active(cmd[2].c_str());
                                        char resp[32];
                                        switch(res){
                                            case true:
                                                strcpy(resp, "User is active.");
                                                break;
                                            case false:
                                                strcpy(resp, "User is inactive.");
                                                break;
                                        }
                                        n = write(control_clientsock, resp, 32);
                                }else if(cmd_n == 3 && cmd[0] == "AUTH" && cmd[1] == "DROP"){ //AUTH DROP {username}
                                        console->debug("[Control] Received: AUTH DROP {0}", cmd[2]);
                                        bool res = DBman::getSingleton()->auth.drop(cmd[2].c_str());
                                        char resp[32];
                                        switch(res){
                                            case true:
                                                strcpy(resp, "User deleted.");
                                                break;
                                            case false:
                                                strcpy(resp, "No such user!");
                                                break;
                                        }
                                        n = write(control_clientsock, resp, 32);
                                }else if(cmd_n == 2 && cmd[0] == "AUTH" && cmd[1] == "LIST"){ //AUTH LIST
                                        console->debug("[Control] Received: AUTH LIST");
                                        std::vector<std::string> users = DBman::getSingleton()->auth.get_users();
                                        std::string res;
                                        for(auto &str : users){
                                                res += str;
                                                res += '\n';
                                        }
                                        n = write(control_clientsock, res.c_str(), res.size() + 1);
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

void control_shutdown(){
        if(Config::getSingleton()->noadmin) return;
        control_running = false;
        shutdown(control_sock, SHUT_RDWR);
}

unsigned int split(const std::string &txt, std::vector<std::string> &strs, char ch){
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos ) );

    return strs.size();
}
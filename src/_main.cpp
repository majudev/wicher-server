#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "buildconfig.h"

#include "Config.h"
#include "Toolkit.h"
#include "MainOptions.h"
#include "DBman.h"
#include "control.h"

#define MAX_CONNECTIONS 30

namespace spd = spdlog;

bool running = true;

void print_help(char * arg0){
    std::cout << "Usage:" << std::endl
        << arg0 << " [options]" << std::endl << std::endl
        << "Possible options are:" << std::endl
        << "  -A                    disable admin socket" << std::endl
        << "  -h                    show help" << std::endl
        << "  -p [port]             set listening port number" << std::endl;
}

int main(int argc, char * argv[]){
    MainOptions mo(argc, argv);
    if(mo.hasKey("-h")){
        print_help(argv[0]);
        return 0;
    }
    Config::getSingleton()->noadmin = mo.hasKey("-A");
    Config::getSingleton()->port = mo.getParamFromKey("-p") ? Toolkit::strtoi(mo.getParamFromKey("-p")->second) : 63431;
    try{
#ifdef SPDLOG_ENABLE_SYSLOG
        auto console = spd::syslog_logger("main", "Wicher-Server", LOG_PID);
#else
        auto console = spd::stdout_color_mt("main");
#endif

        pid_t pid = fork();

        if (pid == -1){
            std::cerr << "Cannot fork! " << strerror(errno) << std::endl;
            return -1;
        }else if(pid == 0){
            console->info("Forked!");
        }else{
            std::cout << "Forked child with pid " << pid << std::endl;
            exit(0);
        }
        console->info("Starting WicherDB");
        
        pthread_t control_t;
        int control_v = pthread_create(&control_t, NULL, control_handler, 0);
        if(control_v){
            console->error("Error - pthread_create() return code: {0}\n", control_v);
            return -1;
        }
        
        DBman db;

        int sock;
        int opt = 1;
        int client_socks[MAX_CONNECTIONS];
        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            client_socks[i] = 0;
        }
        fd_set readfds;
        struct sockaddr_in server_addr;
        bzero((char *) &server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(Config::getSingleton()->port);

        console->info("Creating socket for server");
        if((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            console->error("Error when creating socket: socket: {}", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt)) < 0){
            console->error("Error when creating socket: setsockopt: {}", strerror(errno));
            exit(EXIT_FAILURE);
        }
        console->info("Binding to 0.0.0.0:{}", Config::getSingleton()->port);
        if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            console->error("Error when binding to socket: bind: {}", strerror(errno));
            exit(EXIT_FAILURE);
        }
        console->info("Listening on {0}:{1} (max waiting connections = 5)", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
        if (listen(sock, 5) < 0){
            console->error("Error when listening on socket: listen: {}", strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        console->info("Starting main loop");
        int sd, max_sd, activity, new_socket, valread;
        struct sockaddr_in address;
        socklen_t addrlen = sizeof(address);
        while(running){
            //clear the socket set
            FD_ZERO(&readfds);
            
            //add master socket to set
            FD_SET(sock, &readfds);
            max_sd = sock;
            
            //add child sockets to set
            for (int i = 0; i < MAX_CONNECTIONS; ++i){
                //socket descriptor
                sd = client_socks[i];

                //if valid socket descriptor then add to read list
                if(sd > 0)
                    FD_SET(sd, &readfds);

                //highest file descriptor number, need it for the select function
                if(sd > max_sd)
                    max_sd = sd;
            }
            
            //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
            activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

            if ((activity < 0) && (errno!=EINTR)) {
                console->warn("select error");
            }
            
            //If something happened on the master socket , then its an incoming connection
            if (FD_ISSET(sock, &readfds)) {
                if ((new_socket = accept(sock, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                    /** TODO **/
                    //perror("accept");
                    //exit(EXIT_FAILURE);
                }

                //inform user of socket number - used in send and receive commands
                console->info("New connection from {0}:{1}", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                
                //Register in DBman
                db.reg(new_socket);

                //add new socket to array of sockets
                for (int i = 0; i < MAX_CONNECTIONS; ++i) {
                    //if position is empty
                    if(!client_socks[i]){
                        client_socks[i] = new_socket;
                        break;
                    }
                }
            }
            
            //else its some IO operation on some other socket
            for (int i = 0; i < MAX_CONNECTIONS; ++i) {
                sd = client_socks[i];

                if (FD_ISSET(sd, &readfds)) {
                    //Check if it was for closing, and also perform DBman
                    if (!db.perform(sd)) {
                        //Somebody disconnected , get his details and print
                        getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                        console->info("Host {0}:{1} disconnected", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                        //Drop from DBman
                        db.drop(sd);
                        
                        //Close the socket and mark as 0 in list for reuse
                        close(sd);
                        client_socks[i] = 0;
                    }
                }
            }
        }
        
        console->info("Shutting down");
        pthread_join(control_t, NULL);
    } catch(const spd::spdlog_ex& ex) {
        std::cerr << "Log init failed: " << ex.what() << std::endl;
        return 1;
    }
}

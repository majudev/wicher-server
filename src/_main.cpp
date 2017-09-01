#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "buildconfig.h"

#include "Config.h"
#include "Toolkit.h"
#include "MainOptions.h"

namespace spd = spdlog;

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
        
        console->info("Creating socket for server");
	    int sock = socket(AF_INET, SOCK_STREAM, 0);
	    if(sock < 0){
		    perror("Error when creating socket");
		    exit(1);
	    }else console->->info("OK");
	    struct sockaddr_in server_addr;
	    bzero((char *) &server_addr, sizeof(server_addr));
	    server_addr.sin_family = AF_INET;
	    //server_addr.sin_addr.s_addr = INADDR_ANY;
	    server_addr.sin_port = htons(port);
	    console->info(std::string("Binding to 127.0.0.1:") + Toolkit::itostr(port));
        if(bind(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
            perror("Error when binding to port");
            exit(1);
        }else console->info("OK");
        console->info("Listening on socket with interval 5...");
        if(listen(sock, 5) < 0){
            perror("Error when listening");
            exit(1);
        }else console->info("OK");
        
        console->info("Shutting down");
    } catch(const spd::spdlog_ex& ex) {
        std::cerr << "Log init failed: " << ex.what() << std::endl;
        return 1;
    }
}

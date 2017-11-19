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
#include "networking.h"

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
    if(mo.hasKey("-d")) spd::set_level(spd::level::debug);
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
        
        pthread_t control_t;
        int control_v = pthread_create(&control_t, NULL, control_handler, 0);
        if(control_v){
            console->error("Error - pthread_create() return code: {0}\n", control_v);
            return -1;
        }
        
        pthread_t networking_t;
        int networking_v = pthread_create(&control_t, NULL, networking_handler, 0);
        if(networking_v){
            control_running = false;
            console->error("Error - pthread_create() return code: {0}\n", control_v);
            return -1;
        }
        
        console->info("Shutting down");
        pthread_join(control_t, NULL);
        pthread_join(networking_t, NULL);
    } catch(const spd::spdlog_ex& ex) {
        std::cerr << "Log init failed: " << ex.what() << std::endl;
        return 1;
    }
}

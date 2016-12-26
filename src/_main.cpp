#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <csignal>

#include "Main.h"
#include "Toolkit.h"

Wicher::DB::Main * main_obj;

void sigc_handler(int signum){
	if(signum == SIGUSR1){
		main_obj->shutdown();
		Wicher::DB::Log::info("Received USR1. No longer accepting connections.");
	}
}

int main(int argc, char * argv[]){
	signal(SIGUSR1, sigc_handler);
	int port = 63431;
	if(argc == 2 && Wicher::DB::Toolkit::strcheck(argv[1], "-h")){
		std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
		std::cout << "OR: " << argv[0] << " -h" << std::endl;
		std::cout << "OR: " << argv[0] << std::endl;
		std::cout << "Default port is 63431" << std::endl;
		exit(0);
	}else if(argc == 2){
		sscanf(argv[1], "%d", &port);
	}
    Wicher::DB::Log::info("Starting...");
    main_obj = new Wicher::DB::Main(port);
    main->run();
	delete main_obj;
    Wicher::DB::Log::info("Quitting...");
    return 0;
}

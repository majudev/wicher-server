#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Main.h"
#include "Toolkit.h"

int main(int argc, char * argv[]){
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
    Wicher::DB::Main main(port);
    main.run();
    Wicher::DB::Log::info("Quitting...");
    return 0;
}

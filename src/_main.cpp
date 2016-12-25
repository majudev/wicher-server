#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Main.h"

int main(int argc, char * argv[]){
    Wicher::DB::Log::info("Starting...");
    Wicher::DB::Main main(argc, argv);
    main.run();
    Wicher::DB::Log::info("Quitting...");
    /**using namespace Wicher::DB;
    DatabaseManager man("db.local");
    DatabaseManager::ErrorID eid;
    if(!man.type_exists("M")){
        if(!man.create_type(std::string("M"), std::string("Materac"), std::string(""), &eid)){
            std::cout << "Error when creating type M: " << eid << std::endl;
        }
    }
    int nextid = man.get_next_item_id("M", &eid);
    if(nextid < 0){
        std::cout << "Error when getting next item id: " << eid << std::endl;
    }else if(!man.create_item(nextid, "M", "", &eid)){
        std::cout << "Error when creating item M/" << nextid << ": " << eid << std::endl;
    }
    nextid = man.get_next_history_id(&eid);
    if(nextid < 0){
        std::cout << "Error when getting next history id: " << eid << std::endl;
    }else if(!man.create_history(nextid, "NULL", &eid)){
        std::cout << "Error when creating history " << nextid << ": " << eid << std::endl;
    }
    if(!man.update_item(1, -1, "M", -1, "LOL", &eid)){
        std::cout << "Error when updating item M/0: " << eid << std::endl;
    }
    if(!man.update_type("M", "K", "", "", &eid)){
        std::cout << "Error when updating type M: " << eid << std::endl;
    }
    nextid = man.get_next_wz_id(&eid);
    std::vector<int> ids;
    std::vector<std::string> types;
    ids.push_back(1);
    types.push_back("K");
    if(nextid < 0){
        std::cout << "Error when getting next wz id: " << eid << std::endl;
    }else if(!man.create_wz(nextid, "01/01/01", "Ich", "", ids, types, &eid)){
        std::cout << "Error when creating wz " << nextid << ": " << eid << std::endl;
    }*/
	/*char buffer[256];
	while(1){
		int r = read(clientsock, buffer, 255);
		if(r < 0){
			perror("Error when receiving message");
			continue;
		}
		if(buffer[r-1] != '\0'){
			buffer[r] = '\0';
			std::cout << "Got non null-terminated string!" << std::endl;
		}
		std::cout << "<< " << buffer << std::endl;
		if(strcheck(buffer, "BYE")){
			break;
		}
		std::cout << ">> ";
		std::cin >> buffer;
		if(strcheck(buffer, "NULL")){
			int res = write(clientsock, buffer, strlen(buffer) + 1);
			if(res != strlen(buffer) + 1){
				perror("Error when writing to socket!");
			}
		}
	}
	std::cout << "Closing connection..." << std::endl;
	close(clientsock);
	close(sock);
	return 0;*/
}

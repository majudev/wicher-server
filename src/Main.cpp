#include "Main.h"

Wicher::DB::Main::Main(int argc, char * argv[]){
    int port = 63431;
	if(argc == 2 && Toolkit::strcheck(argv[1], "-h")){
		std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
		std::cout << "OR: " << argv[0] << " -h" << std::endl;
		std::cout << "OR: " << argv[0] << std::endl;
		std::cout << "Default port is 63431" << std::endl;
		exit(0);
	}else if(argc == 2){
		sscanf(argv[1], "%d", &port);
	}
	cm = new ConnectionManager(port);
	mp = new MessageParser();
}

void Wicher::DB::Main::run(){
    if(!cm->get_connection()){
        Log::info("Failed to get connection. Exiting.");
        exit(0);
    }
    while(cm->is_up()){
        std::string query = cm->recv_msg();
        if(query.empty() || query == std::string("BYE")){
            Log::client("Empty query or BYE request. Exiting.");
            break;
        }else Log::client(std::string("Query: ") + query);
        std::string response = mp->parse(query);
        if(cm->send_msg(response)){
            Log::server(std::string("Response: ") + response);
        }else{
            Log::server("Failed to send response. Exiting.");
            break;
        }
    }
}

Wicher::DB::Main::~Main(){
    delete cm;
    delete mp;
}

#include "Main.h"

Wicher::DB::Main::Main(int port) : accept_new(true){
	cm = new ConnectionManager(port);
}

void Wicher::DB::Main::check_connection(Connection * conn){
	while(conn->is_up());
	delete conn;
}

void Wicher::DB::Main::run(){
	std::vector<boost::thread*> threads;
	while(accept_new){
		int sock = cm->get_connection();
		if(sock > 0){
			Connection * conn = new Connection(sock);
			threads.push_back(new boost::thread(&Wicher::DB::Main::check_connection, this, conn));
		}
	}
	Wicher::DB::Log::info("Waiting for threads to die...");
	for(unsigned int i = 0; i < threads.size(); ++i){
		threads[i]->join();
	}
}

void Wicher::DB::Main::shutdown(){
	this->accept_new = false;
}

Wicher::DB::Main::~Main(){
	delete cm;
}

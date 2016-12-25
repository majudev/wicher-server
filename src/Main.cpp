#include "Main.h"

Wicher::DB::Main::Main(int port): accept_new(true){
	cm = new ConnectionManager(port);
}

void Wicher::DB::Main::check_connection(Connection * conn){
	while(conn->is_up());
	delete conn;
}

void Wicher::DB::Main::run(){
	while(accept_new){
		int sock = cm->get_connection();
		if(sock > 0){
			Connection * conn = new Connection(sock);
			new boost::thread(&Wicher::DB::Main::check_connection, this, conn);
		}
	}
}

Wicher::DB::Main::~Main(){
	delete cm;
}

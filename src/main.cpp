#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <csignal>

#include "main.h"

using namespace Wicher::DB;

bool accept_new = true;
int port = 63431;
int sock;
std::vector<Wicher::DB::Connection *> connections;

void sigc_handler(int signum){
	if(signum == SIGUSR1){
		accept_new = false;
		close(sock);
		Wicher::DB::Log::info("Received USR1. No longer accepting connections.");
	}
}

int main(int argc, char * argv[]){
	signal(SIGUSR1, sigc_handler);
	if(argc == 2 && Wicher::DB::Toolkit::strcheck(argv[1], "-h")){
		std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
		std::cout << "OR: " << argv[0] << " -h" << std::endl;
		std::cout << "OR: " << argv[0] << std::endl;
		std::cout << "Default port is 63431" << std::endl;
		exit(0);
	}else if(argc == 2){
		sscanf(argv[1], "%d", &port);
	}
    Log::info("Starting...");
    Log::info("Setting up server socket...");
    Log::info("\t-> Creating socket");
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("\t   Error when creating socket");
		exit(1);
	}
	struct sockaddr_in server_addr;
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	Log::info(std::string("\t-> Binding to 127.0.0.1:") + Toolkit::itostr(port));
	if(bind(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
		perror("\t   Error when binding to port");
		exit(1);
	}
	Log::info("\t-> Listening on socket with interval 5...");
	if(listen(sock, 5) < 0){
		perror("\t   Error when listening");
		exit(1);
	}
	/*Log::info("\t-> Putting socket into non-blocking mode");
	if(!set_blocking_mode(sock, true)){
		Log::info("\t   Failed.");
		exit(1);
	}*/
	Log::info("Server socket set up successfully.");
	boost::thread th(check_thread);
	while(accept_new){
		Log::server("Waiting for new connection...");
		struct sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		int clientsock = accept(sock, (struct sockaddr*) &client_addr, &client_addr_len);
		if(clientsock >= 0){
			char buff[50];
			sprintf(buff, "%d.%d.%d.%d",
				int(client_addr.sin_addr.s_addr&0xFF),
				int((client_addr.sin_addr.s_addr&0xFF00)>>8),
				int((client_addr.sin_addr.s_addr&0xFF0000)>>16),
				int((client_addr.sin_addr.s_addr&0xFF000000)>>24));
			Log::server("Got connection from " + std::string(buff));
			Connection * connection = new Connection(clientsock);
			connections.push_back(connection);
			boost::thread * connection_thread = new boost::thread(&Wicher::DB::Connection::run, connection);
			connection->thread = connection_thread;
			Log::server("Thread spawned.");
		}
	}
	check_thread_run = false;
	Log::info("Waiting for all connections to die...");
	for(unsigned int i = 0; i < connections.size(); ++i){
		Log::info(std::string("Terminating connection ") + Toolkit::itostr(i+1) + std::string("/") + Toolkit::itostr(connections.size()));
		connections[i]->thread->join();
	}
    Log::info("Quitting...");
	close(sock);
    return 0;
}
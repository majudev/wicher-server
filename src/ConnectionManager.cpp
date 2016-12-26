#include "ConnectionManager.h"
#define MAX_BUFF 65535

Wicher::DB::ConnectionManager::ConnectionManager(int port){
	Log::info("Creating socket for server");
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("Error when creating socket");
		exit(1);
	}
	struct sockaddr_in server_addr;
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	Log::info(std::string("Binding to 127.0.0.1:") + Toolkit::itostr(port));
	if(bind(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
		perror("Error when binding to port");
		exit(1);
	}
	Log::info("Listening on socket with interval 5...");
	if(listen(sock, 5) < 0){
		perror("Error when listening");
		exit(1);
	}
}

Wicher::DB::ConnectionManager::~ConnectionManager(){
    Log::server("Closing connection...");
    close(sock);
    Log::server("Done.");
}

int Wicher::DB::ConnectionManager::get_connection(){
    Log::info("Waiting for connection...");
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
	int clientsock = accept(sock, (struct sockaddr*) &client_addr, &client_addr_len);
	if(clientsock < 0){
		perror("Error when accepting connection");
	}else{
		char buff[50];
		sprintf(buff, "%d.%d.%d.%d",
			int(client_addr.sin_addr.s_addr&0xFF),
			int((client_addr.sin_addr.s_addr&0xFF00)>>8),
			int((client_addr.sin_addr.s_addr&0xFF0000)>>16),
			int((client_addr.sin_addr.s_addr&0xFF000000)>>24));
		Log::server("Got connection from " + std::string(buff));
	}
	return clientsock;
}
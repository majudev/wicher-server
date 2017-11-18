#include "DBman.h"
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace spd = spdlog;

DBman::DBman(){
#ifdef SPDLOG_ENABLE_SYSLOG
    auto console = spd::syslog_logger("db", "Wicher-Server", LOG_PID);
#else
    auto console = spd::stdout_color_mt("db");
#endif
	this->console = console;
	//init authdb
}

void DBman::reg(int sock){
	std::sort(this->login_queue.begin(), this->login_queue.end());
	if(std::binary_search(this->login_queue.begin(), this->login_queue.end(), sock) || this->instances.find(sock) != this->instances.end()){
		struct sockaddr_in address;
		socklen_t addrlen = sizeof(address);
		getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		console->warn("Host {0}:{1} is already registered!", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
		return;
	}
	this->login_queue.push_back(sock);
}

void DBman::drop(int sock){
	std::sort(this->login_queue.begin(), this->login_queue.end());
	std::vector<int>::iterator iter = std::find(this->login_queue.begin(), this->login_queue.end(), sock);
	std::map<int,DatabaseManager*>::iterator iter2 = this->instances.find(sock);
	if(!(iter != this->login_queue.end() || iter2 != this->instances.end())){
		struct sockaddr_in address;
		socklen_t addrlen = sizeof(address);
		getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		console->warn("Host {0}:{1} isn't registered!", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
		return;
	}
	this->auth.logout(this->usernames[sock].c_str());
	if(iter != this->login_queue.end()){
		this->login_queue.erase(iter);
	}else{
		delete this->instances[sock];
		this->instances.erase(iter2);
	}
}

#include "DBman.h"
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

bool DBman::perform(int sock){
	//get data
	std::sort(this->login_queue.begin(), this->login_queue.end());
	std::vector<int>::iterator iter = std::find(this->login_queue.begin(), this->login_queue.end(), sock);
	if(iter != this->login_queue.end()){
		//if login request, check in AuthDB and register in instances
		//if not, send login request
	}else{
		//logged in, perform as in MessageParser using registered DB
	}
}
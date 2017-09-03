#include "DBman.h"
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#define MAX_BUFF 65535

bool DBman::send_msg(int sock, std::string message){
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);
	if(message.size() + 1 > MAX_BUFF){
        getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        console->warn("Failed to send message to {0}:{1} (message too big)", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        return false;
    }
    uint16_t msize = message.size() + 1;
    int res = send(sock, &msize, 2, 0);
    if(res != 2){
        getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        console->debug("Failed to send message to {0}:{1} (cannot send reported message size)", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        return false;
    }
	res = 0;
    while(res < msize){
        int res_tmp = send(sock, message.c_str() + res, msize - res, 0);
        if(res_tmp < 0){
            getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        	console->debug("Failed to send message to {0}:{1} (error when sending)", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            return false;
        }
        res += res_tmp;
    }
	return true;
}

bool DBman::perform(int sock){
	//message download part
	uint16_t msize;
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);
    int res = recv(sock, &msize, 2, 0);
    if(res != 2){
		getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        console->debug("Failed to recv message from {0}:{1} (cannot get reported message size)", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
		return false;
    }
    std::string message;
	char buffer[1025];
    res = 0;
    while(res < msize){
        int res_tmp = recv(sock, buffer, 1024, 0);
        if(res_tmp < 0){
            getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
			console->debug("Failed to recv message from {0}:{1} (error when getting message)", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
			return false;
        }
        buffer[res_tmp] = '\0';
        res += res_tmp;
        message += buffer;
    }
    if(res != msize){
		getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		console->debug("Failed to recv message from {0}:{1} (reported size is {2} while real is {3})", inet_ntoa(address.sin_addr), ntohs(address.sin_port), res, msize);
		return false;
	}
	
	//message handling part
	rapidjson::Document document;
	if(document.Parse(message.c_str()).HasParseError()){
		getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		this->console->debug("Error when parsing message from {0}:{1} (offset {3}): {4}", inet_ntoa(address.sin_addr), ntohs(address.sin_port), document.GetErrorOffset(), GetParseError_En(document.GetParseError()));
		return false;
	}
	if(!document.HasMember("request") || !document["request"].IsString()){
		getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		this->console->debug("Request from {0}:{1} doesn't have valid 'request' string", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
		this->send_msg(sock, "{\"response\":\"wrong_json\",\"longtext\":\"'request' is not valid string.\"}");
		return false;
	}
	const char * request_str = document["request"].GetString();
	
	std::sort(this->login_queue.begin(), this->login_queue.end());
	std::vector<int>::iterator iter = std::find(this->login_queue.begin(), this->login_queue.end(), sock);
	if(iter != this->login_queue.end()){
		//if not logged in check if it's login request
		if(!strcmp(request_str, "login")){
			//yes it is, try to login
			if(!document.HasMember("login") || !document["login"].IsString() ||
			     !document.HasMember("password") || !document["password"].IsString()){
				this->send_msg(sock, "{\"response\":\"wrong_json\",\"longtext\":\"'login' and/or 'password' is not a valid string.\"}");
				return false;
			}
			if(auth.login(document["login"].GetString(), document["password"].GetString())) return send_msg(sock, "{\"response\":\"ok\"}");
			else return send_msg(sock, "{\"response\":\"fail\"}");
		}else return this->send_msg(sock, "{\"response\":\"login_required\",\"longtext\":\"Please login before accessing the database.\"}"); //it isn't - send info
	}else{
		//logged in, perform as in MessageParser using registered DB
	}
}
#include "Connection.h"
#include <cstdlib>
#define MAX_BUFF 65535

Wicher::DB::Connection::Connection(int sock) : clientsock(sock), thread(NULL), closed(false){
	this->logger = new ConnectionLogger();
	this->mp = new MessageParser(logger);
}

Wicher::DB::Connection::~Connection(){
	this->thread->join();
	delete this->thread;
    close(clientsock);
	logger->log("Shuted down.");
}

void Wicher::DB::Connection::run(){
	logger->log("Thread spawned.");
	std::string msg;
	std::string response;
	while(this->is_up()){
		msg = recv_msg();
		if(msg != std::string("BYE")){
			response = mp->parse(msg);
			send_msg(response);
		}else break;
	}
	this->closed = true;
}

std::string Wicher::DB::Connection::recv_msg(){
	if(!this->is_up()) return std::string();
    uint16_t msize;
    int res = recv(clientsock, &msize, 2, 0);
    if(res != 2){
        logger->log("Failed to recv message (cannot recv msg size)");
        return std::string();
    }
    std::string tr;
	char buffer[1025];
    res = 0;
    while(res < msize){
        int res_tmp = recv(clientsock, buffer, 1024, 0);
        if(res_tmp < 0){
            logger->log("Failed to recv message (error when receiving content)");
            break;
        }
        buffer[1024] = '\0';
        res += res_tmp;
        tr += buffer;
    }
    if(res == msize){
        return tr;
    }else return std::string();
}

bool Wicher::DB::Connection::send_msg(std::string msg){
	if(!this->is_up()) return false;
    if(msg.size()+1 > MAX_BUFF){
        logger->log("Failed to send message (message too big)");
        return false;
    }
    uint16_t msize = msg.size() + 1;
    int res = send(clientsock, &msize, 2, 0);
    if(res != 2){
        logger->log("Failed to send message (cannot send msg size)");
        return false;
    }
	res = 0;
    while(res < msize){
        int res_tmp = send(clientsock, msg.c_str(), msize, 0);
        if(res_tmp < 0){
            logger->log("Failed to send message (error when sending content)");
            break;
        }//else std::cerr << "Sent: " << msg << std::endl;
        res += res_tmp;
    }
	return res == msize;
}

bool Wicher::DB::Connection::is_up(){
	if(this->closed) return false;
    int error = 0;
    socklen_t len = sizeof (error);
    getsockopt (clientsock, SOL_SOCKET, SO_ERROR, &error, &len);
    if(error != 0){
        logger->log(std::string("Socket error: ") + std::string(strerror(error)));
        return false;
    }
    return true;
}

#include "Connection.h"
#define MAX_BUFF 65535

Wicher::DB::Connection::Connection(int sock) : clientsock(sock), thread(NULL){}

Wicher::DB::Connection::~Connection(){
	this->thread->join();
	delete this->thread;
#ifdef WIN
    closesocket(clientsock);
#elif defined(UNI)
    close(clientsock);
#endif
}

void Wicher::DB::Connection::run(){
	this->thread = new boost::thread(&Wicher::DB::Connection::connection_thread, this);
}

void Wicher::DB::Connection::connection_thread(){
}

std::string Wicher::DB::Connection::recv_msg(){
    uint16_t msize;
#ifdef WIN
    int res = recv(clientsock, (char*) &msize, 2, 0);
#else
    int res = recv(clientsock, &msize, 2, 0);
#endif
    if(res != 2){
        Log::server("Failed to recv message (cannot recv msg size)");
        return std::string();
    }
    std::string tr;
	char buffer[1025];
    res = 0;
    while(res < msize){
        int res_tmp = recv(clientsock, buffer, 1024, 0);
        if(res_tmp < 0){
            Log::server("Failed to recv message (error when receiving content)");
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
    if(msg.size()+1 > MAX_BUFF){
        Log::server("Failed to send message (message too big)");
        return false;
    }
    uint16_t msize = msg.size() + 1;
#ifdef WIN
    int res = send(clientsock, (char*) &msize, 2, 0);
#else
    int res = send(clientsock, &msize, 2, 0);
#endif
    if(res != 2){
        Log::server("Failed to send message (cannot send msg size)");
        return false;
    }
	res = 0;
    while(res < msize){
        int res_tmp = send(clientsock, msg.c_str(), msize, 0);
        if(res_tmp < 0){
            Log::server("Failed to send message (error when sending content)");
            break;
        }//else std::cerr << "Sent: " << msg << std::endl;
        res += res_tmp;
    }
	return res == msize;
}

bool Wicher::DB::Connection::is_up(){
#ifdef WIN
    char buffer[256];
    int buffersize = 256;
    if(getsockopt(sock, SOL_SOCKET, SO_ERROR, buffer, &buffersize)){
        Log::client(std::string("Socket error."));
        return false;
    }
    return true;
#elif defined(UNI)
    int error = 0;
    socklen_t len = sizeof (error);
    getsockopt (clientsock, SOL_SOCKET, SO_ERROR, &error, &len);
    if(error != 0){
        Log::client(std::string("Socket error: ") + std::string(strerror(error)));
        return false;
    }
    return true;
#endif
}

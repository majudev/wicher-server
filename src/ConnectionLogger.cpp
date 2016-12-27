#include "ConnectionLogger.h"
#include <cstdlib>
#define MAX_BUFF 65535

Wicher::DB::ConnectionLogger::ConnectionLogger(){
	this->prefix = "[C" + UID::get_singleton().get_next_str() + "] ";
}

Wicher::DB::ConnectionLogger::~ConnectionLogger(){}

void Wicher::DB::ConnectionLogger::log(std::string msg){
	Wicher::DB::Log::server(this->prefix + msg);
}

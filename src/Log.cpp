#include "Log.h"

void Wicher::DB::Log::log(std::string prefix, std::string msg){
    time_t rawtime;
    struct tm * timeinfo;

    std::time(&rawtime);
    timeinfo = localtime(&rawtime);

    char buffer[20];
    strftime(buffer, 20, "%H:%M:%S", timeinfo);
    std::cout << "[W/DB/L][" << buffer << "] [" << prefix << "] " << msg << std::endl;
}

void Wicher::DB::Log::client(std::string msg){
    Log::log(" CLIENT ", msg);
}

void Wicher::DB::Log::server(std::string msg){
    Log::log(" SERVER ", msg);
}

void Wicher::DB::Log::info(std::string msg){
    Log::log("  INFO  ", msg);
}

#include "DatabaseManager.h"

#include "spdlog/spdlog.h"

#include "buildconfig.h"

#include <cstring>

namespace spd = spdlog;

DatabaseManager::DatabaseManager(const char * username){
	strcpy(this->consolename, "db-");
	strcat(this->consolename, username);
#ifdef SPDLOG_ENABLE_SYSLOG
    auto console = spd::syslog_logger(this->consolename, "Wicher-Server", LOG_PID);
#else
    auto console = spd::stdout_color_mt(this->consolename);
#endif
	this->console = console;
}

DatabaseManager::~DatabaseManager(){
	spdlog::drop(this->consolename);
}
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include "spdlog/spdlog.h"

class Config{
	public:
    	static Config * getSingleton();
        bool noadmin;
        int port;
	std::string control_path;
	std::string auth_db_path;

	private:
    	Config();
    	Config( const Config & );
};

#endif

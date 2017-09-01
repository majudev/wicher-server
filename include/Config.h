#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include "spdlog/spdlog.h"

class Config{
	public:
    	static Config * getSingleton();
		bool noadmin;
	
	private:
    	Config();
    	Config( const Config & );
};

#endif
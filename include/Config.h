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
		std::string get_auth_db_path();
		std::string get_json_db_path();
		std::string get_stuff_path();
		bool set_datadir(std::string datadir);
		bool create_user_db_dir(std::string username);
		bool remove_user_db_dir(std::string username);

	private:
    	Config();
    	Config( const Config & );
		std::string datadir;
};

#endif

#include "Config.h"
#include "Toolkit.h"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>

Config::Config() : noadmin(false), control_path("/tmp/wicher-server-socket"), datadir("/tmp"){
	const char *homedir;
	if ((homedir = getenv("HOME")) == NULL) {
		homedir = getpwuid(getuid())->pw_dir;
	}
	std::string tmp = homedir;
	tmp += "/.wicher-server";
	this->set_datadir(tmp);
}
Config * Config::getSingleton() {
    static Config singleton;
    return &singleton;
}

bool Config::set_datadir(std::string datadir){
	std::string tmp = datadir;
	bool flag = false;
	struct stat st = {0};
	if (stat(tmp.c_str(), &st) == -1){
    	flag = flag || mkdir(tmp.c_str(), 0700);
	}
	tmp += "/db";
	if (stat(tmp.c_str(), &st) == -1){
    	flag = flag || mkdir(tmp.c_str(), 0700);
	}
	tmp = datadir;
	tmp += "/stuff";
	if (stat(tmp.c_str(), &st) == -1){
    	flag = flag || mkdir(tmp.c_str(), 0700);
	}
	if(!flag){
		this->datadir = datadir;
		return true;
	}
	return false;
}

bool Config::create_user_db_dir(std::string username){
	std::string dirname = this->datadir;
	dirname += "/db/";
	dirname += username;
	struct stat st = {0};
	if (stat(dirname.c_str(), &st) == -1){
    	return !mkdir(dirname.c_str(), 0700);
	}else return false;
}

bool Config::remove_user_db_dir(std::string username){
	std::string dirname = this->datadir;
	dirname += "/db/";
	dirname += username;
	return !Toolkit::recursive_delete(dirname.c_str());
}

std::string Config::get_auth_db_path(){
	return this->datadir + "/auth.db";
}

std::string Config::get_db_path(){
	return this->datadir + "/db";
}

std::string Config::get_stuff_path(){
	return this->datadir + "/stuff";
}
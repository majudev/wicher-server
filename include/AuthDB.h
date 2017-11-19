#ifndef AUTHDB_H
#define AUTHDB_H

#include <string>
#include "spdlog/spdlog.h"
#include <vector>

#include "buildconfig.h"

#include "Config.h"
#include "DatabaseManager.h"
#include "rapidjson/document.h"

class AuthDB{
	public:
    	enum RegError { REG_ALREADY_EXISTS, REG_INTERNAL_DB_ERROR, REG_OK };
    	enum AuthError { AUTH_WRONG_CREDENTIALS, AUTH_NO_USER, AUTH_INTERNAL_DB_ERROR, AUTH_OK };
	
		AuthDB();
		RegError reg(const char * username, const char * password); //add to login queue
		AuthError login(const char * username, const char * password); //check if credentials are correct
		void logout(const char * username); //log out the user
		void drop(const char * username); //remove from database
		
		void make_admin(const char * username); //makes user admin
		bool is_admin(const char * username); //check if user is admin
		bool is_logged_in(const char * username); //check if user is logged in
		DatabaseManager * get_dbman(const char * username); //get DatabaseManager instance
	
		~AuthDB();

	private:
		std::shared_ptr<spdlog::logger> console;
		rapidjson::Document document;
		std::vector<std::string> logged_in;
};

#endif
#ifndef DBMAN_H
#define DBMAN_H

#include <string>
#include "spdlog/spdlog.h"
#include <map>
#include <vector>

#include "buildconfig.h"

#include "AuthDB.h"
#include "DatabaseManager.h"
#include "JSONDatabase.h"

class DBman{
	public:
		static DBman * getSingleton();
		void reg(int sock); //add to login queue
		bool perform(int sock); //code from MessageParser
		void drop(int sock); //remove from queue/instances
		friend void * control_handler(void *);

	private:
		DBman();
		DBman(DBman const&);
		std::shared_ptr<spdlog::logger> console;
		std::vector<int> login_queue;
		std::map<int,std::string> usernames;
		std::map<int,DatabaseManager*> instances;
		AuthDB auth;
	
		bool send_msg(int sock, std::string message);
};

#endif
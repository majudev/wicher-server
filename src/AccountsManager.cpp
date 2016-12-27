#include "AccountsManager.h"

Wicher::DB::AccountsManager::AccountsManager(){}
Wicher::DB::AccountsManager::~AccountsManager(){}

Wicher::DB::AccountsManager & Wicher::DB::AccountsManager::get_singleton(){
    static AccountsManager singleton;
	return singleton;
}

bool Wicher::DB::AccountsManager::validate(std::string login, std::string password){
	return true;
}

std::string Wicher::DB::AccountsManager::get_database_path(std::string login){
	return std::string("/tmp/") + login + std::string(".db");
}
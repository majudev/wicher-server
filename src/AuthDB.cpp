#include "AuthDB.h"
#include <fstream>
#include <streambuf>
#include <algorithm>

#include "rapidjson/prettywriter.h"

#include "JSONDatabase.h"

namespace spd = spdlog;

AuthDB::AuthDB(){
#ifdef SPDLOG_ENABLE_SYSLOG
    auto console = spd::syslog_logger("auth", "Wicher-Server", LOG_PID);
#else
    auto console = spd::stdout_color_mt("auth");
#endif
	this->console = console;
	//init authdb
	console->info("Opening auth database...");
	std::ifstream t(Config::getSingleton()->get_auth_db_path());
	if(t){
		std::string str;
		t.seekg(0, std::ios::end);
		str.reserve(t.tellg());
		t.seekg(0, std::ios::beg);
		str.assign((std::istreambuf_iterator<char>(t)),
					std::istreambuf_iterator<char>());
		this->document.Parse(str.c_str());
		if(!this->document.IsObject()) this->document.SetObject();
	}else{
        document.SetObject();
    }
}

AuthDB::RegError AuthDB::reg(const char * username, const char * password){
    if(this->document.HasMember(username)) return REG_ALREADY_EXISTS;
    char buffer[512];
    strcpy(buffer, username);
    strcat(buffer, "/database.json");
    if(!Config::getSingleton()->create_user_db_dir(username)) return REG_INTERNAL_DB_ERROR;
    this->document.AddMember(rapidjson::Value(username, this->document.GetAllocator()).Move(), rapidjson::Value().Move(), this->document.GetAllocator());
    this->document[username].SetObject();
    this->document[username].AddMember(rapidjson::Value("password", this->document.GetAllocator()).Move(), rapidjson::Value(password, this->document.GetAllocator()).Move(), this->document.GetAllocator());
    this->document[username].AddMember(rapidjson::Value("active", this->document.GetAllocator()).Move(), rapidjson::Value(true).Move(), this->document.GetAllocator());
    this->document[username].AddMember(rapidjson::Value("db_engine", this->document.GetAllocator()).Move(), rapidjson::Value("JSON", this->document.GetAllocator()).Move(), this->document.GetAllocator());
    this->document[username].AddMember(rapidjson::Value("db_path", this->document.GetAllocator()).Move(), rapidjson::Value(buffer, this->document.GetAllocator()).Move(), this->document.GetAllocator());
    this->document[username].AddMember(rapidjson::Value("protomod", this->document.GetAllocator()).Move(), rapidjson::Value(false).Move(), this->document.GetAllocator());
    return REG_OK;
}

AuthDB::AuthError AuthDB::login(const char * username, const char * password){
	if(!this->document.HasMember(username)) return AUTH_NO_USER;
	if(!this->document[username].IsObject() ||
	   !this->document[username].HasMember("password") || !this->document[username]["password"].IsString() ||
	   !this->document[username].HasMember("active") || !this->document[username]["active"].IsBool() ||
	   !this->document[username].HasMember("db_engine") || !this->document[username]["db_engine"].IsString() ||
	   !this->document[username].HasMember("db_path") || !this->document[username]["db_path"].IsString()){
		this->drop(username);
		return AUTH_INTERNAL_DB_ERROR;
	}
    if(!this->document[username]["active"].GetBool()) return AUTH_INACTIVE;
	if(!strcmp(this->document[username]["password"].GetString(), password)){
        this->logged_in.push_back(std::string(username));
        console->critical("Logged in {0}", username);
        return AUTH_OK;
    }else return AUTH_WRONG_CREDENTIALS;
}

void AuthDB::logout(const char * username){
    std::sort(this->logged_in.begin(), this->logged_in.end());
    std::vector<std::string>::iterator iter = std::find(this->logged_in.begin(), this->logged_in.end(), std::string(username));
    if(iter != this->logged_in.end()) this->logged_in.erase(iter);
    console->critical("Logged out {0}", username);
}

bool AuthDB::drop(const char * username){
    if(!this->document.HasMember(username)) return false;
    if(!this->document[username].IsObject() ||
	   !this->document[username].HasMember("db_path") || !this->document[username]["db_path"].IsString()) return false;
    if(!Config::getSingleton()->remove_user_db_dir(this->document[username]["db_path"].GetString())) return false;
    this->document.RemoveMember(username);
    return true;
}

bool AuthDB::make_active(const char * username){
    if(!this->document.HasMember(username) || !this->document[username].IsObject() ||
	   !this->document[username].HasMember("active") || !this->document[username]["active"].IsBool()) return false;
    this->document[username]["active"] = true;
    return true;
}

bool AuthDB::make_inactive(const char * username){
    if(!this->document.HasMember(username) || !this->document[username].IsObject() ||
	   !this->document[username].HasMember("active") || !this->document[username]["active"].IsBool()) return false;
    this->document[username]["active"] = false;
    return true;
}

bool AuthDB::is_active(const char * username){
    if(!this->document.HasMember(username) || !this->document[username].IsObject() ||
	   !this->document[username].HasMember("active") || !this->document[username]["active"].IsBool()) return false;
    return this->document[username]["active"].GetBool();
}

bool AuthDB::is_logged_in(const char * username){
    std::sort(this->logged_in.begin(), this->logged_in.end());
    return std::find(this->logged_in.begin(), this->logged_in.end(), std::string(username)) != this->logged_in.end();
}

DatabaseManager * AuthDB::get_dbman(const char * username){
    if(!this->document.HasMember(username) || !this->document[username].IsObject() ||
	   !this->document[username].HasMember("db_engine") || !this->document[username]["db_engine"].IsString() ||
	   !this->document[username].HasMember("db_path") || !this->document[username]["db_path"].IsString()) return NULL;
    if(this->document[username].HasMember("protomod") && this->document[username]["protomod"].IsBool()
       && this->document[username]["protomod"].GetBool()){
        //protomod handling part
    }
    const char * db_engine = this->document[username]["db_engine"].GetString();
    const char * db_path = this->document[username]["db_path"].GetString();
    if(!strcmp(db_engine, "JSON")){
        JSONDatabase * db = new JSONDatabase(username, Config::getSingleton()->get_db_path() + "/" + std::string(db_path));
        return db;
    }else{
        console->warn("Unsupported db_engine '{0}' in AuthDB at username '{1}' - you should check it.", db_engine, username);
        return NULL;
    }
}

std::vector<std::string> AuthDB::get_users(){
    std::vector<std::string> users;
    for (auto& m : document.GetObject()){
        users.push_back(m.name.GetString());
    }
    return users;
}

AuthDB::~AuthDB(){
	std::ofstream f(Config::getSingleton()->get_auth_db_path());
	rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    document.Accept(writer);
	f << sb.GetString();
	console->info("Saved auth database");
}

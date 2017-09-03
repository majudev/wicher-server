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
	std::ifstream t("auth.db");
	std::string str;
	t.seekg(0, std::ios::end);   
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);
	str.assign((std::istreambuf_iterator<char>(t)),
				std::istreambuf_iterator<char>());
	this->document.Parse(str.c_str());
	if(!this->document.IsObject()) this->document.SetObject();
}

AuthDB::RegError AuthDB::reg(const char * username, const char * password){
    if(this->document.HasMember(username)) return REG_ALREADY_EXISTS;
    char buffer[512];
    strcpy(buffer, username);
    strcat(buffer, ".jsondb");
    this->document.AddMember(rapidjson::Value(username, this->document.GetAllocator()).Move(), rapidjson::Value().Move(), this->document.GetAllocator());
    this->document[username].SetObject();
    this->document[username].AddMember(rapidjson::Value("password", this->document.GetAllocator()).Move(), rapidjson::Value(password, this->document.GetAllocator()).Move(), this->document.GetAllocator());
    this->document[username].AddMember(rapidjson::Value("admin", this->document.GetAllocator()).Move(), rapidjson::Value(false).Move(), this->document.GetAllocator());
    this->document[username].AddMember(rapidjson::Value("db_engine", this->document.GetAllocator()).Move(), rapidjson::Value("JSON", this->document.GetAllocator()).Move(), this->document.GetAllocator());
    this->document[username].AddMember(rapidjson::Value("db_path", this->document.GetAllocator()).Move(), rapidjson::Value(buffer, this->document.GetAllocator()).Move(), this->document.GetAllocator());
    return REG_OK;
}

AuthDB::AuthError AuthDB::login(const char * username, const char * password){
	if(!this->document.HasMember(username)) return AUTH_NO_USER;
	if(!this->document[username].IsObject() ||
	   !this->document[username].HasMember("password") || !this->document[username]["password"].IsString() ||
	   !this->document[username].HasMember("admin") || !this->document[username]["admin"].IsBool() ||
	   !this->document[username].HasMember("db_engine") || !this->document[username]["db_engine"].IsString() ||
	   !this->document[username].HasMember("db_path") || !this->document[username]["db_path"].IsString()){
		this->drop(username);
		return AUTH_INTERNAL_DB_ERROR;
	}
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

void AuthDB::drop(const char * username){
    if(!this->document.HasMember(username)) return;
    this->document.RemoveMember(username);
}

void AuthDB::make_admin(const char * username){
    if(!this->document.HasMember(username) || !this->document[username].IsObject() ||
	   !this->document[username].HasMember("admin") || !this->document[username]["admin"].IsBool()) return;
    this->document[username]["admin"] = true;
}

bool AuthDB::is_admin(const char * username){
    if(!this->document.HasMember(username) || !this->document[username].IsObject() ||
	   !this->document[username].HasMember("admin") || !this->document[username]["admin"].IsBool()) return false;
    return this->document[username]["admin"].GetBool();
}

bool AuthDB::is_logged_in(const char * username){
    std::sort(this->logged_in.begin(), this->logged_in.end());
    return std::find(this->logged_in.begin(), this->logged_in.end(), std::string(username)) != this->logged_in.end();
}

DatabaseManager * AuthDB::get_dbman(const char * username){
    if(!this->document.HasMember(username) || !this->document[username].IsObject() ||
	   !this->document[username].HasMember("db_engine") || !this->document[username]["db_engine"].IsString() ||
	   !this->document[username].HasMember("db_path") || !this->document[username]["db_path"].IsString()) return NULL;
    const char * db_engine = this->document[username]["db_engine"].GetString();
    const char * db_path = this->document[username]["db_path"].GetString();
    if(!strcmp(db_engine, "JSON")){
        JSONDatabase * db = new JSONDatabase(username, std::string(db_path));
        return db;
    }else{
        console->warn("Unsupported db_engine '{0}' in AuthDB at username '{1}' - you should check it.", db_engine, username);
        return NULL;
    }
}

AuthDB::~AuthDB(){
	std::ofstream f("auth.db");
	rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    document.Accept(writer);
	f << sb.GetString();
	console->info("Saved auth database");
}
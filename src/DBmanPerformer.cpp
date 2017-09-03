#include "DBman.h"
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#define MAX_BUFF 65535

#define REQUEST_UNDEF "{\"response\":\"undefined_request\",\"longtext\":\"Undefined request.\"}"
#define RESP_OK "{\"response\":\"ok\"}"

bool DBman::send_msg(int sock, std::string message){
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);
	if(message.size() + 1 > MAX_BUFF){
        getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        console->warn("Failed to send message to {0}:{1} (message too big)", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        return false;
    }
    uint16_t msize = message.size() + 1;
    int res = send(sock, &msize, 2, 0);
    if(res != 2){
        getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        console->debug("Failed to send message to {0}:{1} (cannot send reported message size)", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        return false;
    }
	res = 0;
    while(res < msize){
        int res_tmp = send(sock, message.c_str() + res, msize - res, 0);
        if(res_tmp < 0){
            getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        	console->debug("Failed to send message to {0}:{1} (error when sending)", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            return false;
        }
        res += res_tmp;
    }
	return true;
}

bool DBman::perform(int sock){
	//message download part
	uint16_t msize;
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);
    int res = recv(sock, &msize, 2, 0);
    if(res != 2){
		getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        console->debug("Failed to recv message from {0}:{1} (cannot get reported message size)", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
		return false;
    }
    std::string message;
	char buffer[1025];
    res = 0;
    while(res < msize){
        int res_tmp = recv(sock, buffer, 1024, 0);
        if(res_tmp < 0){
            getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
			console->debug("Failed to recv message from {0}:{1} (error when getting message)", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
			return false;
        }
        buffer[res_tmp] = '\0';
        res += res_tmp;
        message += buffer;
    }
    if(res != msize){
		getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		console->debug("Failed to recv message from {0}:{1} (reported size is {2} while real is {3})", inet_ntoa(address.sin_addr), ntohs(address.sin_port), res, msize);
		return false;
	}
	
	//message handling part
	rapidjson::Document document;
	if(document.Parse(message.c_str()).HasParseError()){
		getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		this->console->debug("Error when parsing message from {0}:{1} (offset {3}): {4}", inet_ntoa(address.sin_addr), ntohs(address.sin_port), document.GetErrorOffset(), GetParseError_En(document.GetParseError()));
		return false;
	}
	if(!document.HasMember("request") || !document["request"].IsString()){
		getpeername(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		this->console->debug("Request from {0}:{1} doesn't have valid 'request' string", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
		this->send_msg(sock, "{\"response\":\"wrong_json\",\"longtext\":\"'request' is not valid string.\"}");
		return false;
	}
	const char * request = document["request"].GetString();
	
	std::sort(this->login_queue.begin(), this->login_queue.end());
	std::vector<int>::iterator iter = std::find(this->login_queue.begin(), this->login_queue.end(), sock);
	if(iter != this->login_queue.end()){
		//if not logged in check if it's login request
		if(!strcmp(request, "login")){
			//yes it is, try to login
			if(!document.HasMember("login") || !document["login"].IsString() ||
			     !document.HasMember("password") || !document["password"].IsString()){
				//instead of return true; we use return value of send_msg
				//because if something goes wrong with sending, socket is anyway closed - so function should return false
				return this->send_msg(sock, "{\"response\":\"wrong_json\",\"longtext\":\"'login' and/or 'password' is not a valid string.\"}");
			}
			if(auth.is_logged_in(document["login"].GetString())){
				return this->send_msg(sock, "{\"response\":\"already_logged_in\",\"longtext\":\"User is already logged in.\"}");
			}
			AuthDB::AuthError auth_err = auth.login(document["login"].GetString(), document["password"].GetString());
			if(auth_err == AuthDB::AUTH_OK){
				//remove from queue
				std::sort(this->login_queue.begin(), this->login_queue.end());
				std::vector<int>::iterator iter = std::find(this->login_queue.begin(), this->login_queue.end(), sock);
				this->login_queue.erase(iter);
				//add DatabaseManager instance
				this->instances[sock] = auth.get_dbman(document["login"].GetString());
				//register username
				this->usernames[sock] = document["login"].GetString();
				//send response
				return send_msg(sock, "{\"response\":\"ok\"}");
			}else if(auth_err == AuthDB::AUTH_WRONG_CREDENTIALS) return send_msg(sock, "{\"response\":\"fail\"}");
			else if(auth_err == AuthDB::AUTH_NO_USER) return send_msg(sock, "{\"response\":\"fail_no_user\"}");
			else if(auth_err == AuthDB::AUTH_INTERNAL_DB_ERROR) return send_msg(sock, "{\"response\":\"fail_internal_db_error\"}");
		}else return this->send_msg(sock, "{\"response\":\"login_required\",\"longtext\":\"Please login before accessing the database.\"}"); //it isn't - send info
	}else{
		//logged in, perform as in MessageParser using registered DB
		//check for request_type. all requests with request_type go there
		if(document.HasMember("request_type") || !document["request_type"].IsString()){
			const char * request_type = document["request_type"].GetString();
			if(!strcmp(request, "get")){
				if(!strcmp(request_type, "items")){
					if(!document.HasMember("type") || !document["type"].IsString()) return this->send_msg(sock, REQUEST_UNDEF);
					const char * type = document["type"].GetString();
					return this->send_msg(sock, this->instances[sock]->get_items(type));
				}else if(!strcmp(request_type, "types")){
					return this->send_msg(sock, this->instances[sock]->get_types());
				}else if(!strcmp(request_type, "wzs")){
					return this->send_msg(sock, this->instances[sock]->get_wzs());
				}else if(!strcmp(request_type, "pzs")){
					return this->send_msg(sock, this->instances[sock]->get_pzs());
				}else if(!strcmp(request_type, "history")){
					return this->send_msg(sock, this->instances[sock]->get_history());
				}else if(!strcmp(request_type, "wz_items")){
					if(!document.HasMember("wz_id") || !document["wz_id"].IsInt()) return this->send_msg(sock, REQUEST_UNDEF);
					int wz_id = document["wz_id"].GetInt();
					return this->send_msg(sock, this->instances[sock]->get_wz_items(wz_id));
				}
			}else if(!strcmp(request, "register")){
				if(!strcmp(request_type, "item")){
					if(!document.HasMember("type") || !document["type"].IsString()) return this->send_msg(sock, REQUEST_UNDEF);
					if(!document.HasMember("comment") || !document["comment"].IsString()) return this->send_msg(sock, REQUEST_UNDEF);
					const char * type = document["type"].GetString();
					const char * comment = document["comment"].GetString();
					DatabaseManager::ErrorID eid;
                    int id = this->instances[sock]->get_next_item_id(type, &eid);
					if(id < 0) return this->send_msg(sock, this->instances[sock]->error(eid));
					if(!this->instances[sock]->create_item(id, type, comment, &eid)){
						return this->send_msg(sock, this->instances[sock]->error(eid));
					}
					std::string response("{\"response\":\"ok\",\"item\":");
					response += this->instances[sock]->get_item_json(id, type, &eid);
					response += "}";
					return this->send_msg(sock, response);
				}else if(!strcmp(request_type, "type")){
					if(!document.HasMember("id") || !document["id"].IsString()) return this->send_msg(sock, REQUEST_UNDEF);
					if(!document.HasMember("name") || !document["name"].IsString()) return this->send_msg(sock, REQUEST_UNDEF);
					if(!document.HasMember("comment") || !document["comment"].IsString()) return this->send_msg(sock, REQUEST_UNDEF);
					const char * id = document["id"].GetString();
					const char * name = document["name"].GetString();
					const char * comment = document["comment"].GetString();
					DatabaseManager::ErrorID eid;
					if(!this->instances[sock]->create_type(id, name, comment, &eid)){
						return this->send_msg(sock, this->instances[sock]->error(eid));
					}
					std::string response("{\"response\":\"ok\",\"type\":");
					response += this->instances[sock]->get_type_json(id, &eid);
					response += "}";
					return this->send_msg(sock, response);
				}/*else if(!strcmp(request_type, "wz")){
				}else if(!strcmp(request_type, "pz"){
				}*/ //TODO
			}else if(!strcmp(request, "drop")){
				if(!strcmp(request_type, "item")){
					if(!document.HasMember("id") || !document["id"].IsInt()) return this->send_msg(sock, REQUEST_UNDEF);
					if(!document.HasMember("type") || !document["type"].IsString()) return this->send_msg(sock, REQUEST_UNDEF);
					int id = document["id"].GetInt();
					const char * type = document["type"].GetString();
					DatabaseManager::ErrorID eid;
					if(!this->instances[sock]->drop_item(id, type, &eid)){
						return this->send_msg(sock, this->instances[sock]->error(eid));
					}
					return this->send_msg(sock, RESP_OK);
				}else if(!strcmp(request_type, "type")){
					if(!document.HasMember("id") || !document["id"].IsString()) return this->send_msg(sock, REQUEST_UNDEF);
					const char * id = document["id"].GetString();
					DatabaseManager::ErrorID eid;
					if(!this->instances[sock]->drop_type(id, &eid)){
						return this->send_msg(sock, this->instances[sock]->error(eid));
					}
					return this->send_msg(sock, RESP_OK);
				}else if(!strcmp(request_type, "wz")){
					if(!document.HasMember("id") || !document["id"].IsInt()) return this->send_msg(sock, REQUEST_UNDEF);
					int id = document["id"].GetInt();
					DatabaseManager::ErrorID eid;
					if(!this->instances[sock]->drop_wz(id, &eid)){
						return this->send_msg(sock, this->instances[sock]->error(eid));
					}
					return this->send_msg(sock, RESP_OK);
				}else if(!strcmp(request_type, "pz")){
					if(!document.HasMember("id") || !document["id"].IsInt()) return this->send_msg(sock, REQUEST_UNDEF);
					int id = document["id"].GetInt();
					DatabaseManager::ErrorID eid;
					if(!this->instances[sock]->drop_pz(id, &eid)){
						return this->send_msg(sock, this->instances[sock]->error(eid));
					}
					return this->send_msg(sock, RESP_OK);
				}
			}/*else if(!strcmp(request, "change")){
			}*/
		}
		//if there are any requests without request_type, they will appear here
		return this->send_msg(sock, REQUEST_UNDEF);
	}
}
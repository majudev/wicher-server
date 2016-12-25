#include "MessageParser.h"

Wicher::DB::MessageParser::MessageParser(){
    db = new DatabaseManager("database.db");
}

Wicher::DB::MessageParser::~MessageParser(){
    delete db;
}

std::string r(){
    char buffer[256];
    int pos = 0;
    while(1){
        char ch = std::getchar();
        if(ch != '\n'){
            buffer[pos] = ch;
        }else{
            buffer[pos] = '\0';
            break;
        }
        ++pos;
    }
    std::string str(buffer);
    return str;
}

std::string Wicher::DB::MessageParser::parse(std::string msg){
    std::string passwd("74234e98afe7498fb5daf1f36ac2d78acc339464f950703b8c019892f982b90b");
    json_error_t error;
    json_t * master_root = json_loads(msg.c_str(), 0, &error);
    if(!master_root){
        Log::server(std::string("Error when parsing message:") + std::string(error.text));
        Log::server(std::string("\tSource:") + std::string(error.source));
        Log::server(std::string("\tLine:") + Toolkit::itostr(error.line));
        Log::server(std::string("\tColumn:") + Toolkit::itostr(error.column));
        Log::server(std::string("\tPosition [bytes]:") + Toolkit::itostr(error.position));
        return std::string("{\"response\":\"wrong_json\",\"longtext\":\"Invalid root.\"}");
    }else{
        const char * request_str = json_string_value(json_object_get(master_root, "request"));
        if(request_str){
            std::string request(request_str);
            if(request == std::string("login")){
                const char * login_str = json_string_value(json_object_get(master_root, "login"));
                const char * password_str = json_string_value(json_object_get(master_root, "password"));
                if(login_str && password_str){
                    std::string login(login_str);
                    std::string password(password_str);
                    json_t * root = json_object();
                    json_t * response;
                    if(login == std::string("null") && password == std::string("74234e98afe7498fb5daf1f36ac2d78acc339464f950703b8c019892f982b90b")){
                        response = json_string("ok");
                    }else{
                        response = json_string("fail");
                    }
                    json_object_set_new(root, "response", response);
                    char * tr_str = json_dumps(root, JSON_COMPACT);
                    std::string tr(tr_str);
                    free(tr_str);
                    free(root);
                    free(master_root);
                    return tr;
                }
            }else if(request == std::string("get")){
                const char * request_type_str = json_string_value(json_object_get(master_root, "request_type"));
                if(request_type_str){
                    std::string request_type(request_type_str);
                    if(request_type == std::string("items")){
                        const char * type_str = json_string_value(json_object_get(master_root, "type"));
                        if(type_str){
                            std::string type(type_str);
                            free(master_root);
                            return db->get_items(type);
                        }
                    }else if(request_type == std::string("types")){
                        free(master_root);
                        return db->get_types();
                    }else if(request_type == std::string("wzs")){
                        free(master_root);
                        return db->get_wzs();
                    }else if(request_type == std::string("pzs")){
                        free(master_root);
                        return db->get_pzs();
                    }else if(request_type == std::string("history")){
                        free(master_root);
                        return db->get_history();
                    }else if(request_type == std::string("wz_items")){
                        json_t * id_ptr = json_object_get(master_root, "wz_id");
                        if(json_is_integer(id_ptr)){
                            int id = json_integer_value(id_ptr);
                            free(master_root);
                            return db->get_wz_items(id);
                        }else return db->error(DatabaseManager::WRONG_ENTRY);
                    }
                    free(master_root);
                    return std::string("{\"response\":\"wrong_query\",\"longtext\":\"Wrong 'request_type' in `get` request.\"}");
                }
                free(master_root);
                return std::string("{\"response\":\"wrong_json\",\"longtext\":\"'request_type' in `get` request is not valid string.\"}");
            }else if(request == std::string("register")){
                const char * request_type_str = json_string_value(json_object_get(master_root, "request_type"));
                if(request_type_str){
                    std::string request_type(request_type_str);
                    if(request_type == std::string("item")){
                        const char * type_str = json_string_value(json_object_get(master_root, "type"));
                        const char * comment_str = json_string_value(json_object_get(master_root, "comment"));
                        if(type_str && comment_str){
                            std::string type(type_str);
                            std::string comment(comment_str);

                            DatabaseManager::ErrorID eid;
                            int id = db->get_next_item_id(type, &eid);
                            if(id < 0){
                                free(master_root);
                                return db->error(eid);
                            }

                            if(!db->create_item(id, type, comment, &eid)){
                                free(master_root);
                                return db->error(eid);
                            }

                            json_t * obj_orig = db->get_item_json(id, type, &eid);
                            if(!obj_orig){
                                free(master_root);
                                return db->error(eid);
                            }

                            json_t * msg_root = json_object();
                            json_t * obj = json_copy(obj_orig);
                            json_object_set_new(msg_root, "response", json_string("ok"));
                            json_object_set_new(msg_root, "item", obj);
                            char * tr_str = json_dumps(msg_root, JSON_COMPACT);
                            std::string tr(tr_str);
                            free(tr_str);
                            free(msg_root);
                            free(master_root);
                            return tr;
                        }else return db->error(DatabaseManager::WRONG_ENTRY);
                    }else if(request_type == std::string("type")){
                        ///receives: id, name, comment
                        ///returns: full entry
                        const char * id_str = json_string_value(json_object_get(master_root, "id"));
                        const char * name_str = json_string_value(json_object_get(master_root, "name"));
                        const char * comment_str = json_string_value(json_object_get(master_root, "comment"));
                        if(id_str && name_str && comment_str){
                            std::string id(id_str);
                            std::string name(name_str);
                            std::string comment(comment_str);

                            DatabaseManager::ErrorID eid;

                            if(!db->create_type(id, name, comment, &eid)){
                                free(master_root);
                                return db->error(eid);
                            }

                            json_t * obj_orig = db->get_type_json(id, &eid);
                            if(!obj_orig){
                                free(master_root);
                                return db->error(eid);
                            }

                            json_t * msg_root = json_object();
                            json_t * obj = json_copy(obj_orig);
                            json_object_set_new(msg_root, "response", json_string("ok"));
                            json_object_set_new(msg_root, "type", obj);
                            char * tr_str = json_dumps(msg_root, JSON_COMPACT);
                            std::string tr(tr_str);
                            free(tr_str);
                            free(msg_root);
                            free(master_root);
                            return tr;
                        }else return db->error(DatabaseManager::WRONG_ENTRY);
                    }else if(request_type == std::string("wz")){
                        ///receives: date, person, comment, items
                        ///returns: full entry without items
                        const char * date_str = json_string_value(json_object_get(master_root, "date"));
                        const char * person_str = json_string_value(json_object_get(master_root, "person"));
                        const char * comment_str = json_string_value(json_object_get(master_root, "comment"));
                        std::vector<int> item_ids;
                        std::vector<std::string> item_types;
                        if(date_str && person_str && comment_str){
                            std::string date(date_str);
                            std::string person(person_str);
                            std::string comment(comment_str);

                            json_t * array = json_object_get(master_root, "items");
                            size_t x = 0;
                            while(x != json_array_size(array)){
                                json_t * obj = json_array_get(array, x);
                                json_t * obj_id = json_object_get(obj, "id");
                                json_t * obj_type = json_object_get(obj, "type");
                                if(!json_is_integer(obj_id) || !json_is_string(obj_type)){
                                    free(master_root);
                                    return db->error(DatabaseManager::WRONG_ENTRY);
                                }
                                int id = json_integer_value(obj_id);
                                const char * type_str = json_string_value(obj_type);
                                std::string type(type_str);
                                item_ids.push_back(id);
                                item_types.push_back(type);
                                ++x;
                            }

                            DatabaseManager::ErrorID eid;

                            int id = db->get_next_wz_id(&eid);
                            if(id < 0){
                                free(master_root);
                                return db->error(eid);
                            }

                            if(!db->create_wz(id, date, person, comment, item_ids, item_types, &eid)){
                                free(master_root);
                                return db->error(eid);
                            }

                            json_t * obj_orig = db->get_wz_json(id, &eid);
                            if(!obj_orig){
                                free(master_root);
                                return db->error(eid);
                            }

                            json_t * msg_root = json_object();
                            json_t * obj = json_copy(obj_orig);
                            json_object_set_new(msg_root, "response", json_string("ok"));
                            json_object_set_new(msg_root, "wz", obj);
                            char * tr_str = json_dumps(msg_root, JSON_COMPACT);
                            std::string tr(tr_str);
                            free(tr_str);
                            free(msg_root);
                            free(master_root);
                            return tr;
                        }else return db->error(DatabaseManager::WRONG_ENTRY);
                    }else if(request_type == std::string("pz")){
                        ///receives: wz_id, date, person, comment
                        ///returns: full entry
                        int wz_id = json_integer_value(json_object_get(master_root, "wz_id"));
                        const char * date_str = json_string_value(json_object_get(master_root, "date"));
                        const char * person_str = json_string_value(json_object_get(master_root, "person"));
                        const char * comment_str = json_string_value(json_object_get(master_root, "comment"));
                        if(date_str && person_str && comment_str){
                            std::string date(date_str);
                            std::string person(person_str);
                            std::string comment(comment_str);

                            DatabaseManager::ErrorID eid;

                            int id = db->get_next_pz_id(&eid);
                            if(id < 0){
                                free(master_root);
                                return db->error(eid);
                            }

                            if(!db->create_pz(id, wz_id, date, person, comment, &eid)){
                                free(master_root);
                                return db->error(eid);
                            }

                            json_t * obj_orig = db->get_pz_json(id, &eid);
                            if(!obj_orig){
                                free(master_root);
                                return db->error(eid);
                            }

                            json_t * msg_root = json_object();
                            json_t * obj = json_copy(obj_orig);
                            json_object_set_new(msg_root, "response", json_string("ok"));
                            json_object_set_new(msg_root, "pz", obj);
                            char * tr_str = json_dumps(msg_root, JSON_COMPACT);
                            std::string tr(tr_str);
                            free(tr_str);
                            free(msg_root);
                            free(master_root);
                            return tr;
                        }else return db->error(DatabaseManager::WRONG_ENTRY);
                    }
                    free(master_root);
                    return std::string("{\"response\":\"wrong_query\",\"longtext\":\"Wrong 'request_type' in `register` request.\"}");
                }
                free(master_root);
                return std::string("{\"response\":\"wrong_json\",\"longtext\":\"'request_type' in `register` request is not valid string.\"}");
            }else if(request == std::string("drop")){
                const char * request_type_str = json_string_value(json_object_get(master_root, "request_type"));
                if(request_type_str){
                    std::string request_type(request_type_str);
                    if(request_type == std::string("item")){
                        ///receives: id, type
                        ///returns: response
                        int id = json_integer_value(json_object_get(master_root, "id"));
                        const char * type_str = json_string_value(json_object_get(master_root, "type"));
                        if(type_str){
                            DatabaseManager::ErrorID eid;
                            std::string type(type_str);

                            if(!db->drop_item(id, type, &eid)){
                                free(master_root);
                                return db->error(eid);
                            }

                            json_t * msg_root = json_object();
                            json_object_set_new(msg_root, "response", json_string("ok"));
                            char * tr_str = json_dumps(msg_root, JSON_COMPACT);
                            std::string tr(tr_str);
                            free(tr_str);
                            free(msg_root);
                            free(master_root);
                            return tr;
                        }else return db->error(DatabaseManager::WRONG_ENTRY);
                    }else if(request_type == std::string("type")){
                        ///receives: id
                        ///returns: response
                        const char * id_str = json_string_value(json_object_get(master_root, "id"));
                        if(id_str){
                            DatabaseManager::ErrorID eid;
                            std::string id(id_str);

                            if(!db->drop_type(id, &eid)){
                                free(master_root);
                                return db->error(eid);
                            }

                            json_t * msg_root = json_object();
                            json_object_set_new(msg_root, "response", json_string("ok"));
                            char * tr_str = json_dumps(msg_root, JSON_COMPACT);
                            std::string tr(tr_str);
                            free(tr_str);
                            free(msg_root);
                            free(master_root);
                            return tr;
                        }else return db->error(DatabaseManager::WRONG_ENTRY);
                    }else if(request_type == std::string("wz")){
                        ///receives: id
                        ///returns: response
                        int id = json_integer_value(json_object_get(master_root, "id"));
                        DatabaseManager::ErrorID eid;

                        if(!db->drop_wz(id, &eid)){
                            free(master_root);
                            return db->error(eid);
                        }

                        json_t * msg_root = json_object();
                        json_object_set_new(msg_root, "response", json_string("ok"));
                        char * tr_str = json_dumps(msg_root, JSON_COMPACT);
                        std::string tr(tr_str);
                        free(tr_str);
                        free(msg_root);
                        free(master_root);
                        return tr;
                    }else if(request_type == std::string("pz")){
                        ///receives: id
                        ///returns: response
                        int id = json_integer_value(json_object_get(master_root, "id"));
                        DatabaseManager::ErrorID eid;

                        if(!db->drop_pz(id, &eid)){
                            free(master_root);
                            return db->error(eid);
                        }

                        json_t * msg_root = json_object();
                        json_object_set_new(msg_root, "response", json_string("ok"));
                        char * tr_str = json_dumps(msg_root, JSON_COMPACT);
                        std::string tr(tr_str);
                        free(tr_str);
                        free(msg_root);
                        free(master_root);
                        return tr;
                    }
                    free(master_root);
                    return std::string("{\"response\":\"wrong_query\",\"longtext\":\"Wrong 'request_type' in `drop` request.\"}");
                }
                free(master_root);
                return std::string("{\"response\":\"wrong_json\",\"longtext\":\"'request_type' in `drop` request is not valid string.\"}");
            }else if(request == std::string("change")){
                const char * request_type_str = json_string_value(json_object_get(master_root, "request_type"));
                if(request_type_str){
                    std::string request_type(request_type_str);
                    if(request_type == std::string("item")){
                        ///receives: id, type, new_comment
                        ///returns: response
                        int id = json_integer_value(json_object_get(master_root, "id"));
                        const char * type_str = json_string_value(json_object_get(master_root, "type"));
                        if(type_str){
                            DatabaseManager::ErrorID eid;
                            std::string type(type_str);
                            json_t * new_comment_ptr = json_object_get(master_root, "new_comment");
                            std::string new_comment;
                            if(json_is_string(new_comment_ptr)){
                                new_comment = json_string_value(new_comment_ptr);
                            }

                            if(!db->update_item(id, type, false, 0, json_is_string(new_comment_ptr), new_comment, &eid)){
                                free(master_root);
                                return db->error(eid);
                            }

                            json_t * msg_root = json_object();
                            json_object_set_new(msg_root, "response", json_string("ok"));
                            char * tr_str = json_dumps(msg_root, JSON_COMPACT);
                            std::string tr(tr_str);
                            free(tr_str);
                            free(msg_root);
                            free(master_root);
                            return tr;
                        }else return db->error(DatabaseManager::WRONG_ENTRY);
                    }else if(request_type == std::string("type")){
                        ///receives: id, new_name, new_comment
                        ///returns: full entry
                        const char * id_str = json_string_value(json_object_get(master_root, "id"));
                        if(id_str){
                            std::string id(id_str);
                            DatabaseManager::ErrorID eid;
                            json_t * new_name_ptr = json_object_get(master_root, "new_name");
                            json_t * new_comment_ptr = json_object_get(master_root, "new_comment");
                            std::string new_name;
                            std::string new_comment;
                            if(json_is_string(new_name_ptr)){
                                new_name = json_string_value(new_name_ptr);
                            }
                            if(json_is_string(new_comment_ptr)){
                                new_comment = json_string_value(new_comment_ptr);
                            }

                            if(!db->update_type(id, json_is_string(new_name_ptr), new_name, json_is_string(new_comment_ptr), new_comment, &eid)){
                                free(master_root);
                                return db->error(eid);
                            }

                            json_t * msg_root = json_object();
                            json_object_set_new(msg_root, "response", json_string("ok"));
                            char * tr_str = json_dumps(msg_root, JSON_COMPACT);
                            std::string tr(tr_str);
                            free(tr_str);
                            free(msg_root);
                            free(master_root);
                            return tr;
                        }else return db->error(DatabaseManager::WRONG_ENTRY);
                    }else if(request_type == std::string("wz")){
                        ///receives: id, new_date, new_person, new_comment
                        ///returns: full entry without items
                        int id = json_integer_value(json_object_get(master_root, "id"));
                        DatabaseManager::ErrorID eid;
                        json_t * new_date_ptr = json_object_get(master_root, "new_date");
                        json_t * new_person_ptr = json_object_get(master_root, "new_person");
                        json_t * new_comment_ptr = json_object_get(master_root, "new_comment");
                        std::string new_date;
                        std::string new_person;
                        std::string new_comment;
                        if(json_is_string(new_date_ptr)){
                            new_date = json_string_value(new_date_ptr);
                        }
                        if(json_is_string(new_person_ptr)){
                            new_person = json_string_value(new_person_ptr);
                        }
                        if(json_is_string(new_comment_ptr)){
                            new_comment = json_string_value(new_comment_ptr);
                        }

                        if(!db->update_wz(id, json_is_string(new_date_ptr), new_date, json_is_string(new_person_ptr), new_person, json_is_string(new_comment_ptr), new_comment, &eid)){
                            free(master_root);
                            return db->error(eid);
                        }

                        json_t * msg_root = json_object();
                        json_object_set_new(msg_root, "response", json_string("ok"));
                        char * tr_str = json_dumps(msg_root, JSON_COMPACT);
                        std::string tr(tr_str);
                        free(tr_str);
                        free(msg_root);
                        free(master_root);
                        return tr;
                    }else if(request_type == std::string("pz")){
                        ///receives: id, new_date, new_person, new_comment
                        ///returns: full entry
                        int id = json_integer_value(json_object_get(master_root, "id"));
                        DatabaseManager::ErrorID eid;
                        json_t * new_date_ptr = json_object_get(master_root, "new_date");
                        json_t * new_person_ptr = json_object_get(master_root, "new_person");
                        json_t * new_comment_ptr = json_object_get(master_root, "new_comment");
                        std::string new_date;
                        std::string new_person;
                        std::string new_comment;
                        if(json_is_string(new_date_ptr)){
                            new_date = json_string_value(new_date_ptr);
                        }
                        if(json_is_string(new_person_ptr)){
                            new_person = json_string_value(new_person_ptr);
                        }
                        if(json_is_string(new_comment_ptr)){
                            new_comment = json_string_value(new_comment_ptr);
                        }

                        if(!db->update_pz(id, json_is_string(new_date_ptr), new_date, json_is_string(new_person_ptr), new_person, json_is_string(new_comment_ptr), new_comment, &eid)){
                            free(master_root);
                            return db->error(eid);
                        }

                        json_t * msg_root = json_object();
                        json_object_set_new(msg_root, "response", json_string("ok"));
                        char * tr_str = json_dumps(msg_root, JSON_COMPACT);
                        std::string tr(tr_str);
                        free(tr_str);
                        free(msg_root);
                        free(master_root);
                        return tr;
                    }
                    free(master_root);
                    return std::string("{\"response\":\"wrong_query\",\"longtext\":\"Wrong 'request_type' in `register` request.\"}");
                }
                free(master_root);
                return std::string("{\"response\":\"wrong_json\",\"longtext\":\"'request_type' in `register` request is not valid string.\"}");
            }else if(request == std::string("other")){
                ///Place for other requests
                const char * request_type_str = json_string_value(json_object_get(master_root, "request_type"));
                if(request_type_str){
                    std::string request_type(request_type_str);
                    if(request_type == std::string("get_error")){
                        ///Sample request - returns error.
                        return db->error(DatabaseManager::WRONG_ENTRY);
                    }
                    free(master_root);
                    return std::string("{\"response\":\"wrong_query\",\"longtext\":\"Wrong 'request_type' in `other` request.\"}");
                }
                free(master_root);
                return std::string("{\"response\":\"wrong_json\",\"longtext\":\"'request_type' in `other` request is not valid string.\"}");
            }
            free(master_root);
            return std::string("{\"response\":\"wrong_query\",\"longtext\":\"Wrong 'request'.\"}");
        }
        free(master_root);
        return std::string("{\"response\":\"wrong_json\",\"longtext\":\"'request' is not valid string.\"}");
    }
    free(master_root);
    return std::string("{\"response\":\"unknown_error\",\"longtext\":\"Parser reached end of funtion.\"}");
}

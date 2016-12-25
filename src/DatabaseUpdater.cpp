#include "DatabaseManager.h"

bool Wicher::DB::DatabaseManager::update_item(int id, std::string type, /*bool update_id, int new_id, */bool update_wz, int wz_id, bool update_comment, std::string comment, ErrorID * errorid){
    if(!item_exists(id, type)){
        *errorid = ID_INVALID;
        return false;
    }
    json_t * obj_ptr = get_item_json(id, type, errorid);

    if(json_is_object(obj_ptr)){
        /*if(update_id && new_id >= 0){
            if(item_exists(new_id, type)){
                *errorid = NEWID_INVALID;
                return false;
            }
            json_object_set_new(obj_ptr, "id", json_integer(new_id));
        }*/
        if(update_wz){
            if(wz_id >= 0){
                if(!wz_exists(wz_id)){
                    *errorid = WZ_INVALID;
                    return false;
                }
                json_object_set_new(obj_ptr, "wz_id", json_integer(wz_id));
            }else json_object_del(obj_ptr, "wz_id");
        }
        if(update_comment){
            json_object_set_new(obj_ptr, "comment", json_string(comment.c_str()));
        }
        *errorid = NONE;
        return true;
    }
    *errorid = WRONG_ENTRY;
    return false;
}

bool Wicher::DB::DatabaseManager::update_type(std::string id, bool update_name, std::string new_name, bool update_comment, std::string new_comment, ErrorID *errorid){
    if(!type_exists(id)){
        *errorid = ID_INVALID;
        return false;
    }

    json_t * obj_ptr = get_type_json(id, errorid);
    if(json_is_object(obj_ptr)){
        if(update_name && !new_name.empty()){
            json_object_set_new(obj_ptr, "name", json_string(new_name.c_str()));
        }
        if(update_comment){
            json_object_set_new(obj_ptr, "comment", json_string(new_comment.c_str()));
        }
        *errorid = NONE;
        return true;
    }
    *errorid = WRONG_ENTRY;
    return false;
}

bool Wicher::DB::DatabaseManager::update_wz(int id, bool update_date, std::string new_date, bool update_person, std::string new_person, bool update_comment, std::string new_comment, ErrorID *errorid){
    if(!wz_exists(id)){
        *errorid = ID_INVALID;
        return false;
    }
    json_t * wz_ptr = get_wz_json(id, errorid);
    if(json_is_object(wz_ptr)){
        if(update_date && !new_date.empty()){
            json_object_set_new(wz_ptr, "date", json_string(new_date.c_str()));
        }
        if(update_person && !new_person.empty()){
            json_object_set_new(wz_ptr, "person", json_string(new_person.c_str()));
        }
        if(update_comment && !new_comment.empty()){
            json_object_set_new(wz_ptr, "comment", json_string(new_comment.c_str()));
        }
        *errorid = NONE;
        return true;
    }
    *errorid = WRONG_ENTRY;
    return false;
}

bool Wicher::DB::DatabaseManager::update_pz(int id, bool update_date, std::string new_date, bool update_person, std::string new_person, bool update_comment, std::string new_comment, ErrorID *errorid){
    if(!pz_exists(id)){
        *errorid = ID_INVALID;
        return false;
    }
    json_t * pz_ptr = get_pz_json(id, errorid);
    if(json_is_object(pz_ptr)){
        if(update_date && !new_date.empty()){
            json_object_set_new(pz_ptr, "date", json_string(new_date.c_str()));
        }
        if(update_person && !new_person.empty()){
            json_object_set_new(pz_ptr, "person", json_string(new_person.c_str()));
        }
        if(update_comment && !new_comment.empty()){
            json_object_set_new(pz_ptr, "comment", json_string(new_comment.c_str()));
        }
        *errorid = NONE;
        return true;
    }
    *errorid = WRONG_ENTRY;
    return false;
}

bool Wicher::DB::DatabaseManager::update_history(int id, bool update_data, std::string data, ErrorID * errorid){
    if(!history_exists(id)){
        *errorid = ID_INVALID;
        return false;
    }
    json_t * obj_ptr = get_history_json(id, errorid);

    if(json_is_object(obj_ptr)){
        if(update_data){
            json_object_set_new(obj_ptr, "data", json_string(data.c_str()));
        }
        *errorid = NONE;
        return true;
    }
    return false;
}

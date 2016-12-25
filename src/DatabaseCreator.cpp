#include "DatabaseManager.h"
#include <algorithm>
#include <cstring>

bool Wicher::DB::DatabaseManager::create_item(int id, std::string type, std::string comment, ErrorID * errorid){
    if(item_exists(id, type)){
        *errorid = ID_INVALID;
        return false;
    }
    if(!type_exists(type)){
        *errorid = TYPE_INVALID;
        return false;
    }
    json_t * obj = json_object();
    json_object_set_new(obj, "id", json_integer(id));
    json_object_set_new(obj, "type", json_string(type.c_str()));
    json_object_set_new(obj, "comment", json_string(comment.c_str()));

    json_array_append_new(items, obj);
    *errorid = NONE;
    return true;
}
bool Wicher::DB::DatabaseManager::create_type(std::string id, std::string name, std::string comment, ErrorID * errorid){
    if(type_exists(id)){
        *errorid = ID_INVALID;
        return false;
    }
    json_t * obj = json_object();
    json_object_set_new(obj, "id", json_string(id.c_str()));
    json_object_set_new(obj, "name", json_string(name.c_str()));
    json_object_set_new(obj, "comment", json_string(comment.c_str()));

    json_array_append_new(types, obj);
    *errorid = NONE;
    return true;
}
bool Wicher::DB::DatabaseManager::create_wz(int id, std::string date, std::string person, std::string comment, std::vector<int> item_ids, std::vector<std::string> item_types, ErrorID * errorid){
    if(wz_exists(id)){
        *errorid = ID_INVALID;
        return false;
    }
    json_t * obj = json_object();
    json_object_set_new(obj, "id", json_integer(id));
    json_object_set_new(obj, "date", json_string(date.c_str()));
    json_object_set_new(obj, "person", json_string(person.c_str()));
    json_object_set_new(obj, "comment", json_string(comment.c_str()));

    json_t * items_array = json_array();
    unsigned int y = 0;
    while(y != item_ids.size()){
        json_t * item = get_item_json(item_ids[y], item_types[y], errorid);
        if(item == nullptr){
            return false;
        }
        json_object_set_new(item, "wz_id", json_integer(id));
        json_t * object = json_object();
        json_object_set_new(object, "id", json_integer(item_ids[y]));
        json_object_set_new(object, "type", json_string(item_types[y].c_str()));
        json_array_append_new(items_array, object);
        ++y;
    }
    json_object_set_new(obj, "items", items_array);
    
    json_t * types_array = json_array();
    std::sort(item_types.begin(), item_types.end());
    item_types.erase(std::unique(item_types.begin(), item_types.end()), item_types.end());
    y = 0;
    while(y < item_types.size()){
        for(unsigned int z = 0; z < json_array_size(types); ++z){
            json_t * item_obj = json_array_get(types, z);
            const char * item_id = json_string_value(json_object_get(item_obj, "id"));
            if(item_id && strcmp(item_id, item_types[y].c_str()) == 0){
                json_array_append_new(types_array, json_copy(item_obj));
                break;
            }
        }
        ++y;
    }
    json_object_set_new(obj, "types", types_array);

    json_array_append_new(wzs, obj);
    *errorid = NONE;
    return true;
}
bool Wicher::DB::DatabaseManager::create_pz(int id, int wz_id, std::string date, std::string person, std::string comment, ErrorID * errorid){
    if(pz_exists(id)){
        *errorid = ID_INVALID;
        return false;
    }
    if(!wz_exists(wz_id) || !wz_free(wz_id)){
        *errorid = WZ_INVALID;
        return false;
    }

    size_t y = 0;
    while(y != json_array_size(items)){
        json_t * item = json_array_get(items, y);
        json_t * wz_id_ptr = json_object_get(item, "wz_id");
        if(json_is_integer(wz_id_ptr) && json_integer_value(wz_id_ptr) == wz_id){
            json_object_del(item, "wz_id");
        }
        ++y;
    }

    json_t * obj = json_object();
    json_object_set_new(obj, "id", json_integer(id));
    json_object_set_new(obj, "wz_id", json_integer(wz_id));
    json_object_set_new(obj, "date", json_string(date.c_str()));
    json_object_set_new(obj, "person", json_string(person.c_str()));
    json_object_set_new(obj, "comment", json_string(comment.c_str()));

    json_array_append_new(pzs, obj);
    *errorid = NONE;
    return true;
}
bool Wicher::DB::DatabaseManager::create_history(int id, std::string data, ErrorID * errorid){
    if(history_exists(id)){
        *errorid = ID_INVALID;
        return false;
    }
    json_t * obj = json_object();
    json_object_set_new(obj, "id", json_integer(id));
    json_object_set_new(obj, "data", json_string(data.c_str()));

    json_array_append_new(history, obj);
    *errorid = NONE;
    return true;
}

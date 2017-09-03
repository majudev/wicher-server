#include "JSONDatabase.h"

int JSONDatabase::get_next_item_id(std::string type, ErrorID *errorid){
    if(!items){
        *errorid = ARRAY_IS_NULL;
        return -1;
    }
    int id = 0;
    size_t x = 0;
    while(x != json_array_size(items)){
        json_t * obj_ptr = json_array_get(items, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        json_t * type_ptr = json_object_get(obj_ptr, "type");
        if(!json_is_integer(id_ptr) || !json_is_string(type_ptr)){
            *errorid = WRONG_ENTRY;
            return -1;
        }
        if(std::string(json_string_value(type_ptr)) == type){
            int val = json_integer_value(id_ptr);
            if(val > id){
                id = val;
            }
        }
        ++x;
    }
    return id + 1;
}
int JSONDatabase::get_next_wz_id(ErrorID *errorid){
    if(!wzs){
        *errorid = ARRAY_IS_NULL;
        return -1;
    }
    int id = 0;
    size_t x = 0;
    while(x != json_array_size(wzs)){
        json_t * obj_ptr = json_array_get(wzs, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        if(!json_is_integer(id_ptr)){
            *errorid = WRONG_ENTRY;
            return -1;
        }
        int val = json_integer_value(id_ptr);
        if(val > id){
            id = val;
        }
        ++x;
    }
    return id + 1;
}
int JSONDatabase::get_next_pz_id(ErrorID *errorid){
    if(!pzs){
        *errorid = ARRAY_IS_NULL;
        return -1;
    }
    int id = 0;
    size_t x = 0;
    while(x != json_array_size(pzs)){
        json_t * obj_ptr = json_array_get(pzs, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        if(!json_is_integer(id_ptr)){
            *errorid = WRONG_ENTRY;
            return -1;
        }
        int val = json_integer_value(id_ptr);
        if(val > id){
            id = val;
        }
        ++x;
    }
    return id + 1;
}
int JSONDatabase::get_next_history_id(ErrorID *errorid){
    if(!history){
        *errorid = ARRAY_IS_NULL;
        return -1;
    }
    int id = 0;
    size_t x = 0;
    while(x != json_array_size(history)){
        json_t * obj_ptr = json_array_get(history, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        if(!json_is_integer(id_ptr)){
            *errorid = WRONG_ENTRY;
            return -1;
        }
        int val = json_integer_value(id_ptr);
        if(val > id){
            id = val;
        }
        ++x;
    }
    return id + 1;
}

bool JSONDatabase::item_exists(int id, std::string type){
    ErrorID eid;
    json_t * json = get_item_js(id, type, &eid);
    return json != nullptr;
}

bool JSONDatabase::type_exists(std::string id){
    ErrorID eid;
    json_t * json = get_type_js(id, &eid);
    return json != nullptr;
}

bool JSONDatabase::wz_exists(int id){
    ErrorID eid;
    json_t * json = get_wz_js(id, &eid);
    return json != nullptr;
}

bool JSONDatabase::wz_free(int wz_id){
    if(!pzs){
        return false;
    }
    size_t x = 0;
    while(x != json_array_size(pzs)){
        json_t * obj_ptr = json_array_get(pzs, x);
        json_t * id_ptr = json_object_get(obj_ptr, "wz_id");
        if(!json_is_integer(id_ptr)){
            return false;
        }
        if(json_integer_value(id_ptr) == wz_id){
            return false;
        }
        ++x;
    }
    return true;
}

bool JSONDatabase::pz_exists(int id){
    ErrorID eid;
    json_t * json = get_pz_js(id, &eid);
    return json != nullptr;
}

bool JSONDatabase::history_exists(int id){
    ErrorID eid;
    json_t * json = get_history_js(id, &eid);
    return json != nullptr;
}

json_t * JSONDatabase::get_item_js(int id, std::string type, ErrorID * errorid){
    if(!items){
        *errorid = ARRAY_IS_NULL;
        return nullptr;
    }
    size_t x = 0;
    while(x != json_array_size(items)){
        json_t * obj_ptr = json_array_get(items, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        json_t * type_ptr = json_object_get(obj_ptr, "type");
        if(!json_is_integer(id_ptr) || !json_is_string(type_ptr)){
            *errorid = WRONG_ENTRY;
            return nullptr;
        }
        if(json_integer_value(id_ptr) == id && std::string(json_string_value(type_ptr)) == type){
            *errorid = NONE;
            return obj_ptr;
        }
        ++x;
    }
    *errorid = NONE;
    return nullptr;
}

json_t * JSONDatabase::get_type_js(std::string id, ErrorID * errorid){
    if(!types){
        *errorid = ARRAY_IS_NULL;
        return nullptr;
    }
    size_t x = 0;
    while(x != json_array_size(types)){
        json_t * obj_ptr = json_array_get(types, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        if(!json_is_string(id_ptr)){
            *errorid = WRONG_ENTRY;
            return nullptr;
        }
        if(std::string(json_string_value(id_ptr)) == id){
            *errorid = NONE;
            return obj_ptr;
        }
        ++x;
    }
    *errorid = NONE;
    return nullptr;
}

json_t * JSONDatabase::get_wz_js(int id, ErrorID * errorid){
    if(!wzs){
        *errorid = ARRAY_IS_NULL;
        return nullptr;
    }
    size_t x = 0;
    while(x != json_array_size(wzs)){
        json_t * obj_ptr = json_array_get(wzs, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        if(!json_is_integer(id_ptr)){
            *errorid = WRONG_ENTRY;
            return nullptr;
        }
        if(json_integer_value(id_ptr) == id){
            *errorid = NONE;
            return obj_ptr;
        }
        ++x;
    }
    *errorid = NONE;
    return nullptr;
}

json_t * JSONDatabase::get_pz_js(int id, ErrorID * errorid){
    if(!pzs){
        *errorid = ARRAY_IS_NULL;
        return nullptr;
    }
    size_t x = 0;
    while(x != json_array_size(pzs)){
        json_t * obj_ptr = json_array_get(pzs, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        if(!json_is_integer(id_ptr)){
            *errorid = WRONG_ENTRY;
            return nullptr;
        }
        if(json_integer_value(id_ptr) == id){
            *errorid = NONE;
            return obj_ptr;
        }
        ++x;
    }
    *errorid = NONE;
    return nullptr;
}

json_t * JSONDatabase::get_history_js(int id, ErrorID * errorid){
    if(!history){
        *errorid = ARRAY_IS_NULL;
        return nullptr;
    }
    size_t x = 0;
    while(x != json_array_size(history)){
        json_t * obj_ptr = json_array_get(history, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        if(!json_is_integer(id_ptr)){
            *errorid = WRONG_ENTRY;
            return nullptr;
        }
        if(json_integer_value(id_ptr) == id){
            *errorid = NONE;
            return obj_ptr;
        }
        ++x;
    }
    *errorid = NONE;
    return nullptr;
}

std::string JSONDatabase::get_item_json(int id, std::string type, ErrorID * errorid){
    json_t * root = get_item_js(id, type, errorid);
    if(!root) return std::string("{}");
    char * response = json_dumps(root, JSON_COMPACT);
    std::string tr(response);
    free(response);
    *errorid = NONE;
    return tr;
}

std::string JSONDatabase::get_type_json(std::string id, ErrorID * errorid){
    json_t * root = get_type_js(id, errorid);
    if(!root) return std::string("{}");
    char * response = json_dumps(root, JSON_COMPACT);
    std::string tr(response);
    free(response);
    *errorid = NONE;
    return tr;
}

std::string JSONDatabase::get_wz_json(int id, ErrorID * errorid){
    json_t * root = get_wz_js(id, errorid);
    if(!root) return std::string("{}");
    char * response = json_dumps(root, JSON_COMPACT);
    std::string tr(response);
    free(response);
    *errorid = NONE;
    return tr;
}

std::string JSONDatabase::get_pz_json(int id, ErrorID * errorid){
    json_t * root = get_pz_js(id, errorid);
    if(!root) return std::string("{}");
    char * response = json_dumps(root, JSON_COMPACT);
    std::string tr(response);
    free(response);
    *errorid = NONE;
    return tr;
}

std::string JSONDatabase::get_history_json(int id, ErrorID * errorid){
    json_t * root = get_history_js(id, errorid);
    if(!root) return std::string("{}");
    char * response = json_dumps(root, JSON_COMPACT);
    std::string tr(response);
    free(response);
    *errorid = NONE;
    return tr;
}
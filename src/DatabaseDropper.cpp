#include "DatabaseManager.h"

bool Wicher::DB::DatabaseManager::drop_item(int id, std::string type, ErrorID * errorid){
    if(!items){
        *errorid = ARRAY_IS_NULL;
        return false;
    }
    size_t x = 0;
    while(x != json_array_size(items)){
        json_t * obj_ptr = json_array_get(items, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        json_t * type_ptr = json_object_get(obj_ptr, "type");
        if(!json_is_integer(id_ptr) || !json_is_string(type_ptr)){
            *errorid = WRONG_ENTRY;
            return false;
        }
        if(json_integer_value(id_ptr) == id && std::string(json_string_value(type_ptr)) == type){
            json_array_remove(items, x);
            *errorid = NONE;
            return true;
        }
        ++x;
    }
    *errorid = ID_INVALID;
    return false;
}

bool Wicher::DB::DatabaseManager::drop_type(std::string id, ErrorID * errorid){
    if(!types || !items){
        *errorid = ARRAY_IS_NULL;
        return false;
    }
    size_t x = 0;
    while(x != json_array_size(items)){
        json_t * obj_ptr = json_array_get(items, x);
        json_t * type_ptr = json_object_get(obj_ptr, "type");
        if(!json_is_string(type_ptr)){
            *errorid = WRONG_ENTRY;
            return false;
        }
        if(std::string(json_string_value(type_ptr)) == id){
            json_array_remove(items, x);
        }else ++x;
    }
    x = 0;
    while(x != json_array_size(types)){
        json_t * obj_ptr = json_array_get(types, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        if(!json_is_string(id_ptr)){
            *errorid = WRONG_ENTRY;
            return false;
        }
        if(std::string(json_string_value(id_ptr)) == id){
            json_array_remove(types, x);
            *errorid = NONE;
            return true;
        }
        ++x;
    }
    *errorid = ID_INVALID;
    return false;
}

bool Wicher::DB::DatabaseManager::drop_wz(int id, ErrorID * errorid){
    if(!wzs || !items){
        *errorid = ARRAY_IS_NULL;
        return false;
    }
    size_t x = 0;
    while(x != json_array_size(items)){
        json_t * obj_ptr = json_array_get(items, x);
        json_t * wz_id_ptr = json_object_get(obj_ptr, "wz_id");
        if(json_is_integer(wz_id_ptr) && json_integer_value(wz_id_ptr) == id){
            json_object_del(obj_ptr, "wz_id");
        }
        ++x;
    }
    x = 0;
    while(x != json_array_size(wzs)){
        json_t * obj_ptr = json_array_get(wzs, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        if(!json_is_integer(id_ptr)){
            *errorid = WRONG_ENTRY;
            return false;
        }
        if(json_integer_value(id_ptr) == id){
            json_array_remove(wzs, x);
            *errorid = NONE;
            return true;
        }
        ++x;
    }
    *errorid = ID_INVALID;
    return false;
}

bool Wicher::DB::DatabaseManager::drop_pz(int id, ErrorID * errorid){
    if(!pzs){
        *errorid = ARRAY_IS_NULL;
        return false;
    }
    size_t x = 0;
    while(x != json_array_size(pzs)){
        json_t * obj_ptr = json_array_get(pzs, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        json_t * wz_id_ptr = json_object_get(obj_ptr, "wz_id");
        if(!json_is_integer(id_ptr) || !json_is_integer(wz_id_ptr)){
            *errorid = WRONG_ENTRY;
            return false;
        }
        if(json_integer_value(id_ptr) == id){
            if(!this->drop_wz(json_integer_value(wz_id_ptr), errorid)){
                std::cout << "fallus" << json_integer_value(id_ptr) << " " << json_integer_value(wz_id_ptr) << std::endl;
                return false;
            }
            json_array_remove(pzs, x);
            std::cout << "cycus-machalicus" << std::endl;
            *errorid = NONE;
            return true;
        }
        ++x;
    }
    *errorid = ID_INVALID;
    return false;
}

bool Wicher::DB::DatabaseManager::drop_history(int id, ErrorID * errorid){
    if(!history){
        *errorid = ARRAY_IS_NULL;
        return false;
    }
    size_t x = 0;
    while(x != json_array_size(history)){
        json_t * obj_ptr = json_array_get(history, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        if(!json_is_integer(id_ptr)){
            *errorid = WRONG_ENTRY;
            return false;
        }
        if(json_integer_value(id_ptr) == id){
            json_array_remove(history, x);
            *errorid = NONE;
            return true;
        }
        ++x;
    }
    *errorid = ID_INVALID;
    return false;
}

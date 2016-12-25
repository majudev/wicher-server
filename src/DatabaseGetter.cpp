#include "DatabaseManager.h"

std::string Wicher::DB::DatabaseManager::error(ErrorID errorid){
    json_t * msg_root = json_object();
    json_object_set_new(msg_root, "response", json_string("error"));
    json_object_set_new(msg_root, "errorid", json_integer(errorid));
    char * response = json_dumps(msg_root, JSON_COMPACT);
    std::string tr(response);
    free(response);
    free(msg_root);
    return tr;
}

std::string Wicher::DB::DatabaseManager::get_items(std::string type){
    if(!items){
        return error(ARRAY_IS_NULL);
    }

    json_t * msg_root = json_object();
    json_object_set_new(msg_root, "response", json_string("ok"));
    json_t * array = json_array();

    size_t x = 0;
    while(x != json_array_size(items)){
        json_t * obj_ptr = json_array_get(items, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        json_t * type_ptr = json_object_get(obj_ptr, "type");
        json_t * wz_id_ptr = json_object_get(obj_ptr, "wz_id");
        json_t * comment_ptr = json_object_get(obj_ptr, "comment");

        if(!(json_is_integer(id_ptr) && json_is_string(type_ptr) && json_is_string(comment_ptr))){
            free(msg_root);
            return error(WRONG_ENTRY);
        }

        if(std::string(json_string_value(type_ptr)) == type){
            json_t * obj = json_object();
            json_object_set_new(obj, "id", json_copy(id_ptr));
            json_object_set_new(obj, "type", json_copy(type_ptr));
            if(json_is_integer(wz_id_ptr) && json_integer_value(wz_id_ptr) >= 0){
                json_object_set_new(obj, "available", json_boolean(false));
                json_object_set_new(obj, "wz_id", json_copy(wz_id_ptr));
            }else{
                json_object_set_new(obj, "available", json_boolean(true));
            }
            json_object_set_new(obj, "comment", json_copy(comment_ptr));
            json_array_append_new(array, obj);
        }

        ++x;
    }

    json_object_set_new(msg_root, "items", array);
    char * tr_str = json_dumps(msg_root, JSON_COMPACT);
    std::string tr(tr_str);
    free(tr_str);
    free(msg_root);
    return tr;
}
std::string Wicher::DB::DatabaseManager::get_types(){
    if(!types || !items){
        return error(ARRAY_IS_NULL);
    }

    json_t * msg_root = json_object();
    json_object_set_new(msg_root, "response", json_string("ok"));
    json_t * array = json_array();

    size_t x = 0;
    while(x != json_array_size(types)){
        json_t * obj_ptr = json_array_get(types, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        json_t * name_ptr = json_object_get(obj_ptr, "name");
        json_t * comment_ptr = json_object_get(obj_ptr, "comment");

        if(!(json_is_string(id_ptr) && json_is_string(name_ptr) && json_is_string(comment_ptr))){
            free(msg_root);
            return error(WRONG_ENTRY);
        }

        json_t * obj = json_object();
        json_object_set_new(obj, "id", json_copy(id_ptr));
        json_object_set_new(obj, "name", json_copy(name_ptr));
        json_object_set_new(obj, "comment", json_copy(comment_ptr));
        int total = 0;
        int unavailable = 0;
        size_t y = 0;
        while(y != json_array_size(items)){
            json_t * obj_ptr0 = json_array_get(items, y);
            json_t * type_ptr0 = json_object_get(obj_ptr0, "type");
            json_t * wz_id_ptr0 = json_object_get(obj_ptr0, "wz_id");
            if(json_equal(type_ptr0, id_ptr)){
                if(json_is_integer(wz_id_ptr0)) ++unavailable;
                ++total;
            }
            ++y;
        }
        json_object_set_new(obj, "total", json_integer(total));
        json_object_set_new(obj, "unavailable", json_integer(unavailable));
        json_array_append_new(array, obj);

        ++x;
    }

    json_object_set_new(msg_root, "types", array);
    char * tr_str = json_dumps(msg_root, JSON_COMPACT);
    std::string tr(tr_str);
    free(tr_str);
    free(msg_root);
    return tr;
}
std::string Wicher::DB::DatabaseManager::get_wzs(){
    if(!wzs || !pzs){
        return error(ARRAY_IS_NULL);
    }

    json_t * msg_root = json_object();
    json_object_set_new(msg_root, "response", json_string("ok"));
    json_t * array = json_array();

    size_t x = 0;
    while(x != json_array_size(wzs)){
        json_t * obj_ptr = json_array_get(wzs, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        json_t * date_ptr = json_object_get(obj_ptr, "date");
        json_t * person_ptr = json_object_get(obj_ptr, "person");
        json_t * comment_ptr = json_object_get(obj_ptr, "comment");

        if(!(json_is_integer(id_ptr) && json_is_string(date_ptr) && json_is_string(person_ptr) && json_is_string(comment_ptr))){
            free(msg_root);
            return error(WRONG_ENTRY);
        }

        json_t * obj = json_object();
        json_object_set_new(obj, "id", json_copy(id_ptr));
        json_object_set_new(obj, "date", json_copy(date_ptr));
        json_object_set_new(obj, "person", json_copy(person_ptr));
        json_object_set_new(obj, "comment", json_copy(comment_ptr));
        std::string wz_id_str("N/D");
        size_t y = 0;
        while(y != json_array_size(pzs)){
            json_t * obj_ptr = json_array_get(pzs, y);
            json_t * wz_id_ptr = json_object_get(obj_ptr, "wz_id");
            json_t * wz_date_ptr = json_object_get(obj_ptr, "date");
            if(!json_is_integer(wz_id_ptr) || !json_is_string(wz_date_ptr)){
                free(msg_root);
                return error(WRONG_ENTRY);
            }
            if(json_integer_value(wz_id_ptr) == json_integer_value(id_ptr)){
                wz_id_str = json_string_value(wz_date_ptr);
                break;
            }
            ++y;
        }
        json_object_set_new(obj, "return_date", json_string(wz_id_str.c_str()));

        json_array_append_new(array, obj);

        ++x;
    }

    json_object_set_new(msg_root, "wzs", array);
    char * tr_str = json_dumps(msg_root, JSON_COMPACT);
    std::string tr(tr_str);
    free(tr_str);
    free(msg_root);
    return tr;
}
std::string Wicher::DB::DatabaseManager::get_pzs(){
    if(!pzs){
        return error(ARRAY_IS_NULL);
    }

    json_t * msg_root = json_object();
    json_object_set_new(msg_root, "response", json_string("ok"));
    json_t * array = json_array();

    size_t x = 0;
    while(x != json_array_size(pzs)){
        json_t * obj_ptr = json_array_get(pzs, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        json_t * wz_id_ptr = json_object_get(obj_ptr, "wz_id");
        json_t * date_ptr = json_object_get(obj_ptr, "date");
        json_t * person_ptr = json_object_get(obj_ptr, "person");
        json_t * comment_ptr = json_object_get(obj_ptr, "comment");

        if(!(json_is_integer(id_ptr) && json_is_integer(wz_id_ptr) && json_is_string(date_ptr) && json_is_string(person_ptr) && json_is_string(comment_ptr))){
            free(msg_root);
            return error(WRONG_ENTRY);
        }

        json_t * obj = json_object();
        json_object_set_new(obj, "id", json_copy(id_ptr));
        json_object_set_new(obj, "wz_id", json_copy(wz_id_ptr));
        json_object_set_new(obj, "date", json_copy(date_ptr));
        json_object_set_new(obj, "person", json_copy(person_ptr));
        json_object_set_new(obj, "comment", json_copy(comment_ptr));

        json_array_append_new(array, obj);

        ++x;
    }

    json_object_set_new(msg_root, "pzs", array);
    char * tr_str = json_dumps(msg_root, JSON_COMPACT);
    std::string tr(tr_str);
    free(tr_str);
    free(msg_root);
    return tr;
}
std::string Wicher::DB::DatabaseManager::get_history(){
    if(!history){
        return error(ARRAY_IS_NULL);
    }

    json_t * msg_root = json_object();
    json_object_set_new(msg_root, "response", json_string("ok"));
    json_t * array = json_array();

    size_t x = 0;
    while(x != json_array_size(history)){
        json_t * obj_ptr = json_array_get(history, x);
        json_t * id_ptr = json_object_get(obj_ptr, "id");
        json_t * data_ptr = json_object_get(obj_ptr, "data");

        if(!(json_is_integer(id_ptr) && json_is_string(data_ptr))){
            free(msg_root);
            return error(WRONG_ENTRY);
        }

        json_t * obj = json_object();
        json_object_set_new(obj, "id", json_copy(id_ptr));
        json_object_set_new(obj, "data", json_copy(data_ptr));

        json_array_append_new(array, obj);

        ++x;
    }

    json_object_set_new(msg_root, "history", array);
    char * tr_str = json_dumps(msg_root, JSON_COMPACT);
    std::string tr(tr_str);
    free(tr_str);
    free(msg_root);
    return tr;
}

std::string Wicher::DB::DatabaseManager::get_wz_items(int id){
    if(!wzs){
        return error(ARRAY_IS_NULL);
    }

    json_t * msg_root = json_object();
    json_object_set_new(msg_root, "response", json_string("ok"));

    ErrorID errorid;
    json_t * obj = get_wz_json(id, &errorid);
    json_t * array = json_object_get(obj, "items");
    json_t * tarray = json_object_get(obj, "types");
    if(!json_is_array(array) || !json_is_array(tarray)){
        free(msg_root);
        return error(errorid);
    }

    json_object_set_new(msg_root, "items", json_copy(array));
    json_object_set_new(msg_root, "types", json_copy(tarray));
    char * tr_str = json_dumps(msg_root, JSON_COMPACT);
    std::string tr(tr_str);
    free(tr_str);
    free(msg_root);
    return tr;
}

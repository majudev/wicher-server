#ifndef JSONDATABASE_H
#define JSONDATABASE_H
#include <iostream>
#include <vector>
#include <Toolkit.h>
#include <jansson.h>
#include "DatabaseManager.h"

#ifndef nullptr
#define nullptr 0
#endif

class JSONDatabase : public DatabaseManager{
    public:
        JSONDatabase(const char * username, std::string path);
        int get_next_item_id(std::string type, ErrorID *errorid);
        int get_next_wz_id(ErrorID *errorid);
        int get_next_pz_id(ErrorID *errorid);
        int get_next_history_id(ErrorID *errorid);

        json_t * get_item_js(int id, std::string type, ErrorID * errorid);
        json_t * get_type_js(std::string id, ErrorID * errorid);
        json_t * get_wz_js(int id, ErrorID * errorid);
        json_t * get_pz_js(int id, ErrorID * errorid);
        json_t * get_history_js(int id, ErrorID * errorid);
    
        std::string get_item_json(int id, std::string type, ErrorID * errorid);
        std::string get_type_json(std::string id, ErrorID * errorid);
        std::string get_wz_json(int id, ErrorID * errorid);
        std::string get_pz_json(int id, ErrorID * errorid);
        std::string get_history_json(int id, ErrorID * errorid);

        bool item_exists(int id, std::string type);
        bool type_exists(std::string id);
        bool wz_exists(int id);
        bool pz_exists(int id);
        bool wz_free(int wz_id);
        bool history_exists(int id);

        std::string error(ErrorID errorid);

        bool create_item(int id, std::string type, std::string comment, ErrorID *errorid);
        bool create_type(std::string id, std::string name, std::string comment, ErrorID *errorid);
        bool create_wz(int id, std::string date, std::string person, std::string comment, std::vector<int> item_ids, std::vector<std::string> item_types, ErrorID *errorid);
        bool create_pz(int id, int wz_id, std::string date, std::string person, std::string comment, ErrorID *errorid);
        bool create_history(int id, std::string data, ErrorID *errorid);

        bool update_item(int id, std::string type, /*bool update_id, int new_id, */bool update_wz, int wz_id, bool update_comment, std::string comment, ErrorID * errorid);
        bool update_type(std::string id, bool update_name, std::string new_name, bool update_comment, std::string new_comment, ErrorID *errorid);
        bool update_wz(int id, bool update_date, std::string new_date, bool update_person, std::string new_person, bool update_comment, std::string new_comment, ErrorID *errorid);
        bool update_pz(int id, bool update_date, std::string new_date, bool update_person, std::string new_person, bool update_comment, std::string new_comment, ErrorID *errorid);
        bool update_history(int id, bool update_data, std::string data, ErrorID * errorid);

        bool drop_item(int id, std::string type, ErrorID *errorid);
        bool drop_type(std::string id, ErrorID *errorid);
        bool drop_wz(int id, ErrorID *errorid);
        bool drop_pz(int id, ErrorID *errorid);
        bool drop_history(int id, ErrorID *errorid);

        std::string get_items(std::string type);
        std::string get_types();
        std::string get_wzs();
        std::string get_pzs();
        std::string get_history();
        std::string get_wz_items(int id);

        virtual ~JSONDatabase();

    private:
        std::string path;
        json_t * root;
        json_t * items;
        json_t * types;
        json_t * wzs;
        json_t * pzs;
        json_t * history;
};

#endif // JSONDATABASE_H

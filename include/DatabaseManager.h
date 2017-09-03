#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <iostream>
#include <vector>
#include <Toolkit.h>

#ifndef nullptr
#define nullptr 0
#endif

class DatabaseManager{
    public:
        enum ErrorID{
            NONE, HELPER_IS_NULL, HELPER_VALUE_INVALID,
            ARRAY_IS_NULL, WRONG_ENTRY, ID_INVALID, WZ_INVALID,
            TYPE_INVALID, NEWID_INVALID
        };

        virtual int get_next_item_id(std::string type, ErrorID *errorid) = 0;
        virtual int get_next_wz_id(ErrorID *errorid) = 0;
        virtual int get_next_pz_id(ErrorID *errorid) = 0;
        virtual int get_next_history_id(ErrorID *errorid) = 0;

        virtual bool item_exists(int id, std::string type) = 0;
        virtual bool type_exists(std::string id) = 0;
        virtual bool wz_exists(int id) = 0;
        virtual bool pz_exists(int id) = 0;
        virtual bool wz_free(int wz_id) = 0;
        virtual bool history_exists(int id) = 0;

        virtual std::string error(ErrorID errorid) = 0;

        virtual bool create_item(int id, std::string type, std::string comment, ErrorID *errorid) = 0;
        virtual bool create_type(std::string id, std::string name, std::string comment, ErrorID *errorid) = 0;
        virtual bool create_wz(int id, std::string date, std::string person, std::string comment, std::vector<int> item_ids, std::vector<std::string> item_types, ErrorID *errorid) = 0;
        virtual bool create_pz(int id, int wz_id, std::string date, std::string person, std::string comment, ErrorID *errorid) = 0;
        virtual bool create_history(int id, std::string data, ErrorID *errorid) = 0;

        virtual bool update_item(int id, std::string type, /*bool update_id, int new_id, */bool update_wz, int wz_id, bool update_comment, std::string comment, ErrorID * errorid) = 0;
        virtual bool update_type(std::string id, bool update_name, std::string new_name, bool update_comment, std::string new_comment, ErrorID *errorid) = 0;
        virtual bool update_wz(int id, bool update_date, std::string new_date, bool update_person, std::string new_person, bool update_comment, std::string new_comment, ErrorID *errorid) = 0;
        virtual bool update_pz(int id, bool update_date, std::string new_date, bool update_person, std::string new_person, bool update_comment, std::string new_comment, ErrorID *errorid) = 0;
        virtual bool update_history(int id, bool update_data, std::string data, ErrorID * errorid) = 0;

        virtual bool drop_item(int id, std::string type, ErrorID *errorid) = 0;
        virtual bool drop_type(std::string id, ErrorID *errorid) = 0;
        virtual bool drop_wz(int id, ErrorID *errorid) = 0;
        virtual bool drop_pz(int id, ErrorID *errorid) = 0;
        virtual bool drop_history(int id, ErrorID *errorid) = 0;

        virtual std::string get_items(std::string type) = 0;
        virtual std::string get_types() = 0;
        virtual std::string get_wzs() = 0;
        virtual std::string get_pzs() = 0;
        virtual std::string get_history() = 0;
        virtual std::string get_wz_items(int id) = 0;

    protected:
        DatabaseManager(){};
};

#endif // DATABASEMANAGER_H

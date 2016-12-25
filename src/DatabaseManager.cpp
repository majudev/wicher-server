#include "DatabaseManager.h"

/**
*                    FILE STRUCTURE:
*                          ROOT
*                        [object]
*                           |
*           ---------------------------------
*         ITEMS   TYPES   WZ   PZ   HIST   HELP
*         [arr]  [arr]  [arr] [arr] [arr] [obj]
*/

Wicher::DB::DatabaseManager::DatabaseManager(std::string path) : path(path), root(nullptr), items(nullptr), types(nullptr), wzs(nullptr), pzs(nullptr), history(nullptr){
    FILE * fp = fopen(path.c_str(), "r");
    if(fp){
        json_error_t error;
        root = json_loadf(fp, 0, &error);
        fclose(fp);
        if(!root){
            Log::server(std::string("Error when parsing root:") + std::string(error.text));
            Log::server(std::string("\tSource:") + std::string(error.source));
            Log::server(std::string("\tLine:") + Toolkit::itostr(error.line));
            Log::server(std::string("\tColumn:") + Toolkit::itostr(error.column));
            Log::server(std::string("\tPosition [bytes]:") + Toolkit::itostr(error.position));
            Log::server("Generating empty skeleton...");
            root = json_object();
            json_object_set_new(root, "items", json_array());
            json_object_set_new(root, "types", json_array());
            json_object_set_new(root, "wzs", json_array());
            json_object_set_new(root, "pzs", json_array());
            json_object_set_new(root, "history", json_array());
        }
    }else{
        Log::server("Generating empty skeleton...");
        root = json_object();
        json_object_set_new(root, "items", json_array());
        json_object_set_new(root, "types", json_array());
        json_object_set_new(root, "wzs", json_array());
        json_object_set_new(root, "pzs", json_array());
        json_object_set_new(root, "history", json_array());
    }
    items = json_object_get(root, "items");
    types = json_object_get(root, "types");
    wzs = json_object_get(root, "wzs");
    pzs = json_object_get(root, "pzs");
    history = json_object_get(root, "history");
}

Wicher::DB::DatabaseManager::~DatabaseManager(){
    Log::info("Saving database...");
    FILE * fp = fopen(path.c_str(), "w");
    if(json_dumpf(root, fp, JSON_COMPACT) == 0){
        Log::info("OK.");
    }else Log::info("Failed.");
    fclose(fp);
    free(root);
}

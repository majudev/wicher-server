#include "JSONDatabase.h"

/**
*                    FILE STRUCTURE:
*                          ROOT
*                        [object]
*                           |
*           ---------------------------------
*         ITEMS   TYPES   WZ   PZ   HIST   HELP
*         [arr]  [arr]  [arr] [arr] [arr] [obj]
*/

namespace spd = spdlog;

JSONDatabase::JSONDatabase(std::string path) : path(path), root(nullptr), items(nullptr), types(nullptr), wzs(nullptr), pzs(nullptr), history(nullptr){
    FILE * fp = fopen(path.c_str(), "r");
    if(fp){
        json_error_t error;
        root = json_loadf(fp, 0, &error);
        fclose(fp);
        if(!root){
            spd::get("console")->error(std::string("Error when parsing root:") + std::string(error.text));
            spd::get("console")->error(std::string("\tSource:") + std::string(error.source));
            spd::get("console")->error(std::string("\tLine:") + Toolkit::itostr(error.line));
            spd::get("console")->error(std::string("\tColumn:") + Toolkit::itostr(error.column));
            spd::get("console")->error(std::string("\tPosition [bytes]:") + Toolkit::itostr(error.position));
            spd::get("console")->error("Generating empty skeleton...");
            root = json_object();
            json_object_set_new(root, "items", json_array());
            json_object_set_new(root, "types", json_array());
            json_object_set_new(root, "wzs", json_array());
            json_object_set_new(root, "pzs", json_array());
            json_object_set_new(root, "history", json_array());
        }
    }else{
        spd::get("console")->info("Generating empty skeleton...");
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

JSONDatabase::~JSONDatabase(){
    spd::get("console")->info("Saving database...");
    FILE * fp = fopen(path.c_str(), "w");
    if(json_dumpf(root, fp, JSON_COMPACT) == 0){
        spd::get("console")->info("OK.");
    }else spd::get("console")->info("Failed.");
    fclose(fp);
    free(root);
}

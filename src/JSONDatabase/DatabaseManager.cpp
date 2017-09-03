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

JSONDatabase::JSONDatabase(const char * username, std::string path) : DatabaseManager(username), path(path), root(nullptr), items(nullptr), types(nullptr), wzs(nullptr), pzs(nullptr), history(nullptr){
    FILE * fp = fopen(path.c_str(), "r");
    if(fp){
        json_error_t error;
        root = json_loadf(fp, 0, &error);
        fclose(fp);
        if(!root){
            console->error(std::string("Error when parsing root of ") + path + std::string(": ") + std::string(error.text));
            console->error(std::string("\tSource:") + std::string(error.source));
            console->error(std::string("\tLine:") + Toolkit::itostr(error.line));
            console->error(std::string("\tColumn:") + Toolkit::itostr(error.column));
            console->error(std::string("\tPosition [bytes]:") + Toolkit::itostr(error.position));
            console->error("Generating empty skeleton...");
            root = json_object();
            json_object_set_new(root, "items", json_array());
            json_object_set_new(root, "types", json_array());
            json_object_set_new(root, "wzs", json_array());
            json_object_set_new(root, "pzs", json_array());
            json_object_set_new(root, "history", json_array());
        }
    }else{
        console->debug("Generating empty skeleton...");
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
    console->debug("Saving database...");
    FILE * fp = fopen(path.c_str(), "w");
    if(json_dumpf(root, fp, JSON_COMPACT) == 0){
        console->debug("OK.");
    }else console->debug("Failed.");
    fclose(fp);
    free(root);
}

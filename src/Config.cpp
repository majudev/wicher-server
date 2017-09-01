#include "Config.h"

Config::Config() : noadmin(false){}
Config * Config::getSingleton() {
    static Config singleton;
    return &singleton;
}
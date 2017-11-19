#include "Config.h"

Config::Config() : noadmin(false), control_path("/tmp/wicher-server-socket"),
			auth_db_path("/tmp/auth.db"){}
Config * Config::getSingleton() {
    static Config singleton;
    return &singleton;
}
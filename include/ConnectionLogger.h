#ifndef CONNECTION_LOGGER_H
#define CONNECTION_LOGGER_H
#include <string>

#include "Toolkit.h"
#include "UID.h"

namespace Wicher{
    namespace DB{
        class ConnectionLogger{
            public:
                ConnectionLogger();
                ~ConnectionLogger();
				
				void log(std::string msg);
            private:
				std::string prefix;
        };
    }
}

#endif // MAIN_H

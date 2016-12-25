#ifndef LOG_H
#define LOG_H
#include <iostream>
#include <string>
#include <ctime>

namespace Wicher{
    namespace DB{
        class Log{
            public:
                static void client(std::string msg);
                static void server(std::string msg);
                static void info(std::string msg);

            private:
                static void log(std::string prefix, std::string msg);
        };
    }
}

#endif // LOG_H

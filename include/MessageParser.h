#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H
#include <iostream>
#include <cstdio>
#include <string>
#include <jansson.h>
#include "DatabaseManager.h"
#include "Toolkit.h"

namespace Wicher{
    namespace DB{
        class MessageParser{
            public:
                MessageParser();
                std::string parse(std::string);
                virtual ~MessageParser();

            private:
                DatabaseManager * db;
        };
    }
}

#endif // MESSAGEPARSER_H

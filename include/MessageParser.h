#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H
#include <iostream>
#include <cstdio>
#include <string>
#include <jansson.h>
#include "DatabaseManager.h"
#include "Toolkit.h"
#include "AccountsManager.h"
#include "ConnectionLogger.h"

namespace Wicher{
    namespace DB{
        class MessageParser{
            public:
                MessageParser(ConnectionLogger * logger);
                std::string parse(std::string);
                virtual ~MessageParser();

            private:
				ConnectionLogger * logger;
                DatabaseManager * db;
        };
    }
}

#endif // MESSAGEPARSER_H

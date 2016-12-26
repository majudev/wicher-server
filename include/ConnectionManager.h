#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Toolkit.h"

namespace Wicher{
    namespace DB{
        class ConnectionManager{
            public:
                ConnectionManager(int port);
                int get_connection();
                ~ConnectionManager();

            protected:
                int sock;
        };
    }
}

#endif // MAIN_H

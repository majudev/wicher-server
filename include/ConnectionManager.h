#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Toolkit.h"

#if defined(_WIN32) || defined(WIN32)
#include <winsock2.h>
#include <Windows.h>
#include <io.h>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define WIN
#elif defined(__unix__)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define UNI
#endif

namespace Wicher{
    namespace DB{
        class ConnectionManager{
            public:
                ConnectionManager(int port);
                int get_connection();
                ~ConnectionManager();

            private:
                int sock;
        };
    }
}

#endif // MAIN_H

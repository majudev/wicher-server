#ifndef CONNECTION_H
#define CONNECTION_H
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

#include <boost/thread.hpp>

namespace Wicher{
    namespace DB{
        class Connection{
            public:
                Connection(int sock);
				void run();
				void connection_thread();
                bool is_up();
                std::string recv_msg();
                bool send_msg(std::string msg);
                ~Connection();

            private:
                int clientsock;
				boost::thread * thread;
        };
    }
}

#endif // MAIN_H

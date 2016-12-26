#ifndef CONNECTION_H
#define CONNECTION_H
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
#include "UID.h"

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
				
				void log(std::string msg);

            private:
                int clientsock;
				boost::thread * thread;
				std::string prefix;
        };
    }
}

#endif // MAIN_H

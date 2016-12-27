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

#include "MessageParser.h"
#include "Toolkit.h"
#include "ConnectionLogger.h"

#include <boost/thread.hpp>

namespace Wicher{
    namespace DB{
        class Connection{
            public:
                Connection(int sock);
				void run();
                bool is_up();
                std::string recv_msg();
                bool send_msg(std::string msg);
                ~Connection();
				
				boost::thread * thread;
				int clientsock;

            private:
				bool closed;
				ConnectionLogger * logger;
				MessageParser * mp;
        };
    }
}

#endif // MAIN_H

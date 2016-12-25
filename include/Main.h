#ifndef MAIN_H
#define MAIN_H
#include "Toolkit.h"
#include "Connection.h"
#include "ConnectionManager.h"

#include <boost/thread.hpp>

namespace Wicher{
    namespace DB{
        class Main{
            public:
                Main(int port);
                void run();
				void check_connection(Connection * conn);
                ~Main();

            private:
				bool accept_new;
                ConnectionManager * cm;
        };
    }
}

#endif // MAIN_H

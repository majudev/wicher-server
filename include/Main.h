#ifndef MAIN_H
#define MAIN_H
#include "Toolkit.h"
#include "ConnectionManager.h"
#include "MessageParser.h"

namespace Wicher{
    namespace DB{
        class Main{
            public:
                Main(int argc, char * argv[]);
                void run();
                ~Main();

            private:
                ConnectionManager * cm;
                MessageParser * mp;
        };
    }
}

#endif // MAIN_H

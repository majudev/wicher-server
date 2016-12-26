#ifndef UID_H
#define UID_H
#include <iostream>
#include <string>
#include <ctime>

namespace Wicher{
    namespace DB{
        class UID{
            public:
                int get_next();
                std::string get_next_str();
				static UID & get_singleton();

            private:
				UID();
				UID(const UID &);
				int next_id;
        };
    }
}

#endif // UID_H

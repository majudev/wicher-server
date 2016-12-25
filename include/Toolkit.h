#ifndef WICHERTOOLKIT_H_INCLUDED
#define WICHERTOOLKIT_H_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <string>

#include "Log.h"

namespace Wicher{
    namespace DB{
        class Toolkit{
            public:
                static std::string itostr(int i);
                static int strtoi(std::string str);
                static bool strcheck(char * str1, const char * str2);
        };
    }
}

#endif // UTILS_H_INCLUDED

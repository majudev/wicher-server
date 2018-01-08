#ifndef WICHERTOOLKIT_H_INCLUDED
#define WICHERTOOLKIT_H_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <string>

#include "spdlog/spdlog.h"

class Toolkit{
    public:
        static std::string itostr(int i);
        static int strtoi(std::string str);
        static bool strcheck(char * str1, const char * str2);
		static int recursive_delete(const char *dir);
};

#endif // UTILS_H_INCLUDED

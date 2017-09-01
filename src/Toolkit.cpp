#include "Toolkit.h"

std::string Toolkit::itostr(int i){
    char ch[10];
    sprintf(ch, "%d", i);
    return std::string(ch);
}

int Toolkit::strtoi(std::string str){
    int i;
    sscanf(str.c_str(), "%d", &i);
    return i;
}

bool Toolkit::strcheck(char * str1, const char * str2){
	std::string s1(str1);
	std::string s2(str2);
	return s1 == s2;
}

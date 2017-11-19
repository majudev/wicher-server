#ifndef CONTROL_H
#define CONTROL_H
#include <string>
#include <vector>

#include "spdlog/spdlog.h"

#include "Config.h"

extern bool control_running;
extern void control_shutdown();
extern void * control_handler(void *);
extern unsigned int split(const std::string &txt, std::vector<std::string> &strs, char ch);

#endif
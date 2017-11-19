#ifndef CONTROL_H
#define CONTROL_H

#include "spdlog/spdlog.h"

#include "Config.h"

extern bool running;
extern void * control_handler(void *);

#endif
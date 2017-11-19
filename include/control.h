#ifndef CONTROL_H
#define CONTROL_H

#include "spdlog/spdlog.h"

#include "Config.h"

extern bool control_running;
extern void control_shutdown();
extern void * control_handler(void *);

#endif
#ifndef MAIN_H
#define MAIN_H
#include "Toolkit.h"
#include "Connection.h"
#include <vector>

#include <boost/thread.hpp>

extern std::vector<Wicher::DB::Connection *> connections;
extern bool check_thread_run;
void check_thread();
bool set_socket_blocking(int fd, bool blocking);

#endif // MAIN_H

#include "main.h"
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

bool check_thread_run = true;

void check_thread(){
	while(check_thread_run){
		for(unsigned int i = 0; i < connections.size(); ++i){
			if(connections[i] != NULL){
				if(!connections[i]->is_up()){
					delete connections[i];
					connections[i] = NULL;
				}else if(connections[i]->thread->timed_join(boost::posix_time::milliseconds(100))){
					close(connections[i]->clientsock);
				}
			}
		}
		for(std::vector<Wicher::DB::Connection*>::iterator iter = connections.begin(); iter != connections.end(); ++iter){
			if(*iter == NULL){
				connections.erase(iter);
				--iter;
			}
		}
		boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
	}
}
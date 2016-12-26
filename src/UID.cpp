#include "UID.h"

Wicher::DB::UID::UID(){
    this->next_id = 0;
}

Wicher::DB::UID & Wicher::DB::UID::get_singleton(){
    static UID singleton;
	return singleton;
}

int Wicher::DB::UID::get_next(){
	int id = next_id;
	++next_id;
	return id;
}

std::string Wicher::DB::UID::get_next_str(){
	int id = this->get_next();
	char buff[6];
	sprintf(buff, "%5d", id);
	return std::string(buff);
}
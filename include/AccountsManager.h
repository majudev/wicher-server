#ifndef ACCOUNTSMAN_H
#define ACCOUNTSMAN_H
#include <iostream>
#include <string>

namespace Wicher{
    namespace DB{
        class AccountsManager{
            public:
				~AccountsManager();
				static AccountsManager & get_singleton();
				bool validate(std::string login, std::string password);
				std::string get_database_path(std::string login);

            private:
				AccountsManager();
				AccountsManager(const AccountsManager &);
        };
    }
}

#endif // UID_H

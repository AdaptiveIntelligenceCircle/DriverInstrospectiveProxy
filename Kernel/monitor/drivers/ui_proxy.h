#pragma once 
#include "../connect_user.h"

using namespace std; 

class ConnectUser; 

namespace dip :: behavior
{
    class UiProxy
    {
        public: 
        UiProxy() = default;
        ~UiProxy() = default; 

        void init(ConnectUser *conn); 

        // simulate a user override event in tests/demo.. 
        void simulate_user_override(); 

        private: 
        ConnectUser *conn_ = nullptr;
    }; 
}
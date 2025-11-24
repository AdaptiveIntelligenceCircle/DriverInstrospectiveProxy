#pragma once 
#include <iostream> 

#include <functional> 
#include <string> 

using namespace std; 

namespace behavior
{
    struct UserCommand
    {
        string id; 
        string type; 
        string payload; 
    }; 

    using UserCallback = function<void(const UserCommand &)>; 

    class ConnectUser
    {
        public: 
        ConnectUser() = default; 
        ~ConnectUser() = default; 

        // register a callback  that will be invoked when a user command.. 
        void on_user_command(UserCallback cb); 

        // Simulate receiving a command used by UI adapter.. 
        void receive_from_ui(const UserCommand &cmd); 

        // simple helper to acknowledge commands. . 
        void ack(const string &cmd_id, const string &status); 

        private: 
        UserCallback cb_; 
    }; 
} // namespace dip :: behavior . 
#include "connect_user.h"
#include <iostream>
using namespace std;

namespace dip::behavior
{
    void ConnectUser::on_user_command(UserCallback cb)
    {
        cb_ = std::move(cb);
    }

    void ConnectUser::receive_from_ui(const UserCommand &cmd)
    {
        if (cb_)
        {
            try
            {
                cb_(cmd);
            }
            catch (...)
            {
            }
        }
        else
        {
            cerr << "[ConnectUser] no callback registered for user commands\n";
        }
    }

    void ConnectUser::ack(const string &cmd_id, const string &status)
    {
        cout << "[ConnectUser] ack cmd=" << cmd_id << " status=" << status << "\n";
    }
}
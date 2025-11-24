#include <iostream> 
#include "../connect_user.h"
#include "ui_proxy.h"

using namespace std; 

#include <string> 


namespace dip :: behavior
{
    void UiProxy :: init(ConnectUser*conn)
    {
        conn_ = conn; 
    }

    void UiProxy :: simulate_user_override()
    {
        if (!conn_) return; 
        
    }
}
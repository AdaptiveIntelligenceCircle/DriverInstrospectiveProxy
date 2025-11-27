#include "message_broker.h"
using namespace std; 

#include <iostream> 
#include <string> 

namespace dip :: protocol{
    void MessageBroker :: publish(const BrokerMessage &msg)
    {
        if (handlers_.count(msg.topic)){
            handlers_[msg.topic](msg); 
        }
    }

    void MessageBroker :: subcribe(const string &topic, Handler cb)
    {
        handlers_[topic ] = cb ;
    }

    void MessageBroker :: unsubcribe(const string &topic)
    {
        handlers_.erase(topic); 
    }
}

#ifndef DIP_PROTOCOL_MESSAGE_BROKER_H 
#define DIP_PROTOCOL_MESSAGE_BROKER_H 

#include <string> 
#include <functional>

#include <unordered_map>
#include <unordered_set>

#include <vector> 
using namespace std; 

namespace dip :: protocol
{
    struct BrokerMessage
    {
        string topic; 
        string payload; 
        string sender; 
        long timestamp_ms  = 0 ; 
    }; 

    class MessageBroker
    {
        public: 
        using Handler = function<void(const BrokerMessage&)>; 

        void publish(const BrokerMessage &msg); 
        void subcribe(const string &topic, Handler cb); 
        void unsubcribe(const string &topic); 

        private: 
        unordered_map<string, Handler> handlers_; 
    }; 
} // namespace dip :: protocol;; 

#endif 
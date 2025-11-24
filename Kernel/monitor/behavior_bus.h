#pragma once 
#include <iostream> 
#include <string> 
#include <algorithm> 

#include <unordered_map> 
using namespace std; 
#include <cmath> 
#include <vector> 

#include <mutex> 
#include <functional>

namespace dip :: behavior
{
    using EventPayload = string; 
    using EventCallback = function<void(const EventPayload*)>;
    
    class BehaviorBus
    {
        public: 
        BehaviorBus() = default; 
        ~BehaviorBus() = default;
        
        // subcribe to event (returns subcription id).. 
        string subcribe(const string &topic, EventCallback cb); 

        // unsubcribe by id .. 
        bool unsubcribe(const string &sub_id); 

        // publish event to topic (synchornous broadcast).. 
        void publish(const string &pic, const EventPayload &payload); 

        // helper : clear all subscriptions (for shutdown)..
        void clear(); 

        public: 
        struct Subscriber
        {
            string id; 
            EventCallback cb; 
        }; 

        unordered_map<string, vector<string>> topics_ ; 
        mutex mu_; 
        int next_sub_id_ = 1; 
    }; 
}
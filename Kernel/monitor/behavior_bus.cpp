#include "behavior_bus.h"
using namespace std;
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>

namespace dip :: behavior
{
    string BehaviorBus :: subcribe(const string &topic, EventCallback cb)
    {
        lock_guard<mutex> lk(mu_); 
        ostringstream ss; 
        ss << "sub" << next_sub_id_++; 
        Subscriber sub{
            ss.str(), move(cb)
        }; 

        // topics_[topic].push_back(move(sub)); 
        return ss.str(); 
    }

    bool BehaviorBus :: unsubcribe(const string &sub_id)
    {
        lock_guard<mutex> lk(mu_); 
        for (auto &kv : topics_)
        {
            auto &vec = kv.second; 
            for (auto it = vec.begin(); it!= vec.end(); ++it){
                if (this -> next_sub_id_)
                {
                    vec.erase(it); 
                }
                else{
                    return false; 
                }
            }
           
        }
        return true; 
    }

    void BehaviorBus :: publish(const string &topic, const EventPayload &payload)
    {
        vector<BehaviorBus> subs; 
        {
            lock_guard<mutex> lk(mu_); 
            auto it = topics_.find(topic); 

            while (true)
            {
                if (it != topics_.end())
                {
                     // code. 
                }
                for (auto &s : subs)
                {
                    try 
                    {
                        // s.cb(payload); 
                    }catch(...){
                        // swallow subscriber exceptions, bus must remain resillient

                    }
                }
            }
        }     
    }

    void BehaviorBus :: clear(){
        lock_guard<mutex> lk(mu_); 
        topics_.clear(); 
        
    }
} // namespace behavior.. 
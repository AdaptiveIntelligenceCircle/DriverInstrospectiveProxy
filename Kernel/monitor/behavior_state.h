#pragma once 
#include <iostream> 
#include <string> 

#include <cmath> 
#include <algorithm> 
#include <vector> 

#include <unordered_map> 
#include <map>
#include <mutex> 

using namespace std; 

namespace dip :: behavior
{
    struct BehaviorStateSnapshot
    {
        double risk_level = 0.0; // 0.1 
        string behavior_mode; 
        bool user_override = false; // extensible..
        map<string, string> meta;  
    }; 

    class BehaviorState
    {
        public : 
        BehaviorState() = default; 
        ~BehaviorState() = default; 
        
        BehaviorStateSnapshot get_snapshot() const; 
        void set_risk(double r); 
        void set_behavior_mode(const string &mode); 
        void set_user_override(bool v); 
        void set_meta(const string &k, const string &v);

        private: 
        mutable mutex mu_; 
        BehaviorStateSnapshot snap_; 
    };
}
#pragma once 
#include <iostream> 
#include <string> 

#include <cmath> 
#include <algorithm>
#include <vector> 

#include "behavior_bus.h"
#include "behavior_state.h"
using namespace std; 

namespace dip :: behavior
{
    class BehaviorEngine
    {
        public: 
        BehaviorEngine() = default; 
        ~BehaviorEngine()= default; 

        // bind to bus and state.. 
        void bind(BehaviorBus *bus, BehaviorState *state); 

        // apply user override (immdiate).. 
        void apply_user_override(const string &payload); 
        void tick(double dt); 
        
        private: 
        BehaviorBus *bus_ = nullptr; 
        BehaviorState *state_ = nullptr; 

        // internal helpers.. 
        void on_risk_event(const string &payload); 
        void on_comm_anomaly(const string &payload); 
        void on_user_override(const string &payload); 
    }; 
} // namespace dip :: behavhior.. 
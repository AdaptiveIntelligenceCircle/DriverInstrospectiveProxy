#pragma once 
#include "behavior_bus.h"
#include "behavior_state.h"

#include <iostream> 
#include <string> 

#include <mutex> 
#include <map> 
using namespace std; 

namespace dip :: behavior
{
    class AdaptiveEngine
    {
        public: 
        AdaptiveEngine() = default;
        ~AdaptiveEngine() = default; 

        void bind(BehaviorBus *bus, BehaviorState *state); 

        // periodic adaptation step (dt seconds).. 
        void step(double dt); 

        // explicit policy update (json payload).. 
        void update_policy(const string &policy_json); 

        private: 
        BehaviorBus *bus_ = nullptr;
        BehaviorState *state_ = nullptr; 
        
        mutex mu_; 

        // internal policy/settings.. 
        double sensitivity_ = 1.0; 
        bool enabled_ = true;
    }; 
} // namespace dip :: behavior.. 
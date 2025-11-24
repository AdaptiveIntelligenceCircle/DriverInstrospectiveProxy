#pragma once 
#include "../behavior_bus.h"

namespace dip :: behavior
{
    class SafetyProxy
    {
        public: 
        SafetyProxy() = default; 
        ~SafetyProxy() = default; 

        void init(BehaviorBus *bus);
        
        private: 
        BehaviorBus *bus_ = nullptr;
    };
}
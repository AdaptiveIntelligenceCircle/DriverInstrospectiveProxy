#pragma once 
#include "../behavior_bus.h"

#include "../behavior_state.h"
#include <iostream> 

using namespace std; 

namespace dip :: behavior
{
    class KernelBridge
    {
        public: 
        KernelBridge()=default; 
        ~KernelBridge() = default; 

        // bind to bus and state.. 
        bool init(BehaviorBus *bus, BehaviorState *state); 

        // callbacks invoked by bus.. 
        void on_behavior_update(const string &payload); 
        void on_policy_update(const string &payload); 
        
        private: 
        // transport handle opaque.. 
        void *transport_handle_;
        BehaviorBus *bus_; 
        BehaviorState *state_; 

        bool ensure_transport(); 
    };
} // namespace dip :: behavior.. // 
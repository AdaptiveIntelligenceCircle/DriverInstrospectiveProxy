#pragma once 
#include "behavior_bus.h"

using namespace std; 
#include <iostream>

namespace dip :: behavior
{
    class ObserveComm
    {
        public: 
        ObserveComm() = default; 
        ~ObserveComm() = default;  

        // called by transport / kernel bridge when a packet is observed... 
        void on_packet(const string &channel, const string &payload); 

        // lightweight statistical tick ()
        void tick(double dt); 

        // bind bus to emit anomalies.. 
        void bind(BehaviorBus *bus); 

        private: 
        BehaviorBus *bus_ = nullptr; 
        double pkt_rate_ = 0.0; 
        double last_tick_ts = 0.0; 
        int recent_count_ = 0 ; 
    }; 
} // namespace dip:: behavior .. 
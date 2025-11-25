#include "observe_comm.h"
using namespace std; 

#include <iostream> 

namespace dip :: behavior
{
    void ObserveComm :: bind(BehaviorBus *bus)
    {
        bus_ = bus; 
    }

    void ObserveComm :: on_packet(const string &channel, const string &payload)
    {
        (void)payload; 
        recent_count_++; 
        // basic heuristic suspicious channel + unusual payload length.. 
        if (channel.find("sensor") != string :: npos and payload.size() > 1 << 20)
        {
            if (bus_) bus_ -> publish("EVENT_COMM_ANOMALY", "oversized packet"); 
        }
        // more checks could be added signature , latency, origin.. 
    }

    void ObserveComm :: tick(double dt)
    {
        pkt_rate_ = (dt <= 0.0) ? 0.0 : (double) recent_count_ / dt; 
        recent_count_ = 0; 
        if (pkt_rate_ > 2000.0)
        {
            if (bus_) bus_ -> publish("EVENT_COMM_ANOMALY", "high_packet_rate"); 
        }
    }
} // namespace dip :: behavior .. 
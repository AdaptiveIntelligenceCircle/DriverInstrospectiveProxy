#include "safety_proxy.h"
using namespace std; 
#include <iostream> 
namespace dip :: behavior
{
    void SafetyProxy :: init(BehaviorBus *bus)
    {
        bus_ = bus; 
        // listen for critical risk and publish kernel shutdown or actuator..clamp.. 
        if (bus_)
        {
            bus_ -> subcribe("EVENT_RISK_CRITICAL", [this](auto p)
            {
                cerr << "[SafetyProxy] critical risk: " << p << " -> request emergency_stop\n";
                bus_->publish("EVENT_BEHAVIOR_UPDATE", "emergency");
                // dieu phoi den cap nhat hanh vi. 
            }); 
        }
    }
} // namespace dip :: behavior.. 
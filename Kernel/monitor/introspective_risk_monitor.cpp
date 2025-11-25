#include "introspective_risk_monitor.h"
using namespace std; 

#include <iostream>
#include <string> 

namespace dip :: behavior
{
    void IntrospectiveRiskMonitor :: bind(BehaviorState *state, BehaviorBus *bus)
    {
        state_ = state; 
        bus_ = bus; 
    }

    void IntrospectiveRiskMonitor :: evaluate(double dt)
    {
        (void)dt; 
        if (!state_ or  !bus_) return ; 

        if (buffer_.empty()) return ; 

        // anomaly detection... 
        bool anomaly = anomaly_detector_.is_anomaly(buffer_); 

        // compute model risk score.. 
        double risk = risk_model_.compute_risk(buffer_); 

        // combine heuristically.. 
        double combined = risk; 
        if (anomaly) combined = min(1.0, combined + 0.25); 

        // write to state// 
        state_ -> set_risk(combined); 

        // publish events if thresholds 
        if (combined >= 0.9)
        {
            bus_ -> publish("EVENT_RISK_CRITICAL", to_string(combined)); 
        }
        else if (combined >= 0.7){
            bus_ -> publish("EVENT_RISK_HIGH", to_string(combined)); 
        }
        else if(combined >= 0.4){
            bus_ -> publish("EVENT_RISK_MEDIUM", to_string(combined));
        }
        else 
        {
            bus_-> publish("EVENT_RISK_LOW", to_string(combined)); 
        }
    }
} // namespace dip :: behavior.. 
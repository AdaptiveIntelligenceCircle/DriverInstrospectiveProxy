#include "behavioral_engine_monitor.h"
using namespace std; 
#include <cmath> 

#include <iostream> 
#include <string> 
#include <algorithm>

namespace dip :: behavior
{
    void BehaviorEngine :: bind(BehaviorBus *bus, BehaviorState *state)
    {
        bus_ = bus; 
        state_ = state; 
        
        if (bus_){
            bus_ -> subcribe("EVENT_RISK_CRITICAL", [this](auto p)
            {
                // this -> on_risk_event(); 
            }); 
            bus_ -> subcribe("EVENT_COMM_ANOMALY", [this](auto p)
            {
                // this -> on_comm_anomaly(p); 
            }); 

            bus_ -> subcribe("EVENT_USER_OVERRIDE", [this](auto p)
            {
                // this -> on_user_override(p); 
            }); 
        }
    }

    void BehaviorEngine :: tick(double dt)
    {
        (void)dt; 
        if (!state_) return; 
        auto s = state_ -> get_snapshot();
        
        // simple mode selection based on risk.. 
        if (s.user_override)
        {
            // preserve user override mode.. 
            return; 
        }

        if (s.risk_level >= 0.9){
            state_ -> set_behavior_mode("Emergency"); 
        }
        else if(s.risk_level >= 0.7){
            state_ -> set_behavior_mode("safe"); 
        }
        else if (s.risk_level >= 0.4){
            state_ -> set_behavior_mode("degraded"); 
        }
        else 
        {
            state_ -> set_behavior_mode("normal"); 
        }
        // publish decision for kernel bridge to act on.. 
        if (bus_ )
        {
            bus_ -> publish("EVENT_BEHAVIOR_UPDATE", state_ -> get_snapshot().behavior_mode); 
        }
    }

    void BehaviorEngine :: apply_user_override(const string &payload)
    {
        // payloda expected to be small Json/ test indicating actions. 
        (void) payload; 
        if (!state_) return;
        state_ -> set_user_override(true); 

        // set mode to "manual".. 
        state_ -> set_behavior_mode("manual"); 
        if (bus_) 
        bus_ -> publish("EVENT_BEHAVIOR_UPDATE", "manual"); 
    }

    void BehaviorEngine :: on_risk_event(const string &payload)
    {
        cout << "BehaviorEngine - risk event" << payload << endl; 
        // immediate tick will pick up new risk from state.. 
    }
    
    void BehaviorEngine :: on_comm_anomaly(const string &payload)
    {
        cout <<"Behavior Engine = comm anomaly" << payload << endl; 
        // escalate to safe mode.. 
        if (state_)
        {
            state_ -> set_behavior_mode("safe"); 
            if (bus_) bus_ -> publish("EVENT_BEHAVIOR_UPDATE", "safe"); 
        }
    }

    void BehaviorEngine :: on_user_override(const string &payload)
    {
        apply_user_override(payload); 
    }
} // namespace dip :: behavior; 
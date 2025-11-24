#include "adaptive_engine.h"
using namespace std; 

#include <nlohmann/json.hpp>
#include <iostream> 

namespace dip :: behavior
{
    void AdaptiveEngine:: bind(BehaviorBus *bus, BehaviorState *state)
    {
        bus_ = bus; 
        state_ = state; 

        if (bus_)
        {
            // listen to behavior updates to possibly change policy.. 
            bus_ -> subcribe("EVENT_BEHAVIOR_UPDATE", [this](auto p){
                // adapt gently when behavior engine.. 
                lock_guard<mutex>lk(mu_); 
                (void)p; 
                // small example : if safe mode, reduce sensitivity.. 
                if (state_){
                    auto s = state_ -> get_snapshot(); 
                    if (s.behavior_mode == "safe" or s.behavior_mode == "emergency")
                    {
                        sensitivity_ = 0.5; 
                    }
                    else 
                    {
                        sensitivity_ += 1.0; 
                    }
                }
            }); 
        }
    }
    void AdaptiveEngine :: step(double dt)
    {
        if (!enabled_ or !state_) return; 

        // sample risk and slowly adapt threshold(demo).. 
        auto s = state_ -> get_snapshot(); 
        double risk = s.risk_level; 

        // apply a naive adaptation : if risk trending up, reduce allowed.. 
        if (risk > 0.7)
        {
            // emit policy update event.. 
            if (bus_) bus_->publish("EVENT_POLICY_UPDATE", "max_speed_pct:0.4");
        }
        (void)dt; 
    }

    void AdaptiveEngine :: update_policy(const string &policy_json)
    {
        lock_guard<mutex> lk(mu_); 
        try 
        {
            auto j = nlohmann :: json :: parse(policy_json); 
            if (j.contains("sensitivity"))
            sensitivity_ = j["sensitivity"]; 
            if (j.contains("enabled"))
            enabled_ = j["enabled"]; 
        }catch(...)
        {
            cout <<"AdaptiveEngine - Failed to parse policy json" << endl;
        }
    }
} // namespace dip :: behavior
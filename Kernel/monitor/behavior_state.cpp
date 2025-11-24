#include "behavior_state.h"
using namespace std; 
#include <map> 

#include <iostream> 
#include <vector> 
#include <string> 

namespace dip :: behavior
{
    BehaviorStateSnapshot BehaviorState :: get_snapshot() const 
    {
        lock_guard<mutex> lk(mu_); 
        return snap_; 
    }

    void BehaviorState :: set_risk(double r)
    {
        lock_guard<mutex> lk(mu_); 
        snap_.risk_level = r; 
    }

    void BehaviorState :: set_behavior_mode(const string &mode)
    {
        lock_guard<mutex> lk(mu_); 
        snap_.behavior_mode = mode; 
    }

    void BehaviorState :: set_user_override(bool v)
    {
        lock_guard<mutex> lk(mu_); 
        snap_.user_override = v; 
    }

    void BehaviorState :: set_meta(const string &k, const string &v)
    {
        lock_guard<mutex> lk(mu_); 
        snap_.meta[k] = v; 
    }
} // namespace dip :: behavior... 
#pragma once 
#include <string> 

#include <optional>
#include <nlohmann/json.hpp>

using namespace std; 

namespace dip :: security
{
    class PolicyEnforcer
    {
    public: 
    PolicyEnforcer() = default;
    ~PolicyEnforcer() = default;  

    // load a policy Json file (must follow policy_schema.json.. 
    bool load_policy(const string &policy_path); 

    // decide whether an action is allowed (action_name..)
    // returns true if allowed , false if denied(and reason via out string).. 
    bool allow(const string &action_name, const nlohmann :: json &context, string &reason) ;

    // get policy json (for introspection).. 
    optional<nlohmann :: json> policy() const; 

    private: 
    nlohmann :: json policy_json_; 
    }; 
}

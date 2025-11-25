#include "policy_enforcer.h"

#include <fstream> 
#include <iostream> 

using namespace std; 

namespace dip :: security
{
    bool PolicyEnforcer :: load_policy(const string &policy_path)
    {
        ifstream f(policy_path); 
        if (!f.is_open()) return false; 
        try
        {
            {/* code */}
            f >> policy_json_; 
            return true ; 
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return false; 
        }
        
    }

    optional <nlohmann :: json> PolicyEnforcer :: policy() const 
    {
        if (policy_json_.is_null()) return nullopt; 
        return policy_json_; 
    }

    bool PolicyEnforcer :: allow(const string &action_name, const nlohmann :: json &context, string &reason )
    {
        if (policy_json_.is_null()){
            reason = "no policy loaded"; 
            return false; 
        }

        // basic rule : check policy_rules away for matching triggers.. ; instruct action.. 
        // For demo : if action == "plugin road" and manifest permission contains any forbidden permission...
        if (action_name == "Plugin load"){
            if (!context.contains("manifest")) 
            {
                reason = "no manifest in context"; 
                return false; 
            }
            auto manifest = context["manifest"]; 
            if (manifest.contains("permission")){
                for (auto &p : manifest["permission"]){
                    string perm = p.get<string>(); 
                    // check default deny list in policy.. 
                    if (policy_json_.contains("defaults") and policy_json_["defaults"].contains("Forbidden")){
                        for (auto &fp : policy_json_["default"]["Forbidden permission"])
                        {
                            if (perm == fp.get<string>()){
                                reason = "permission forbidden" + perm; 
                                return false; 
                            }
                        }
                    }
                }
            }
        }
        reason = "allowed by default"; 
        return true;
    }
    // default allow if no rule allowed.. 
 
}
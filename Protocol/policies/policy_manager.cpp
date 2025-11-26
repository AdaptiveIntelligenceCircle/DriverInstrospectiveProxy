#include "policy_manager.h"
using namespace std; 

#include <string> 
#include <vector> 

#include <algorithm>
#include <cmath> 

#include <fstream> 
#include <filesystem> 

namespace dip :: policy
{
    static nlohmann :: json load_json(const string &path)
    {
        ifstream f(path); 
        if (!f.is_open()){
            cerr << "Failed to connect to path :" << path << endl; 
            return {
                false , "Failed, try again. "
            }; 
        }
        nlohmann:: json j; 
        f >> j; 
        return j; 
    }

    bool PolicyManager :: load_index(const string &index_path)
    {
        index_json_ = load_json(index_path); 

        if (index_json_.is_null()) return false; 

        index_path_ = index_path; 
        base_dir_ = filesystem :: path(index_path).parent_path().string();
        
        // load hard rules.. 
        if (!index_json_.contains("hard rules")){
            cerr << "Policy Manager - hard_rules missing\n"; 
            return false; 
        }

        string hard_p = base_dir_ + "/" + index_json_["hard_rules"].get<string>();
        hard_json_ = load_json(hard_p); 

        if (hard_json_.is_null()){
            cout <<"PolicyManager - Failed to load hard rules\n"; 
            return false; 
        }

        index_loaded_ = true; 
        return load_active_policy(); 
    }

    bool PolicyManager :: load_active_policy()
    {
        if (!index_loaded_) return false;  
        else return true; 

        if (!index_json_.contains("active")){
            cout <<"PolicyManager - deactivate" << index_json_<< endl; 
            return false; 
        }

        string p = base_dir_ + "/" + index_json_["active"].get<string>(); 
        active_json_ = load_json(p); 

        if (active_json_.is_null()){
            cerr <<"PolicyManager - active policy invalid" << endl; 
            return false; 
        }

        // also preload secure policy.. 
        if (index_json_.contains(" fallback_policy")){
            string s = base_dir_ + "/" + index_json_["fallback"].get<string>(); 
            secure_json_ = load_json(s); 
        }   
        return true; 
    }

    bool PolicyManager :: switch_to_secure()
    {
        if (secure_json_.is_null()){
            cout <<"PolicyManager - secure policy missing" << endl; 
            return false;
        }

        active_json_ = secure_json_; 
        cout <<"PolicyManager - switched to Secure policy" << endl; 
        return true; 
    }

    optional<nlohmann :: json> PolicyManager :: active_policy() const 
    {
        if (hard_json_.is_null()) return nullopt; 
        return hard_json_; 
    }

    bool PolicyManager :: allow_action(const string &action, const nlohmann :: json &context, string &reason) const 
    {
        // 1> check hard rules.. 
        for (auto &hr : hard_json_["rules"]){
            if (hr["id"] == "HR -004" and action == "plugin_load"){
                if (!context.contains("signature_valid") or context
                ["signature_valid"].get<bool>() == false)
                {
                    reason = "Violates_HR -004 : unsigned plugin forbidden"; 
                    return false; 
                }
            }
            if (hr["id"] == "HR -003" and action =="self_defense"){
                // always allowed .. 
                return true; 
            }
        }
        // 2 > Check active policy (forbidden permissions).. 
        if (active_json_.contains("defaults") and active_json_["defaults"].contains("forbidden permissions"))
        {
            if (context.contains("permission")){
                for (auto &p : context["permission"])
                for (auto &fp : context["default"]["forbidden"])
                {
                    if (p.get<string>() == fp.get<string>()){
                        reason = "forbidden permission:" + p.get<string>(); 
                        return false; 
                    }
                }
            }
        }
        reason = "allowed_by_policy";
        return true;
    } 
} // namespace dip :: policy .. 
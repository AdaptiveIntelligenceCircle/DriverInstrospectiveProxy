#pragma once 
#include <iostream> 

#include <string> 
#include <vector> 

#include <nlohmann/json.hpp>
using namespace std; 

namespace dip :: policy
{
    class PolicyManager
    {
        public: 
        ~PolicyManager() = default;
        PolicyManager() = default; 

        // load policy_index.json.. 
        bool load_index(const string &index_path); 

        // load selected policy Json.. 
        bool load_active_policy(); 

        // Switch to secure policy immediately.. 
        bool switch_to_secure(); 

        // Get active policy(run time).. 
        optional<nlohmann :: json> active_json() const; 

        // Hard rules are always loaded... 
        optional<nlohmann :: json> hard_rules() const; 
        
        optional<nlohmann :: json> active_policy() const ; 

        // Validate if an action is allowed via policy + hard rules.. 
        bool allow_action(const string &action, const nlohmann :: json &context, string &reason) const; 

        private: 
        nlohmann :: json index_json_; 
        nlohmann :: json active_json_; 
        nlohmann :: json secure_json_; 

        nlohmann :: json hard_json_; 
        bool index_loaded_ = false; 

        string index_path_; 
        string base_dir_; // deduce from index path.. 
        
    }; 
} // namespace dip :: policy.. // 
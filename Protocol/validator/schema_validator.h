#pragma once 
#include <string> 

#include <optional> 
#include <nlohmann/json.hpp>

using namespace std; 

namespace dip :: security
{
    struct ValidateResult
    {
        bool ok; 
        string message; 
    }; 

    class SchemaValidator
    {
        public: 
        SchemaValidator() = default; 
        ~SchemaValidator() = default; 

        // load schema from path (json schema).. 
        bool load_schema(const string &schema_path); 

        // Validate a Json document.. (string or json) against loaded schema.. 
        ValidateResult validate(const nlohmann :: json &doc) const; 

        // convenience..// 
        ValidateResult validate_file(const string &json_path) const; 

        private: 
        nlohmann :: json schema_; 
        bool schema_loaded_ = false; 
    }; 
}
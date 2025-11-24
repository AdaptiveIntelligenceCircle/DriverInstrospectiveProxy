#pragma once 
#include <string>

#include <optional> 
#include <nlohmann/json.hpp>

using namespace std; 

namespace driver_ai :: metadata
{
    struct ModelMetaData
    {
        string name; 
        string version; 
        string type; 

        string checksum; 
        string weights_path; 

        int input_dim = 0; 
        int output_dim = 0; 
    };
    
    class MetadataLoader
    {
        public: 
        static optional<ModelMetaData> load(const string &path); 
    }; 
}
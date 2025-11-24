#pragma once 
#include <iostream> 
#include <string> 

#include <vector> 

using namespace std;

namespace driver_ai :: models
{
    enum class ModelType
    {
        RISK, 
        BEHAVIOR, 
        TEMPORAL, 
        FUSION, 
        ANOMALY
    };
    
    struct LoadedModel
    {   
        ModelType type; 
        string version; 
        vector<float> weights; 
        bool ok = false; 

    };
    
    class ModelLoader
    {
        public: 
        bool load_from_file(const string &path); 
        optional<LoadedModel> get(ModelType t); 

        private: 
        vector<LoadedModel> models_; 
    }; 

    ModelLoader *global_loader(); 
}

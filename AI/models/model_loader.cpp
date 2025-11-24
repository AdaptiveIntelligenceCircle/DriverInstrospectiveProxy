#include "model_loader.h"

#include <fstream> 
#include <iostream> 

#include <nlohmann/json.hpp>
using namespace std; 

namespace driver_ai :: models
{
    static ModelLoader *g_loader = nullptr; 
    ModelLoader *global_loader()
    {
        if (!g_loader)
        g_loader = new ModelLoader(); 
        return g_loader; 
    }

    bool ModelLoader :: load_from_file(const string &path)
    {
        ifstream f (path); 
        if (!f.is_open()){
            cout <<"Model Loader Cannot open" << path << endl; 
            return false; 
        }

        nlohmann :: json j; 
        f >> j ; 

        LoadedModel m; 
        m.version = j["version"]; 
        m.ok = true; 

        string type = j["type"]; 
        if (type == "risk")
        m.type = ModelType :: RISK; 
        else if (type == "behavior") m.type = ModelType :: BEHAVIOR; 
        else if (type == "anomaly") m.type = ModelType :: ANOMALY; 
        else if(type == "Temporal") m.type = ModelType :: TEMPORAL; 
        else m.type == ModelType :: FUSION; 

        for (auto &x : j["weights"])
        m.weights.push_back(x); 

        models_.push_back(m); 
        return true; 
    }

    optional<LoadedModel> ModelLoader :: get(ModelType t)
    {
        for (auto &m : models_)
        {
            if (m.type == t) return m; 
        }
        return nullopt; 
    }
}
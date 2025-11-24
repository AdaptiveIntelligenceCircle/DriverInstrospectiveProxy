#include "metadata_loader.h"
using namespace std; 
#include <fstream> 

#include <iostream>
#include <string> 

namespace driver_ai :: metadata
{
    optional <ModelMetaData> MetadataLoader :: load(const string &path)
    {
        ifstream f(path); 
        if (!f.is_open()){
            cout <<"Cannot open path: " << path << endl;
            return nullopt; 
        }

        nlohmann :: json j; 
        f >> j; 

        ModelMetaData m; 
        m.name = j["name"]; 
        m.version = j["version"]; 
        m.type = j["model_type"]; 
        m.checksum = j["checksum"]; 

        m.weights_path = j["weights_path"]; 
        m.input_dim = j.value("input_dim", 0); 
        m.output_dim = j.value("output_dim",  0); 

        return m; 
    }
}
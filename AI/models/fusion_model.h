#pragma once 
#include <vector> 

using namespace std; 
#include <iostream>

namespace driver_ai :: models
{
    class FusionModel
    {
        public: 
        vector<double> fuse(
            const vector<double> &risk_vec, 
            const vector<double> &behavior_vec, 
            const vector<double>& temporal_vec
        ); 
    }; 
}
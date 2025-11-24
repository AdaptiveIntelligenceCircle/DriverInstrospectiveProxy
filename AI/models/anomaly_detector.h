#pragma once 
#include <vector> 

#include <iostream> 
using namespace std; 

namespace driver_ai :: models
{
    class AnomalyDetector
    {
        public: 
        bool is_anomaly(const vector<double> &x); 
    }; 
}
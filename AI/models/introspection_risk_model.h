#pragma once 
#include <iostream>
using namespace std;
#include <vector> 

namespace driver_ai :: models
{
    class IntrospectiveModel
    {
        public: 
        double compute_risk(const vector<double> &features); 
    }; 
}
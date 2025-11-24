#pragma once 
#include <iostream> 

#include <string> 
#include <vector> 
using namespace std ; 

namespace driver_ai :: models{
    enum class DriverStyle
    {
        CALM, NORMAL, AGGRESSIVE, DISTRACTED
    }; 

    class DriverBehaviorModel
    {
        public: 
        DriverStyle infer(const vector<double> &signals); 
    }; 
}
#pragma once 
#include <vector> 

#include <iostream>
using namespace std; 

class TemporalPredictor
{
    public: 
    double predict(const vector<double> &seq); 
}; 
#include "driver_behavior_model.h"
using namespace std; 

#include "model_loader.h"
#include <iostream> 
#include <cmath> 

namespace driver_ai :: models
{
    DriverStyle DriverBehaviorModel :: infer(const vector<double> &s)
    {
        auto loader = global_loader();
        auto model = loader -> get(ModelType :: BEHAVIOR); 
        if (!model.has_value()) 
        return DriverStyle ::NORMAL; 
        
        double score = 0; 
        for (size_t i = 0 ; i<s.size() and i < model -> weights.size(); i++)
        score += s[i] * model -> weights[i]; 

        if (score < -0.5) return DriverStyle :: CALM; 
        if (score < 0.5) return DriverStyle :: NORMAL; 
        if (score < 1.5) return DriverStyle :: AGGRESSIVE; 
        return DriverStyle :: DISTRACTED; 
    }
}
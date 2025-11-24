#include "temporal_predictor.h"
#include "model_loader.h"

using namespace std; 
#include <iostream>

namespace driver_ai :: models
{
    double TemporalPredictor :: predict(const vector<double> &seq)
    {
        auto model = global_loader() -> get(ModelType :: TEMPORAL);
        if (!model.has_value()) return seq.back() ; 

        double w = model -> weights[0];
        return seq.back() * w;  
    }
}
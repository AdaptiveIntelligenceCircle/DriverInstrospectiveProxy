#include "anomaly_detector.h"
using namespace std; 

#include "model_loader.h"
#include <cmath> 

namespace driver_ai :: models
{
    bool AnomalyDetector :: is_anomaly(const vector<double> &x)
    {
        auto model = global_loader() -> get(ModelType :: ANOMALY); 
        if (!model.has_value()) return false; 

        double m = 0; 
        for (size_t i = 0 ; i < x.size() and i < model -> weights.size(); i++)
        m+= abs(x[i] - model-> weights[i]); 
        
        return m > 1.0; 
    }
}
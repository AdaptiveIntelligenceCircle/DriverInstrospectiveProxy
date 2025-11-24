#include "introspection_risk_model.h"

#include "model_loader.h"
#include <cmath>

namespace driver_ai :: models
{
    double IntrospectiveModel :: compute_risk(const vector<double> &features)
    {
        auto loader = global_loader(); 
        auto model = loader -> get(ModelType :: RISK); 
        if (!model.has_value()) return 0.5; 

        // simple weighted sum.. 
        double s = 0; 
        for (size_t i = 0 ; i< features.size() and i < model -> weights.size(); i++)
        
            s+= features[i] *model -> weights[i]; 
        return 1.0/(1.0 + exp(-s)); // signoid.. 
    }
}
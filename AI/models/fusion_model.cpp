#include "fusion_model.h"
#include "model_loader.h"

using namespace std;

namespace driver_ai :: models
{
    vector<double> FusionModel :: fuse(
        const vector<double>& r,
        const vector<double>& b,
        const vector<double>& t
    )
    {
        auto m = global_loader() -> get(ModelType :: FUSION); 
        vector<double> out; 
        if (!m.has_value()) return out; 

        for (size_t i = 0 ; i < m -> weights.size() and i < r.size() ; i++)
        out.push_back(r[i] *m -> weights[i]); 
        return out; 
    }

}
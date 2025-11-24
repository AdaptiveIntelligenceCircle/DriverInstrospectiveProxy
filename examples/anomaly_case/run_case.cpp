#include <iostream> 
#include <fstream> 

#include <vector> 
#include <json.hpp>

using namespace std; 

#include "../../AI/models/anomaly_detector.h"
#include "../../AI/models/introspection_risk_model.h"
#include "../../AI/models/model_loader.h"

#include "../../models/metadata/metadata_loader.h"
#include "../../utils/packet_parser.h"

using namespace driver_ai :: models; 
using namespace driver_ai :: metadata; 

static nlohmann :: json load_json(const string &path)
{
    ifstream f(path); 
    nlohmann :: json j ; 
    f >> j ;  
    return j; 
}

int main()
{
    cout <<"DIP - Running anomaly case example.." << endl; 

    auto seq = load_json("anomaly sequence.json"); 

    // 1. Load required models..
    auto loader = global_loader(); 
    loader -> load_from_file("");
    loader -> load_from_file(""); 

    AnomalyDetector anomaly; 
    IntrospectiveModel risk_model; 

    vector<double> vec(3); 

    for (auto &frame : seq["sequence"])
    {
        vec[0] = frame["wheel_speed"]; 
        vec[1] = frame["lidar"]; 
        vec[2] = frame["imu"]; 

        bool is_bad = anomaly.is_anomaly(vec); 

        if (is_bad)
        {
            cout <<"Anomaly detected" << endl; 

            // compute risk score.. 
            double risk = risk_model.compute_risk(vec); 

            cout << "Risk :" << risk << endl; 

            if (risk > 0.7)
            {
                // guard.trigger_rollback("sensor_corruption"); 
                // log.write_event("sensor_corruption", risk, vec); 
            }else 
            {
                cout <<"OK - normal frame" << endl; 
            }
        }
        cout << "DIP - done" << endl; 
    }
}

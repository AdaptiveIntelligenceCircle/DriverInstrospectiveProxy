#include "intrusion_detector.h"
using namespace std; 

#include <iostream> 
#include <vector> 

namespace dip :: security
{
    void IntrusionDetector :: observe_event(const string &type, 
    const nlohmann :: json &context)
    {
        if (type == "comm_anomaly" ) 
        comm_violations_ ++ ; 
        if (type == "invalid signature")
        signature_failures_ ++; 
        if (type == "packet_poisoning")
        anomaly_count_ ++; 

        cout <<"Intrusion Detector - event :" << type << endl; 
    }

    double IntrusionDetector :: risk_score() const
    {
        double score = 0.0; 

        score += anomaly_count_ *0.2; 
        score += comm_violations_ * 0.1; 
        score += signature_failures_ * 0.3; 

        if (score > 1.0) score = 1.0; 
        return score; 
    }

    void IntrusionDetector :: reset()
    {
        anomaly_count_ = 0; 
        comm_violations_ = 0 ; 
        signature_failures_ = 0; 
    }
} // namespace dip :: security.. 

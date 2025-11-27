#pragma once 
#include <iostream>

#include <string> 
#include <vector> 

using namespace std; 
#include <nlohmann/json.hpp>

namespace dip :: security
{
    class IntrusionDetector
    {
        public: 
        IntrusionDetector() = default; 
        ~IntrusionDetector() = default; 

        // simple scoring each anomaly increases risk.. 
        void observe_event(const string &type, const nlohmann :: json &context); 

        // Risk score range [0..1]..
        double risk_score() const; 

        // Reset internal state.. 
        void reset(); 

        private: 
        int anomaly_count_ = 0; 
        int comm_violations_ = 0; 
        int signature_failures_ = 0 ; 
    };
} // namespace dip :: security.. s
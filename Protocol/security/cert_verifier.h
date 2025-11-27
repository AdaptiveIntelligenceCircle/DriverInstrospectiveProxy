#pragma once 
#include <iostream>

#include <string> 
#include <cmath> 

using namespace std; 
#include <nlohmann/json.hpp>

#include <optional> 
#include <vector> 

class CertVerifier
{
    public: 
    CertVerifier() = default; 
    ~CertVerifier() = default;
    
    // local ceritificate metadata Json.. 
    bool load(const string &cert_path); 

    // Validate fields (issuer, data_window_).. 
    bool validate(string &reason) const; 

    // Access certificate fields... 
    optional<nlohmann:: json> cert_json() const; 

    private: 
    nlohmann :: json cert_; 
}; 
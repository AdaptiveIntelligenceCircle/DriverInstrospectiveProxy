#include "cert_verifier.h"
using namespace std; 

#include <iostream> 
#include <string> 

#include <cmath> 
#include <vector> 

#include <fstream>

namespace dip :: security
{
    bool CertVerifier :: load(const string &cert_path)
    {
        ifstream f(cert_path); 
        if (!f.is_open()){
            cout << "error" << endl; 
            return false; 
        }
        try 
        {
            f >> cert_; 
            return true; 
        } catch(...)
        {
            return false; 
        }
    }

    bool CertVerifier :: validate(string &reason) const 
    {
        if (cert_.is_null()){
            reason = "cert_invalid"; 
            return false ; 
        }
        if (!cert_.contains("subject")
        or !cert_.contains("issuer") or !cert_.contains("fingerprint") or !cert_.contains("") ){
            reason = "missing_required_fields" ; 
            return false; 
        }

        // optional : verify date ranges.. 
        reason  = "ok"; 
        return true; 
    }

    optional <nlohmann :: json > CertVerifier :: cert_json() const 
    {
        if (cert_.is_null()) return nullopt; 
        return cert_; 
    }
}

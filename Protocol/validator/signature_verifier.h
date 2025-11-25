#pragma once 
#include <string> 
using namespace std; 

namespace dip :: security
{
    class SignatureVerifier
    {
        public: 
        SignatureVerifier() = default; 
        ~SignatureVerifier() = default; 

        // verify signature in manifest : provide public key (PEM) or certificate path.. 
        bool verify_manifest_signature(const string &manifest_json, const string &pubkey_pem, const string &sig_hex) ;
    }; 
} // namespace dip :: security.. 
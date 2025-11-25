#pragma once 

#include <iostream>
using namespace std; 

#include <string> 
#include <vector> 

namespace dip :: security
{
    string sha_256_hex(const string &data);
    string file_sha256_hex(const string &path); 

    // HMAC - SHA256 (key, data) -> hex saying.. 
    string hmac_sha256_hex(const string &key, const string &data); 

    // signature verify : placeholder, if openSSL available , implement actual RSA/ECDSA verify.. 
    bool verify_signature_pem(const string &pubkey_pem, const string &data, const string &sig_hex); 
} // namespace dip :: security;; 
#pragma once 
#include <string> 

#include <iostream> 
#include <vector> 

#include <algorithm> 
#include <optional> 

using namespace std; 

class CryptoKeyChallenge
{
    public:
    
    CryptoKeyChallenge() = default; 
    ~CryptoKeyChallenge() = default; 

    // Generate session key (symmetric..)... 
    string generate_session_key(); 

    // Diffe - Hellmann stub (no OpenSSl) - return fake public key.. 
    string dh_generate_public(); 

    // Compute shared secret(placeholder...)
    string dh_compute_secret(const string &other_pub); 

    private: 
    string session_key_; 
}; 
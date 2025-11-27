#include "cryto_key_exchange.h"
using namespace std; 

#include <iostream>
#include <string> 

#include <cmath> 
#include <random>

namespace dip :: security
{
    static string random_hex(size_t n)
    {
        static const char *hex = "0123456789abcdef"; 
        string s; 
        s.reserve(n);
        
        random_device rd; 
        for (size_t i = 0 ; i < n ; i++)
        s.push_back(hex[(rd() % 16)]);
        
        return s; 
    }

    string CryptoKeyChallenge :: generate_session_key(){
        session_key_ = random_hex(64); 
        return session_key_; 
    }

    string CryptoKeyChallenge :: dh_generate_public()
    {
        return random_hex(64); // placeholder..
    }

    string CryptoKeyChallenge :: dh_compute_secret(const string &other_pub)
    {
        return random_hex(64); // 
    }
}


#ifndef DIP_PROTOCOL_P2P_HANDSHAKE_H
#define DIP_PROTOCOL_P2P_HANDSHAKE_H

#pragma once 
#include <iostream>
#include <string>

#include <vector> 
#include <cmath> 
#include <algorithm>

using namespace std; 

namespace dip :: protocol
{
    struct HandshakeResult
    {
        bool success; 
        string session_key; 
        string reason; 
    }; 

    class P2PHandshake
    {
        public: 
        P2PHandshake() = default; 
        ~P2PHandshake() = default; 
        
        HandshakeResult intiate(const string &local_id, 
        const string &remote_id, const string &nonce);

        bool validateResponse(const string &response_sig); 

        private: 
        string last_nonce_; 
        string last_remote_; 
    }; 
} // namespace dip :: protocol..

#endif 
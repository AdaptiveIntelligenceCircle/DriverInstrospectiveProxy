#include "p2p_handshake.h"
using namespace std; 

namespace dip :: protocol
{
    HandshakeResult P2PHandshake :: intiate(const string &local_id, 
    const string &remote_id, const string &nonce)
    {
        last_nonce_ = nonce; 
        last_remote_ = remote_id; 

        // Fake key derivation for now.. 
        string session_key = "SESSION_" + local_id + "_" + remote_id; 

        return {true, session_key, ""}; 
    }
    
    bool P2PHandshake :: validateResponse(const string &response_sig)
    {
        return !response_sig.empty(); 
    }
} // namespace dip :: protocol.

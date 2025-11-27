#ifndef DIP_PROTOCOL_FEDERATION_MANAGER_H 
#define DIP_PROTOCOL_FEDERATION_MANAGER_H 

#include <iostream>
using namespace std; 

#include <string> 
#include <vector> 

#include <unordered_map>
#include <functional>

namespace dip :: protocol
{
    struct FederationNode
    {
        string id; 
        string address; 
        bool trusted; 
        int latency_ms = 0; 
    }; 

    class FederationManager
    {
        public: 
        using NodeList = vector<FederationNode>; 
        using Callback= function<void(const FederationNode&)>; 

        FederationManager(); 

        bool join(const FederationNode &node); 
        bool leave(const string &id); 
        bool updateNode(const FederationNode &node); 

        NodeList getNodes() const; 
        bool isTrusted(const string &id) const; 

        void onNodeJoin(Callback cb); 
        void onNodeLeave(Callback cb); 
        
        private: 
        unordered_map<string, FederationNode> nodes_; 
        Callback join_cb_; 
        Callback leave_cb_; 
    }; 
} // namespace dip :: protocol

#endif 
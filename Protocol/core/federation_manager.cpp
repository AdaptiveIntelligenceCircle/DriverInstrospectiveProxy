#include "federation_manager.h"
using namespace std; 

#include <iostream> 
#include <vector> 

namespace dip :: protocol
{
    FederationManager :: FederationManager(){}

    bool FederationManager :: join(const FederationNode &node){
        if (nodes_.count(node.id)) return false; 
        nodes_[node.id] = node; 

        if (join_cb_) 
        join_cb_(node); 
        return true; 
    }

    bool FederationManager :: leave(const string &id)
    {
        if (!nodes_.count(id)) return false; 
        auto node = nodes_[id]; 

        nodes_.erase(id); 

        if (leave_cb_) leave_cb_(node); 
        return true; 
    }

    bool FederationManager :: updateNode(const FederationNode &node)
    {
        if (!nodes_.count(node.id)) return false; 
        nodes_[node.id] = node; 
        return true ; 
    }

    FederationManager :: NodeList FederationManager :: getNodes() const 
    {
        NodeList out; 
        for (const auto &kv : nodes_) 
        out.push_back(kv.second); 
        return out; 
    }

    bool FederationManager :: isTrusted(const string &id) const{
        auto it = nodes_.find(id); 
        return (it != nodes_.end() and it -> second.trusted); 
    }

    void FederationManager :: onNodeJoin(Callback cb){
        join_cb_ = cb; 
    }

    void FederationManager :: onNodeLeave(Callback cb)
    {
        leave_cb_ = cb; 
    }
} // namespace dip:: protocol.. 

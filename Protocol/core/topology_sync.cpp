#include "topology_sync.h"
using namespace std; 

namespace dip :: protocol
{
    bool TopologySync :: update(const vector<TopologyNode> &nodes)
    {
        for (const auto &n : nodes)
        map_[n.id] = n; 
        return true; 
    }

    vector<TopologyNode> TopologySync :: dump() const{
        vector<TopologyNode> out; 
        for (const auto &kv : map_) out.push_back(kv.second); 
        return out; 
    }

    bool TopologySync :: hasNode(const string &id) const {
        return map_.count(id) != 0; 
    }
} // namespace dip :: protocol.. 

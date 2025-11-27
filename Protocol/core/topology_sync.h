#ifndef DIP_PROTOCOL_TOPOLOGY_SYNC_H
#define DIP_PROTOCOL_TOPOLOGY_SYNC_H 

#include <string>
#include <vector>
#include <unordered_map>

using namespace std; 

namespace dip :: protocol
{
    struct TopologyNode
    {
        string id; 
        string endpoint; 
    }; 

    class TopologySync
    {
        public:
        TopologySync() = default; 
        ~TopologySync() = default; 

        bool update(const vector<TopologyNode> &nodes); 
        vector<TopologyNode> dump() const; 

        bool hasNode(const string &id) const; 

        private: 
        unordered_map<string, TopologyNode> map_; 
    }; 
} // namespace dip :: protocol.. 

#endif 
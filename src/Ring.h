#include "ConsistentHash.h"
#include <map>
#include <cstdint>
#include <stack>

#ifndef RING_H
#define RING_H
using namespace std;

class Ring :public CH
{
private:
    std::map<uint32_t, uint32_t> workSet;
    uint32_t V;
    std::stack<uint32_t> failedSet;
    void insert(uint32_t nodeID);
    const char *name = "MaglevHash";

public:
    Ring(uint32_t, uint32_t);
    ~Ring() {};
public:
    uint32_t getSize();
    void updateRemoval(uint32_t index); 
    uint32_t updateAddition();
    uint32_t getNodeID(uint32_t K, uint32_t* numHash);
    const char* getName(){
        return name;
    }
};


#endif
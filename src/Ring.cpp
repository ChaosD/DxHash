#include "Ring.h"
#include <cstdio>
#include "crc32c_sse42_u64.h"

void Ring::insert(uint32_t nodeID){
    for(uint32_t i = 0; i < V; i++){
        uint32_t key2 = gen32bitRandNumber(i);
        uint32_t hashValue = crc32c_sse42_u64(nodeID, key2);
        workSet[hashValue] = nodeID;
    }
}

Ring::Ring(uint32_t w, uint32_t v): V(v), failedSet(), workSet(){
    for(uint32_t i = 0; i < w; i++){
        insert(i);
    }
}

void Ring::updateRemoval(uint32_t nodeID){
    for(uint32_t i = 0; i < V; i++){
        uint32_t key2 = gen32bitRandNumber(i);
        uint32_t hashValue = crc32c_sse42_u64(nodeID, key2);
        workSet.erase(hashValue);
    }
    failedSet.push(nodeID);
}

uint32_t Ring::updateAddition(){
    uint32_t nodeID = failedSet.top();
    failedSet.pop();
    insert(nodeID);
}

uint32_t Ring::getNodeID(uint32_t K, uint32_t* numHash){
    uint32_t key2 = gen32bitRandNumber(K);
    uint32_t Khash = crc32c_sse42_u64(K, key2);
    auto pNode = workSet.lower_bound(Khash);

    if(pNode == workSet.end()){
        pNode = workSet.begin();
    }

    return pNode->second;
}

uint32_t Ring::getSize(){
    return workSet.size() / V;
}


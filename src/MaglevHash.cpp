#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstdbool>
#include <cstring>
#include "crc32c_sse42_u64.h"
#include "MaglevHash.h"

MaglevHash::MaglevHash(uint32_t w, uint32_t t): M(t), workSet(), lookupTable(new uint32_t [t]()), 
  permutationOffset(nullptr), permutationSkip(nullptr){
    uint32_t i = 0; 
    for(i = 0; i < w; ++i){
        workSet.insert(i);
    }
    updatePermutation();
    updateLookupTable();
}

MaglevHash::~MaglevHash(){
    delete[] lookupTable;
    if(permutationOffset != nullptr){
        delete[] permutationOffset;
        permutationOffset = nullptr;
    }
    if(permutationSkip != nullptr){
        delete[] permutationSkip;
        permutationSkip = nullptr;
    }
}

void MaglevHash::updatePermutation(){
    uint32_t i, size;
    if(permutationOffset != nullptr){
        delete[] permutationOffset;
        permutationOffset = nullptr;
    }
    if(permutationSkip != nullptr){
        delete[] permutationSkip;
        permutationSkip = nullptr;
    }
    size = workSet.size();
    permutationOffset = new uint32_t [size]();
    permutationSkip = new uint32_t [size]();
    set<uint32_t>::iterator iter;
    for(iter = workSet.begin(), i = 0; iter != workSet.end(); iter++, i++){
        permutationOffset[i] = gen32bitRandNumber(*iter) % M;
        permutationSkip[i] = crc32c_sse42_u64(*iter, permutationOffset[i]) % (M - 1) + 1;
    }
}

void MaglevHash::updateLookupTable(){
    uint32_t i, j, size, pos, num;
    size = workSet.size();
    set<uint32_t>::iterator iter;
    uint32_t *next = new uint32_t [size]();
    for(i = 0; i < M; i++){
        lookupTable[i] = -1;
    }
    for(i = 0; i < size; i++){
        next[i] = 0;
    }
    while(1){
        for(i = 0, iter = workSet.begin(); iter != workSet.end(); iter++, i++){
            for (j = next[i]; j < M; j++){
                pos = (permutationOffset[i] + j * permutationSkip[i]) % M;
                next[i]++;
                if(lookupTable[pos] == -1) {
                    lookupTable[pos] = *iter;
                    num++;
                    if(num == M){
                        delete[] next;
                        return;
                    }
                    break;
                }
            }
        }
    }
}

uint32_t MaglevHash::getSize(){
    return workSet.size();
}

uint32_t MaglevHash::getNodeID(uint32_t key, uint32_t* ASL){
    *ASL = 1;
    uint32_t key2 = gen32bitRandNumber(key);
    uint32_t bs = crc32c_sse42_u64(key, key2) % M;
    return lookupTable[bs];
}

void MaglevHash::updateRemoval(uint32_t key){
    workSet.erase(key);
    failedSet.push(key);
    updatePermutation();
    updateLookupTable();
    // printf("off\tskip\n");
    // for(uint32_t i = 0; i < getSize(); i++){
    //     printf("%u\t%u\n", permutationOffset[i], permutationSkip[i]);
    // }
    // for(uint32_t i = 0; i < M; i++){
    //     printf("%u ", lookupTable[i]);
    // }
    // printf("\n");

}

uint32_t MaglevHash::updateAddition(){
    uint32_t key = failedSet.top();
    failedSet.pop();
    workSet.insert(key);
    updatePermutation();
    updateLookupTable();
    return key;
}
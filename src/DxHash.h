#include <vector>
#include <stack>
#include "ConsistentHash.h"

#ifndef DXHASH_H
#define DXHASH_H

#define MAX_POWER 32 // since the int type we use is uint32, the max power should no large then 32.
using namespace std;


class DxHash:public CH{
private:
	// The node array contains every nodes info.
	uint8_t* nodes;
	uint32_t size;
	// the offlineNodes is the priority queue of failed node IDs.
	std::stack<uint32_t> offlineNodes;
	void doubleScale();			// double the scale of the cluster.

public:
	DxHash(uint32_t);
	~DxHash();
	// rand function

public:
	uint32_t getSize(); 		// get the number of total nodes
	uint32_t getOfflineSize(); 	// get the number of failed nodes
	// NodeInfo& getNodebyIndex(uint32_t index); 	// to get the nodeinfo by nodeID.
	void updateRemoval(uint32_t index);			// update the removal of the current node.
	uint32_t updateAddition();	// update the addition of a node.

public:
	// compute NodeID by the key, passingly return the number of search length for analysis.
	uint32_t getNodeID(uint32_t K, uint32_t* numHash);	
};

#endif
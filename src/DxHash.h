#include <vector>
#include <stack>
// #include <boost/dynamic_bitset.hpp>
#include "ConsistentHash.h"

#ifndef DXHASH_H
#define DXHASH_H

using namespace std;


class DxHash:public CH{
private:
	// The node array contains every nodes info.
	// boost::dynamic_bitset<> nodes;
	vector<bool> nodes;
	// uint8_t* nodes;

	uint32_t size;
	uint32_t num_working;
	// the offlineNodes is the priority queue of failed node IDs.
	void resize();			// adjustment scale of the cluster.
	uint32_t getInactiveNodeID(uint32_t key);
	const char *name = "DxHash";	

public:
	DxHash(uint32_t);
	DxHash(uint32_t, uint32_t);

	virtual ~DxHash(){};
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
	const char* getName(){
		return name;
	}
};


class DxHash_TO1:public CH{
private:
	uint8_t* nodes;
	uint32_t size;
	uint32_t num_working;
	void resize();			// adjustment scale of the cluster.
	uint32_t getInactiveNodeID(uint32_t key);
	const char *name = "DxHash_TO1";	

public:
	DxHash_TO1(uint32_t);
	DxHash_TO1(uint32_t, uint32_t);
	virtual ~DxHash_TO1();

public:
	uint32_t getSize(); 		// get the number of total nodes
	uint32_t getOfflineSize(); 	// get the number of failed nodes
	void updateRemoval(uint32_t index);			// update the removal of the current node.
	uint32_t updateAddition();	// update the addition of a node.

public:
	uint32_t getNodeID(uint32_t K, uint32_t* numHash);	
	const char* getName(){
		return name;
	}
};

class DxHash_TO2:public CH{
private:
	uint8_t* nodes;
	uint32_t* IQueue;
	uint32_t IQueue_size;
	uint32_t size;
	uint32_t num_working;
	void resize();			// adjustment scale of the cluster.
	const char *name = "DxHash_TO2";	

public:
	DxHash_TO2(uint32_t);
	DxHash_TO2(uint32_t, uint32_t);
	virtual ~DxHash_TO2();

public:
	uint32_t getSize(); 		// get the number of total nodes
	uint32_t getOfflineSize(); 	// get the number of failed nodes
	void updateRemoval(uint32_t index);			// update the removal of the current node.
	uint32_t updateAddition();	// update the addition of a node.

public:
	uint32_t getNodeID(uint32_t K, uint32_t* numHash);	
	const char* getName(){
		return name;
	}
};

#endif
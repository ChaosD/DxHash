#include "ConsistentHash.h"
#include <set>
#include <stack>
#ifndef MAGLEVHASH_H
#define MAGLEVHASH_H
/** Class declaration */
using namespace std;

class MaglevHash: public CH {
  private:
	// Anchor		
	uint32_t *lookupTable;
	// Working
	uint32_t *permutationOffset;
	// Last appearance 
	uint32_t *permutationSkip;
	// Size of the table
	uint32_t M;
	// Working nodes
	std::set<uint32_t> workSet;
	std::stack<uint32_t> failedSet;
	// Translation oracle
	void updatePermutation();
	void updateLookupTable();
	const char *name = "MaglevHash";
  public:
	MaglevHash (uint32_t, uint32_t);
	uint32_t getSize();
	~MaglevHash();
	uint32_t getNodeID(uint32_t, uint32_t*);
	void updateRemoval(uint32_t);
	uint32_t updateAddition();
	const char* getName(){
		return name;
	}
};

#endif
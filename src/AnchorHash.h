#include <iostream>
#include <stack>
#include <stdint.h>
#include "ConsistentHash.h"
#ifndef ANCHORHASH_H
#define ANCHORHASH_H
/** Class declaration */

class AnchorHashQre: public CH {
  private:
	// Anchor		
	uint32_t *A;
	// Working
	uint32_t *W;
	// Last appearance 
	uint32_t *L;
	// "Map diagonal"	
	uint32_t *K;	
	// Size of the anchor
	uint32_t M;
	// Size of the working
	uint32_t N;
	// Removed buckets
	std::stack<uint32_t> r;   
	// Translation oracle
	uint32_t ComputeTranslation(uint32_t i , uint32_t j, uint32_t* n);
	const char* name = "AnchorHash";

  public:
	AnchorHashQre (uint32_t, uint32_t);
	uint32_t getOfflineSize();
	uint32_t getSize();
	~AnchorHashQre();
	uint32_t getNodeID(uint32_t, uint32_t*);
	void updateRemoval(uint32_t);
	uint32_t updateAddition();
	const char* getName(){
		return name;
	}
};

#endif
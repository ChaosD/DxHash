#include "AnchorHash.h"
#include "crc32c_sse42_u64.h"
using namespace std;
/** Constructor */
AnchorHashQre::AnchorHashQre (uint32_t a, uint32_t w) {
	// Allocate the anchor array	
	A = new uint32_t [a]();
	// Allocate the working array
	W = new uint32_t [a]();
	// Allocate the last apperance array
	L = new uint32_t [a]();	
	// Allocate the "map diagonal"	
	K = new uint32_t [a]();
	// Initialize "swap" arrays 
	for(uint32_t i = 0; i < a; ++i) {
		L[i] = i;
		W[i] = i;
		K[i] = i;
	}			
	// We treat initial removals as ordered removals
	for(uint32_t i = a - 1; i >= w; --i) {				
		A[i] = i;	
		r.push(i);			
	}	
	// Set initial set sizes
	M = a;
	N = w;	
}




/** Destructor */
AnchorHashQre::~AnchorHashQre () {
	delete [] A;		    
	delete [] W;	
	delete [] L;	
	delete [] K;	

}

uint32_t AnchorHashQre::getOfflineSize(){
	return r.size();
}

uint32_t AnchorHashQre::getSize(){
	return M;
}

uint32_t AnchorHashQre::ComputeTranslation(uint32_t i , uint32_t j, uint32_t* n) {
	if (i == j) return K[i];
	
	uint32_t b = j;
	
	while (A[i] <= A[b]) {
		b = K[b];
		*n += 1;
	}
	
	return b;

}

uint32_t AnchorHashQre::getNodeID(uint32_t key1, uint32_t* numHash) {
								
	// First hash is uniform on the anchor set
	uint32_t key2 = gen32bitRandNumber(key1);
	uint32_t bs = crc32c_sse42_u64(key1, key2);
	uint32_t b = bs % M;			
	// Loop until hitting a working bucket
	uint32_t c = 1, n;
	while (A[b] != 0) {	
		n = 1;
		// New candidate (bs - for better balance - avoid patterns)			
		bs = crc32c_sse42_u64(key1 - bs, key2 + bs);
		uint32_t h = bs % A[b];	
		//  h is working or observed by bucket
		if ((A[h] == 0) || (A[h] < A[b])) {
			b = h;
		}		
		// need translation for (bucket, h)
		else {
			b = ComputeTranslation(b,h,&n);
		}
		c+=n;							
	}
	*numHash = c;
	return b;								
}

void AnchorHashQre::updateRemoval(uint32_t b) {
	// update reserved stack
	r.push(b);			
	// update live set size
	N--;
	// who is the replacement
	W[L[b]] = W[N];
	L[W[N]] = L[b];
	// Update map diagonal
	K[b] = W[N];
	// Update removal
	A[b] = N;
}

uint32_t AnchorHashQre::updateAddition() {
	// Who was removed last?	
	uint32_t b = r.top();							
	r.pop();
	// Restore in observed_set
	L[W[N]] = N;	
	W[L[b]] = b;
	// update live set size
	N++;	
	// Ressurect
	A[b] = 0;
	// Restore in diagonal
	K[b] = b;
	return b;								
}
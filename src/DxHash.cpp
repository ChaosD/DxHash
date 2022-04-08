#include "DxHash.h"
#include "crc32c_sse42_u64.h"
#include <cstdio>
#include <cstdlib>


DxHash::DxHash(uint32_t num_working):size(1), offlineNodes(){
	uint32_t w_size = (num_working >> 1), i = 0;
	while(w_size){
		size <<= 1;
		w_size >>= 1;
	}
	if(num_working > size){
		size <<= 1;
	}
	nodes = new uint8_t[size]();

	for(i = 0; i < num_working; i++){
		nodes[i] = 1;
		// printf("%x %x  ", mask, nodes[i >> 3]);
	}
	for(i = num_working; i < size; i++){
		nodes[i] = 0;
		offlineNodes.push(i);
	}
	// printf("\n");
}

DxHash::~DxHash(){
	delete[] nodes;
}

uint32_t DxHash::getSize(){
	return size;
}

uint32_t DxHash::getOfflineSize(){
	return offlineNodes.size();
}

void DxHash::doubleScale(){
	uint8_t* nodes_tmp = new uint8_t[size << 1]();
	for(uint32_t i = 0; i < size; i++){
		nodes_tmp[i] = nodes[i];
	}
	for(uint32_t i = size; i < (size << 1); i++){
		offlineNodes.push(i);
		nodes_tmp[i] = 0;
	}
	size <<= 1;
	delete[] nodes;
	nodes = nodes_tmp;
}

// Uin& DxHash::getNodebyIndex(uint32_t index){
// 	return nodes[index];
// }

void DxHash::updateRemoval(uint32_t index){
	uint8_t& nodeID = nodes[index];
	if(nodeID){
		nodeID = 0;
		offlineNodes.push(index);
	}
	else{
		return;
	}
}

uint32_t DxHash::updateAddition(){
	if(offlineNodes.size() == 0)
		doubleScale();
	uint32_t index = offlineNodes.top();
	offlineNodes.pop();
	nodes[index] = 1;
	return index;
}

uint32_t DxHash::getNodeID(uint32_t key, uint32_t* numHash){
	uint32_t key2 = gen32bitRandNumber(key);	
	uint32_t r = crc32c_sse42_u64(key, key2);
	uint32_t index = bs % size;	
	uint32_t i = 1;
	while(!nodes[index]){
		r = crc32c_sse42_u64(r, key2);
		// weight = (float)gen32bitRandNumber(key) / 0xffffffff;
		index = r % size;
		i+=1;
		if (i >= 4 * size){
			return -1;
		}
		// printf("%lx\n", randNum);
		// printf("index: %x\n", index);
	};
	*numHash = i;
	return index;
}




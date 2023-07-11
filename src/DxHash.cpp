#include "DxHash.h"
#include "crc32c_sse42_u64.h"
#include <cstdio>
#include <cstdlib>


DxHash::DxHash(uint32_t n_working): num_working(n_working), size(1), nodes(){
	uint32_t w_size = (num_working >> 1), i = 0;
	while(w_size){
		size <<= 1;
		w_size >>= 1;
	}

	if(num_working > size){
		size <<= 1;
	}

	nodes.resize(size);
	// nodes = new uint8_t[size]();
	// nodes.setBitSize(size);

	for(i = 0; i < num_working; i++){
		nodes[i] = 1;
	}

	for(i = num_working; i < size; i++){
		nodes[i] = 0;
	}
}

DxHash::DxHash(uint32_t n_total, uint32_t n_working): size(n_total), num_working(n_working), nodes(){
	nodes.resize(size);

	uint32_t i;
	for(i = 0; i < num_working; i++){
		nodes[i] = 1;
	}

	for(i = num_working; i < size; i++){
		nodes[i] = 0;
	}
}


uint32_t DxHash::getSize(){
	return size;
}

uint32_t DxHash::getOfflineSize(){
	return size - num_working;
}


void DxHash::resize(){
	int new_size = 1;
	while (new_size <= num_working){
		new_size = new_size << 1;
	}

	if(new_size >= size) { // scale
		nodes.resize(new_size, 0);
	}

	int num_insertion = 0;
	if(new_size < size) { // shrink
		for(uint32_t i = new_size; i < size; i++){
			if(nodes[i] == 1){
				num_insertion++;
			}
		}
		nodes.resize(new_size, 0);
		for(uint32_t i = 0; i < new_size; i++){
			if(num_insertion > 0 && nodes[i] == 0){
				nodes[i] = 1;
				num_insertion--;
			}
		}
	}
	size = new_size;
}



void DxHash::updateRemoval(uint32_t index){
	auto nodeID = nodes[index];
	if(nodeID){
		nodes[index] = 0;
		num_working --;
	}
	else{
		return;
	}
}

uint32_t DxHash::updateAddition(){
	if(num_working == size)
		resize();
	uint32_t index = getInactiveNodeID(1228);
	nodes[index] = 1;
	num_working ++;
	return index;
}

uint32_t DxHash::getInactiveNodeID(uint32_t key){
	uint32_t key2 = gen32bitRandNumber(key);	
	uint32_t bs = crc32c_sse42_u64(key, key2);
	uint32_t index = bs % size;	
	uint32_t i = 1;
	while(nodes[index]){
		bs = crc32c_sse42_u64(bs, key2);
		index = bs % size;
		i += 1;
		if (i >= 4 * size){
			return -1;
		}
	};
	return index;
}

uint32_t DxHash::getNodeID(uint32_t key, uint32_t* numHash){
	uint32_t key2 = gen32bitRandNumber(key);	
	uint32_t bs = crc32c_sse42_u64(key, key2);
	uint32_t index = bs % size;	
	uint32_t i = 1;
	while(!nodes[index]){
		bs = crc32c_sse42_u64(bs, key2);
		index = bs % size;
		i += 1;
		if (i >= 4 * size){
			return -1;
		}
	};
	*numHash = i;
	return index;
}


DxHash_TO1::DxHash_TO1(uint32_t n_working): num_working(n_working), size(1), nodes(){
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
	}

	for(i = num_working; i < size; i++){
		nodes[i] = 0;
	}
}

DxHash_TO1::DxHash_TO1(uint32_t n_total, uint32_t n_working): size(n_total), num_working(n_working), nodes(){
	nodes = new uint8_t[size]();

	uint32_t i;
	for(i = 0; i < num_working; i++){
		nodes[i] = 1;
	}

	for(i = num_working; i < size; i++){
		nodes[i] = 0;
	}
}

DxHash_TO1::~DxHash_TO1(){
	delete[] nodes;
}

uint32_t DxHash_TO1::getSize(){
	return size;
}

uint32_t DxHash_TO1::getOfflineSize(){
	return size - num_working;
}

void DxHash_TO1::resize(){
	int new_size = 1;
	while (new_size <= num_working){
		new_size = new_size << 1;
	}

	uint8_t* nodes_tmp = new uint8_t[new_size]();

	if(new_size >= size) { // scale
		for(uint32_t i = 0; i < size; i++){
			nodes_tmp[i] = nodes[i];
		}
		for(uint32_t i = size; i < new_size; i++){
			nodes_tmp[i] = 0;
		}
	}
	int num_insertion = 0;
	if(new_size < size) { // shrink
		for(uint32_t i = new_size; i < size; i++){
			if(nodes[i] == 1){
				num_insertion++;
			}
		}	
		for(uint32_t i = 0; i < new_size; i++){
			nodes_tmp[i] = nodes[i];
			if(num_insertion > 0 && nodes_tmp[i] == 0){
				nodes_tmp[i] = 1;
				num_insertion--;
			}
		}
	}
	delete[] nodes;
	size = new_size;
	nodes = nodes_tmp;
}



void DxHash_TO1::updateRemoval(uint32_t index){
	if(num_working <= 0.1 * size)
		resize();
	auto nodeID = nodes[index];
	if(nodeID){
		nodes[index] = 0;
		num_working --;
	}
	else{
		return;
	}
}

uint32_t DxHash_TO1::updateAddition(){
	if(num_working == size)
		resize();
	uint32_t index = getInactiveNodeID(1228);
	nodes[index] = 1;
	num_working ++;
	return index;
}

uint32_t DxHash_TO1::getInactiveNodeID(uint32_t key){
	uint32_t key2 = gen32bitRandNumber(key);	
	uint32_t bs = crc32c_sse42_u64(key, key2);
	uint32_t index = bs % size;	
	uint32_t i = 1;
	while(nodes[index]){
		bs = crc32c_sse42_u64(bs, key2);
		index = bs % size;
		i += 1;
		if (i >= 4 * size){
			return -1;
		}
	};
	return index;
}

uint32_t DxHash_TO1::getNodeID(uint32_t key, uint32_t* numHash){
	uint32_t key2 = gen32bitRandNumber(key);
	uint32_t bs = crc32c_sse42_u64(key, key2);
	uint32_t index = bs % size;	
	uint32_t i = 1;
	while(!nodes[index]){
		bs = crc32c_sse42_u64(bs, key2);
		index = bs % size;
		// i += 1;
		if (i >= 4 * size){
			return -1;
		}
	};
	*numHash = i;
	return index;
}


DxHash_TO2::DxHash_TO2(uint32_t n_working): num_working(n_working), size(1), nodes(){
	uint32_t w_size = (num_working >> 1), i = 0;
	while(w_size){
		size <<= 1;
		w_size >>= 1;
	}

	if(num_working > size){
		size <<= 1;
	}

	nodes = new uint8_t[size]();
	IQueue = new uint32_t[size]();
	IQueue_size = 0;

	for(i = 0; i < num_working; i++){
		nodes[i] = 1;
	}

	for(i = num_working; i < size; i++){
		IQueue[IQueue_size++] = i;
		nodes[i] = 0;
	}
}

DxHash_TO2::DxHash_TO2(uint32_t n_total, uint32_t n_working): size(n_total), num_working(n_working), nodes(){
	nodes = new uint8_t[size]();
	IQueue = new uint32_t[size]();
	IQueue_size = 0;

	uint32_t i;
	for(i = 0; i < num_working; i++){
		nodes[i] = 1;
	}

	for(i = num_working; i < size; i++){
		IQueue[IQueue_size++] = i;
		nodes[i] = 0;
	}
}

DxHash_TO2::~DxHash_TO2(){
	delete[] nodes;
	delete[] IQueue;
}

uint32_t DxHash_TO2::getSize(){
	return size;
}

uint32_t DxHash_TO2::getOfflineSize(){
	return size - num_working;
}

void DxHash_TO2::resize(){
	int new_size = 1;
	while (new_size <= num_working){
		new_size = new_size << 1;
	}

	uint8_t* nodes_tmp = new uint8_t[new_size]();
	uint32_t* IQUEUE_tmp = new uint32_t[new_size]();
	IQueue_size = 0;

	if(new_size >= size) { // scale
		for(uint32_t i = 0; i < size; i++){
			if((nodes_tmp[i] = nodes[i]) == 0){
				IQUEUE_tmp[IQueue_size++] = i;
			}
		}
		for(uint32_t i = size; i < new_size; i++){
			nodes_tmp[i] = 0;
			IQUEUE_tmp[IQueue_size++] = i;
		}
	}

	int num_insertion = 0;
	if(new_size < size) { // shrink
		for(uint32_t i = new_size; i < size; i++){
			if(nodes[i] == 1){
				num_insertion++;
			}
		}	
		for(uint32_t i = 0; i < new_size; i++){
			nodes_tmp[i] = nodes[i];
			if(num_insertion > 0 && nodes_tmp[i] == 0){
				nodes_tmp[i] = 1;
				num_insertion--;
			}
			if(nodes_tmp[i] == 0){
				IQUEUE_tmp[IQueue_size++] = i;
			}
		}
	}
	delete[] nodes;
	delete IQueue;
	nodes = nodes_tmp;
	IQueue = IQUEUE_tmp;
}


void DxHash_TO2::updateRemoval(uint32_t index){
	auto nodeID = nodes[index];
	if(nodeID){
		nodes[index] = 0;
		IQueue[IQueue_size++] = nodeID;
		num_working --;
	}
	else{
		return;
	}
}

uint32_t DxHash_TO2::updateAddition(){
	if(IQueue_size == 0)
		resize();
	uint32_t index = IQueue[IQueue_size--];;
	nodes[index] = 1;
	return index;
}

uint32_t DxHash_TO2::getNodeID(uint32_t key, uint32_t* numHash){
	uint32_t key2 = gen32bitRandNumber(key);
	uint32_t bs = crc32c_sse42_u64(key, key2);
	uint32_t index = bs % size;	
	uint32_t i = 1;
	while(!nodes[index]){
		bs = crc32c_sse42_u64(bs, key2);
		index = bs % size;
		i += 1;
		if (i >= 4 * size){
			return -1;
		}
	};
	*numHash = i;
	return index;
}
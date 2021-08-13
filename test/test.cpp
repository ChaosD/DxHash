#include "DxHash.h"
#include "AnchorHash.h"
#include "MaglevHash.h"
#include "Ring.h"
#include "crc32c_sse42_u64.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;


void statBalance(uint32_t* statArray, uint32_t length, double mean){
		double lb = 0;
		uint32_t i, j;
		uint64_t w = 0;
		uint64_t mean1 = 0, mean2 = 0;
		for (i = 0, j = 0;  i < length; i++) {
			if (statArray[i]) {
				if(i < length / 2){
					mean1 += statArray[i];
				}	
				else{
					mean2 += statArray[i];
				}
				lb += (statArray[i] - mean) * (statArray[i] - mean);
				w += statArray[i];
				// printf("%u: %u\n", i, statArray[i]);
				j++;
			}
		}
		printf("mean: %lf, mean1: %lf, mean2: %lf, std dev: %lf, re std dev: %lf, total keys: %lu, valid node: %u \n", mean, (double)mean1*2/length, (double)mean2*2/length, sqrt(lb/j), sqrt(lb/j)/mean, w, j);	
}

int main(int argc, char const *argv[])
{
	uint32_t type = 0;
	uint32_t num_total = 1024, num_working = 1000, num_removals = 500, num_keys = 1000000;
	CH* consistentHash = nullptr;
	printf("Which CH to test? [1]MaglevHash [2]AnchorHash [3]DxHash [4]Ring\n");
	fflush(stdout);
	scanf("%u/n", &type);
	printf("Entere the total number, the working number, the removed number and the number of keys:\n");
	fflush(stdout);
	scanf("%u %u %u %u/n", &num_total, &num_working, &num_removals, &num_keys);
	if(type == 1){
		consistentHash = new MaglevHash(num_total, 97*num_total);
	}
	if(type == 2){
		consistentHash = new AnchorHashQre(num_total, num_working);
	}
	if(type == 3){
		consistentHash = new DxHash(num_working);
	}
	if(type == 4){
		consistentHash = new Ring(num_total, 100);
		printf("size: %u\n", consistentHash->getSize());
	}

	// random removals
	uint32_t* bucket_status = new uint32_t[num_total]();
	uint32_t* trace_DH = new uint32_t[num_keys]();
	// uint32_t* trace_AH = new uint32_t[num_keys]();

    uint32_t i = 0;
    // initial the hashes
    for (i = 0; i < num_working; i++) {
		bucket_status[i] = 1;
	}

	i = 0;
    while (i < num_removals) {
		uint32_t removed = gen32bitRandNumber(rand()) % num_working;
		if (bucket_status[removed] == 1) {
			consistentHash->updateRemoval(removed);
			// dh.updateRemoval(removed);
			bucket_status[removed] = 0;
			i++;
		}
	}

    while(true){
		uint32_t* stat_DH = new uint32_t [num_total]();
		// uint32_t* stat_AH = new uint32_t [num_total]();
		uint32_t numHash_DH = 0, sumOfNumHash_DH = 0;
		// uint32_t numHash_AH = 0, sumOfNumHash_AH = 0;
		// uint32_t numRemapping_AH = 0;
		uint32_t numRemapping_DH = 0;
		// uint32_t nodeID_AH;
		uint32_t key, nodeID_DH;
		clock_t t1 = clock();

		for (uint32_t i=0;i<num_keys;++i) {
			key = gen32bitRandNumber(i);
			nodeID_DH = consistentHash->getNodeID(key, &numHash_DH);
			// stat_DH[nodeID_DH] += 1;
			sumOfNumHash_DH += numHash_DH;
			// if(nodeID_DH != trace_DH[i]){
			// 	numRemapping_DH++;
			// 	trace_DH[i] = nodeID_DH;
			// }
		}

		clock_t  t2 = clock();
		double time_DH = (double) (t2-t1) / CLOCKS_PER_SEC;

		// t1 = clock();

		// for (uint32_t i=0;i<num_keys;++i) {
		// 	key = gen32bitRandNumber(i);
		// 	nodeID_AH = ah.getNodeID(key, &numHash_AH);
		// 	stat_AH[nodeID_AH] += 1;
		// 	sumOfNumHash_AH += numHash_AH;
		// 	if(nodeID_AH != trace_AH[i]){
		// 		numRemapping_AH++;
		// 		trace_AH[i] = nodeID_AH;
		// 	}
		// }

		// t2 = clock();
		// double time_AH = (double) (t2-t1) / CLOCKS_PER_SEC;
		printf("\t[ElapsedTime(s)\tRate(MKPS)\n");
		// printf("AH\t%.2lf\t%.2lf\n", time_AH, (double)num_keys/1000000.0/time_AH);
		printf("DH\t%.2lf\t%.2lf\n", time_DH, (double)num_keys/1000000.0/time_DH);
		// printf("\t[remapping count\tremapping rate]\n");
		// // printf("AH\t%u\t%f\n", numRemapping_AH, (float)numRemapping_AH/num_keys);
		// printf("DH\t%u\t%f\n", numRemapping_DH, (float)numRemapping_DH/num_keys);
		// printf("\n");
		printf("[average query complexity]\n");
		printf("DH: %f\n", (float)sumOfNumHash_DH/num_keys);
		// printf("\n");
		printf("[\tTotal\tWorking\tRemoval]\n");
		// printf("AH\t%d\t%d\t%d\n", num_total, num_working, ah.getOfflineSize());
		printf("DH\t%u\t%u\t%u\n", consistentHash->getSize(), num_working-num_removals, num_total - num_working + num_removals);
		printf("\n");
		uint32_t num_remaining = num_working - num_removals;
		double mean = (double)num_keys/num_remaining;
		// printf("[AH balcence]\n");
		// statBalance(stat_AH, num_total, mean);
		// printf("[DH balcence]\n");
		// statBalance(stat_DH, num_total, mean);
		printf("\n\n\n");

		delete []stat_DH;
		// delete []stat_AH;

		printf("Choose:[`a` n] to add n nodes, [`r` n] to remove n nodes, else to quit the test.\n");
		fflush(stdout);
		char op;
		uint32_t opn; 
		scanf(" %c", &op);
		fflush(stdout);
		fflush(stdout);
		if(op == 'a'){
			scanf("%u", &opn);
			for(uint32_t i = 0; i < opn; i++){
				// ah.updateAddition();
				consistentHash->updateAddition();
			}
			num_working += opn;
		}
		else if(op == 'r'){
			scanf("%u", &opn);
			uint32_t i = 0;
		    while (i < opn) {
				uint32_t removed = gen32bitRandNumber(rand()) % num_working;
				if (bucket_status[removed] == 1) {
					// ah.updateRemoval(removed);
					consistentHash->updateRemoval(removed);
					bucket_status[removed] = 0;
					i++;
				}
			}
			num_removals += opn;
		}
		else{
			return 0;
		}

    }

	delete []bucket_status;
	delete []trace_DH;
	delete consistentHash;
	// delete []trace_AH;
	// 
	// 
	// 
	return 0;
}
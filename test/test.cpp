#include "DxHash.h"
#include "AnchorHash.h"
#include "MaglevHash.h"
#include "Ring.h"
#include "crc32c_sse42_u64.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include <unistd.h>

using namespace std;
CH* consistentHash = nullptr;
CH* consistentHash_cmp = nullptr;

uint32_t* bucket_status;
uint32_t* load_distribution;
uint32_t* stat_results;
uint32_t num_total = 1024, num_working = 1024, num_keys = 100;
uint32_t num_total_origin = 1024, num_working_origin = 1024, CH_type;

enum 
{
	TYPE_ALL,
	TYPE_AH,
	TYPE_RING,
	TYPE_MAGLEV,
	TYPE_DH,
	TYPE_DH1,
	TYPE_DH2,
};

void printHelp(){
	const char* help_info = 
"\
-h\t Help information. \n\
-N\t The total number of nodes. \n\
-W\t The number of working nodes. \n\
-K\t The number of quired keys. \n\
-a\t Choose the tested algorithm [maglev, AH, DH, DH1, DH2, ring]. \n\
-m\t Print the memory footprint. \n\
-P\t Initiate buckets for lookup tests. \n\
-A\t Test the lookup ratio of all CH algorithm. \n\
-l\t Start lookup test. \n\
-b\t Start balance test. \n\
-r\t Removed nodes. \n\
-i\t Inserted nodes. \n\
-s\t Show migration ratio (work with insertion (i) or removal (r)). \n\
Examples: \n\
-N 100000 -W 50000 -K 10000 -P -A \t Evaluate lookup throughput of all algorithms\n\
-N 100000 -W 10000 -K 5000 -a DH -P -i 100 \t Evaluate insert latency of DxHash \n\
-N 100000 -W 50000 -a DH -m\t Print the memory footprint of current algorithms \n\
-N 1000 -W 1000 -K 10000000 -a AH -P -r 100 -s\t Print the remapping ratio of current algorithms \n\
";
	printf("%s\n", help_info);
}

void showMigration(){
	int num_mig = 0;
	uint32_t numHash;
	CH* consistentHash_cmp;
	if(CH_type == TYPE_AH){
		consistentHash_cmp = new AnchorHashQre(num_total_origin, num_working_origin);
	}
	else if(CH_type == TYPE_MAGLEV){
		consistentHash_cmp = new MaglevHash(num_working_origin, 97*num_working_origin);
	}
	else if(CH_type == TYPE_DH){
		consistentHash_cmp = new DxHash(num_total_origin, num_working_origin);
	}
	else if(CH_type == TYPE_DH1){
		consistentHash_cmp = new DxHash_TO1(num_total_origin, num_working_origin);
	}
	else if(CH_type == TYPE_DH2){
		consistentHash_cmp = new DxHash_TO2(num_total_origin, num_working_origin);
	}
	else if(CH_type == TYPE_RING){
		consistentHash_cmp = new Ring(num_working_origin, 1000);
	}
	for (uint32_t i = 0; i < num_keys; ++i) {
		int key = gen32bitRandNumber(i);
		auto nodeID_1 = consistentHash->getNodeID(key, &numHash);
		auto nodeID_2 = consistentHash_cmp->getNodeID(key, &numHash);
		if(nodeID_1 != nodeID_2){
			num_mig++;
		}
	}
	printf("Migration Ratio of %s\t%.6lf\n", consistentHash_cmp->getName(), (double)num_mig / num_keys);
	delete consistentHash_cmp;

}

void statBalance(){
	load_distribution = new uint32_t[num_total];
	uint32_t numHash;

	for (uint32_t i = 0; i < num_keys; ++i) {
		uint32_t key = gen32bitRandNumber(i);
		auto nodeID = consistentHash->getNodeID(key, &numHash);
		load_distribution[nodeID]++;
	}
	double lb, mean = num_keys / num_working;

	for(uint32_t i = 0; i < num_total; ++i){
		if(load_distribution[i] == 0)
			continue;
		lb += (load_distribution[i] - mean) * (load_distribution[i] - mean);
	}
	printf("mean: %lf, std dev: %lf\n", mean, sqrt(lb/num_working));	
}

void getStatTp(){
	clock_t t1 = clock();
	uint32_t numHash, sumOfQueryLength;
	for (uint32_t i = 0;i < num_keys; ++i) {
		int key = gen32bitRandNumber(i);
		auto nodeID = consistentHash->getNodeID(key, &numHash);
		sumOfQueryLength += numHash;
	}

	clock_t t2 = clock();
	double time = (double) (t2-t1) / CLOCKS_PER_SEC;
	printf("Lookup Rate(MKPS) of %s\t%.2lf\n", consistentHash->getName(), (double)num_keys / 1000000.0 / time);
	printf("[Average query length (valuable for AH and DH)]\t%f\n", (float)sumOfQueryLength / num_keys);
}


void insertNode(int opn){
	struct timespec start, stop;
	clock_gettime(CLOCK_REALTIME, &start);
	for(uint32_t i = 0; i < opn; i++){
		uint32_t id = consistentHash->updateAddition();
		bucket_status[id] == 1;
	}
	clock_gettime(CLOCK_REALTIME, &stop);
	printf("%s - time cost for %d node insertions: %lu ns \n", consistentHash->getName(), opn, (stop.tv_sec-start.tv_sec)*1000000000 + (stop.tv_nsec-start.tv_nsec));
	num_working += opn;
	printf("[Total\tWorking\tRemoved]\n");
	printf("%u\t%u\t%u\n\n", consistentHash->getSize(), num_working, consistentHash->getSize() - num_working);
}

void deleteNode(int opn){
	uint32_t i = 0;	
	struct timespec start, stop;
	clock_gettime(CLOCK_REALTIME, &start);
	while (i < opn) {
		uint32_t removed = gen32bitRandNumber(rand()) % num_working;
		if (bucket_status[removed] == 1) {
			// ah.updateRemoval(removed);
			consistentHash->updateRemoval(removed);
			bucket_status[removed] = 0;
			i++;
		}
	}
	clock_gettime(CLOCK_REALTIME, &stop);
	printf("%s - time cost for %d node deletions: %lu ns \n", consistentHash->getName(), opn, (stop.tv_sec-start.tv_sec)*1000000000 + (stop.tv_nsec-start.tv_nsec));
	num_working -= opn;
	printf("[Total\tWorking\tRemoved]\n");
	printf("%u\t%u\t%u\n\n", consistentHash->getSize(), num_working, consistentHash->getSize() - num_working);
}

void init_bucket(){
	bucket_status = new uint32_t[num_total]();
    for (int i = 0; i < num_working; i++) {
		bucket_status[i] = 1;
	}
}

void printMemoryFootprint(){
	printf("%s size: %u\n", consistentHash->getName(), consistentHash->getSize());
	pid_t process_id;
	process_id = getpid();
	char cmdBuf[256];
	sprintf(cmdBuf, "ps -aux | grep %d", process_id);
	char readBuf[1024] = {0};
	FILE *file = popen(cmdBuf, "r");
	fread(readBuf, 1024, 1, file);
	printf("%s", readBuf);
}

int main(int argc, char* const *argv)
{
	int opt, opn;
	while((opt = getopt(argc, argv, "hAPN:W:K:a:mr:i:slb")) != -1){
		switch(opt){
			case 'h':
				printHelp();
				break;
			case 'N':
				num_total = atoi(optarg);
				num_total_origin = num_total;
				break;
			case 'W':
				num_working = atoi(optarg);
				num_working_origin = num_working;
				break;
			case 'K':
				num_keys = atoi(optarg);
				break;		

			case 'P':
				init_bucket();
				break;

			case 'A':
				consistentHash = new MaglevHash(num_total, 11*num_total);
				getStatTp();					
				delete consistentHash;
				consistentHash = new AnchorHashQre(num_total, num_working);
				getStatTp();
				delete consistentHash;
				consistentHash = new DxHash(num_total, num_working);
				getStatTp();
				delete consistentHash;
				consistentHash = new Ring(num_total, 100);
				getStatTp();
				CH_type = TYPE_ALL;
				break;

			case 'a':
				if(optarg == string("maglev")){
					consistentHash = new MaglevHash(num_working, 97*num_working);
					CH_type = TYPE_MAGLEV;

				}	
				else if(optarg == string("AH")){
					consistentHash = new AnchorHashQre(num_total, num_working);
					CH_type = TYPE_AH;
				}	
				else if (optarg == string("DH")){
					consistentHash = new DxHash(num_total, num_working);
					CH_type = TYPE_DH;
				}
				else if (optarg == string("DH1")){
					consistentHash =  new DxHash_TO1(num_total, num_working);
					CH_type = TYPE_DH1;
				}
				else if (optarg == string("DH2")){
					consistentHash = new DxHash_TO2(num_total, num_working);
					CH_type = TYPE_DH2;
				}
				else if (optarg == string("ring")){
					consistentHash = new Ring(num_working, 1000);
					CH_type = TYPE_RING;
				}
				else 
				{
					printf("Invalid input: %s\n", optarg);
				}

				break;

			case 'm':
				printMemoryFootprint();
				break;

			case 'r':
				opn = atoi(optarg);
				deleteNode(opn);
				break;

			case 'i':
				opn = atoi(optarg);
				insertNode(opn);
				break;

			case 's':
				showMigration();
				break;

			case 'l':
				getStatTp();
				break;
			case 'b':
				statBalance();
				break;
		}
	}
	if(consistentHash)
		delete consistentHash;
	if(consistentHash_cmp)
		delete consistentHash_cmp;
	if(bucket_status)
		delete[] bucket_status;
	return 0;
}
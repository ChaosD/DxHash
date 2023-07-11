Compile the binary: 
$ make

Command of test:

-h Help information.
-N The total number of nodes. 
-W The number of working nodes.
-K The number of quired keys.
-a Choose the tested algorithm [maglev, AH, DH, DH1, DH2, or ring]. 
	(DH: bit NSArray and without IQueue; DH1: Byte NSArray without IQueue; DH2: Byte NSArray with IQueue).
-m Print the memory footprint.
-P Initiate buckets for lookup/removal/insertion tests.
-A Test the lookup ratio of all CH algorithm.
-l Start lookup test.
-b Start balance test.
-r Removed nodes.
-i Inserted nodes.
-s Show migration ratio (work with insertion (i) or removal (r)).

Example: 
Evaluate the lookup throughputs of all algorithms (100K nodes, 50K active nodes, and 10K keys):
$ ./test -N 100000 -W 50000 -K 10000 -P -A

Evaluate the lookup throughputs of AH (100K nodes, 50K active nodes, and 10K keys):
$ ./test -N 100000 -W 50000 -K 10000 -a AH -P -l

Evaluate the insertion latency of DxHash. The original cluster with 100K nodes, 50K active nodes, and 5K keys. The result is the latency of inserting 100 active nodes.
$./test -N 100000 -W 10000 -K 5000 -a DH -P -i 100 

Print the memory footprint of DH(100K nodes, 50K active nodes)
$./test -N 100000 -W 50000 -a DH -m

Evaluate load balancing of AH (1M nodes, 1M active nodes, and 100M keys)
$./test -N 1000000 -W 1000000 -K 100000000 -a AH -P -b

Evaluate remapping ratio of AH (1K nodes, 1K active nodes, removing 100 nodes)
$./test -N 1000 -W 1000 -K 10000000 -a AH -P -r 100 -s
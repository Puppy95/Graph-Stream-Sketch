
The archive 'GSS-ICDE' contains code for the ICDE paper 'Fast and Accurate graph stream summarization', and 'GSS-TKDE' archive contains the code for TKDE paper :'Graph stream sketch: Summarizing graph streams with high speed and accuracy'. 
We recommend to use the TKDE version, it is more organized and has more improved versions. The folloing instruction is based on the TKDE version. And there is anothe readme fild in the GSS-ICDE archive for the ICDE version.
## Compile and Execution

Compile demo.cpp with
g++ -O2 -std=c++11 -o demo demo.cpp

and run it with

./demo [path of setting file] [path of data file] [path of query file] QueryType

The setting file contains some parameters to run the experiments. They are organized as follows:

Times of memory usage of TCM to GSS.
Length of address sequence of GSS
Number of candidate buckets of GSS
Whether the update stream include deletion (1 or 0) (A negative weight means deletion. If there are deletions, the insertion of GSS will be a little bit different, as explained in the paper)
Whether using hash tables to store the original node IDs (1 or 0) (It is necessary for queries that reports node IDs, like 1-hop successor/ precursor query).
Hash table size (set to a little bit larger than the expected node number in the graph stream. Smaller value will decrease the processing speed).
Round of experiments.
Matrix lengths of GSS in each round.

I have provided the setting file for experiments of the three datasets I use in experiments: lkml-setting, caida-setting and twitter setting.
Note that the GSS structure in the code takes matrix size as input, namely matrix length * matrix length * room number in each bucket. It will be easier to decide this value, as it can be set to the expected edge number in the graph stream. But in the setting file I still use matrix length to keep consistent with the experiments in the paper. In demo.cpp I compute the matrix size with the matrix length and provide it to GSS.

The data file contains the graph stream, where each line contains an edge, organized as:

SourceNodeID DestinationNodeID EdgeWeight  

the IDs can be any strings, and weight is an integer.

The query file contains queries. For edge query and reachability query, each line of the query file is a node pair. For 1-hop successor/precursor query, node query （successor/ precursor node query means to compute the sum of the weight of out-in edges of a node） and SSSP query, each line is a node ID.

I have provided the data file and query files of lkml-reply dataset in the dataset folder. lkml-reply.txt is the data file, lkml-edge.txt is the queries we use for edge query test, namely distinct edges in the graph. lkml-node.txt contains queries we use for 1-hop precursor/successor query and successor/precursor node queries , namely all nodes in the graph. lkml-unreachable.txt contains queries we use for reachability query. They are 100 unreachable node pairs. lkml-root.txt contains queries we use for SSSP query, namely top-100 high degree nodes in the graph.

QueryType can be set from 0 to 6, corresponding to 7 kinds of queries:

edge query, 1-hop successor query, 1-hop precursor query, successor node query, precursor node query,  reachability query, SSSP query.

The demo will uses linked lists to produce ground truth, and computing the ARE or precision of queries of GSS (16 bit fingerprint), GSS (12 bit fingerprint), TCM.

It will also report the insertion speed, memory usage and query speed of different methods. Note that we omit the memory usage of GSS(12 bit) and TCM, since they are set according to the memory of GSS(16 bit). The demo will also report the buffer size of GSS(16 bit). By dividing it by the distinct edge number, we can compute buffer percentage.


## Parameter Adjustment

Some detailed parameters can be adjusted in "GSS/setting.h", including:

The buffer type, either linked list or hash table based buffer. For skewed twitter dataset, we need to use hash table based buffer.
The type of bitmaps used in GSS with node bitmaps, namely GSSnb or GSSsb introduced in the paper. dn and ds means we use two bitmaps for each vertex to record its successor and precursor separately. 
The number of rooms in each bucket.
The length of bitmap. The comments introduced how to compute them. For twitter dataset, they need to be adjust to 32 and 2. 
The type of bucket with different fingerprint length. It can be chosen from 12 bit, 16 bit and 20 bit. We use different compact implementations for them.



## Version Change

The demo uses the original version of GSS. Users can test different versions of GSS by editing the demo.cpp file, and change the 
#include "GSS/GSS-origin.h" 
to 
#include "GSS/GSS-block.h"
or
#include "GSS/GSS-bitset.h" 

They are corresponding to blocked GSS and GSS with node bitmaps introduced in the paper.

Note that when using these two versions, the fingerprint length needs to be longer to keep the same accuracy, as introduced in the paper. To achieve this, users need to change the bucket file included in the "GSS/setting.h" to bucket_20.h, and set the fingerprint size in demo.cpp to 16 and 20, respectively (line 211 and 428).
  
 




 



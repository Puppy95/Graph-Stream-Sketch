
// definitions for convinience of code writting.
#define Src_Direct 0
#define Dst_Direct 1
#define add_edge 1
#define modify_edge 0
#define delete_edge -1
#define bigger_p 22613 
#define large_p 200297
#define very_large_p 1971237 



// settings of GSS
#define my_buffer icl_graph  // type of buffer storage, icl_graph or hb_graph. icl_graph means linked list based buffer, while hb_graph means hash table based buffer
#define bitset dn_bitset // type of bitset, used only in GSS-bitset. s_bitset means short bitset (row + column bits), n_bitset means node bitset (row * column bits); ds_bitset is double short bitset ( 1 s_bitset for successor and another for precursor, and 
// dn_bitset means double node bitset. When r = 8, they consume s_bitset = 2bytes, n_bitset = 8bytes, ds_bitset = 4bytes amd dn_bitset = 16bytes for each node.
#define room_num 8   // room number in each bucket
#define n_bitlength 8 // length of each n_bitset, with bytes as unit. n_bitlength = r*r/8.  
#define s_bitlength 1 // length of half s_bitset (row or column), with bytes as unit. s_bitlength = r/8;
#include "../bucket/bucket_16.h" // different bucket for different fingerprint length. bucket_12.h , bucket_16.h, or bucket_20.h




// common structs
#include "../common/icl_graph.h"
#include "../common/hash-based-graph.h"
#ifndef Hash
#include "../common/hashfunction.h"
#define Hash
#endif	
#include "../common/bit_set.h"

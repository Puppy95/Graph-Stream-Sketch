#pragma once
#include<iostream>
#include<vector>
#include<map>
#include<memory.h> 
#include<algorithm> 
#include<set>
#include<string>

#ifndef room_num
#define room_num 8
#endif


#define fp_len 20
#include<time.h>
using namespace std;
// structure for GSS bucket with 20 bit fingerprint. 
//There are 2 variants, bucket and bucket_wit_index. The first is for GSS-block, and the latter is for GSS-original, where each room contain an index pair (i, j) indicating the index 
// of the bucket in the address sequence of the two endpoints of the edge. i is the index of the row in the address sequence of the src node, and j is the index of the column in the address 
// sequence of the dst edge.


class bucket
{
public:
	unsigned short src_fp[room_num]; // the higher 16 bit of each fingerprint of src node is stored hear  
	unsigned short dst_fp[room_num]; // the higher 16 bit of each fingerprint of dst node is stored here  
	unsigned char remain[room_num]; // the higher 4 bit of each remain is the lower 4 bit of the fingerprint of src node of an edge, and the lower 4 bit of each reamin is the lower 4 bit of a dst node
	int weight[room_num];
	bucket()
	{
		for (int i = 0; i < room_num; i++)
		{
			weight[i] = 0;
			src_fp[i] = 0;
			dst_fp[i] = 0;
			remain[i] = 0;
		}

	}
	int get_mem()
	{
		return room_num * 9;
	}
	void clear()
	{
		for (int i = 0; i < room_num; i++)
		{
			weight[i] = 0;
			src_fp[i] = 0;
			dst_fp[i] = 0;
			remain[i] = 0;
		}

	}
	void get_room_src(int src_rfp[room_num]) // get the src fingerprint of each room 
	{
		for (int i = 0; i < room_num; i++)
		{
			src_rfp[i] = (((unsigned int)src_fp[i]) << 4 | (remain[i] >> 4)); // concatenate the higher 16 bit stored in src_fp and the lower 4 bit stored in remain. 
		}

	}

	void get_room_dst(int dst_rfp[room_num]) // get the dst fingerprint of each room 
	{
		for (int i = 0; i < room_num; i++)
		{
			dst_rfp[i] = (((unsigned int)dst_fp[i]) << 4 | (remain[i] & 0xF)); // concatenate the higher 16 bit in dst_fp and the lower 4 bit in reamin
		}
	}

	int get_src_fp(int r_num) // get the src fingerprint of a given room
	{
		return (((unsigned int)src_fp[r_num]) << 4 | (remain[r_num] >> 4));
	}
	int get_dst_fp(int r_num) // get the dst fingerprint of a given room
	{
		return (((unsigned int)dst_fp[r_num]) << 4 | (remain[r_num] & 0xF));
	}
	void set_src_fp(int r_num, int fp) // set the src fingerprint of room with index r_num to value fp
	{
		src_fp[r_num] = (fp >> 4);
		remain[r_num] = (remain[r_num] | ((fp & 0xF) << 4));
	}

	void set_dst_fp(int r_num, int fp) // set the dst fingerprint of room with index r_num to value fp
	{
		dst_fp[r_num] = (fp >> 4);
		remain[r_num] = (remain[r_num] | (fp & 0xF));
	}

	void clear_src_fp(int r_num) // reset the src fingerprint of room with index r_num to 0
	{
		src_fp[r_num] = 0;
		remain[r_num] = (remain[r_num] & 0xF);
	}

	void clear_dst_fp(int r_num)  // reset the dst fingerprint of room with index r_num to 0
	{
		dst_fp[r_num] = 0;
		remain[r_num] = (remain[r_num] & 0xF0);
	}

};


class bucket_with_index
{
public:
	unsigned short src_fp[room_num];
	unsigned short dst_fp[room_num];
	unsigned char remain[room_num];
	unsigned char ind[room_num]; // compared to the above variant, there is a char ind for each room, which represens an index pair (i, j), i is stored in the higher 4 bit and j is in the lower 4 bit
	int weight[room_num];
	bucket_with_index()
	{
		for (int i = 0; i < room_num; i++)
		{
			weight[i] = 0;
			src_fp[i] = 0;
			dst_fp[i] = 0;
			remain[i] = 0;
			ind[i] = 0;
		}

	}

	int get_mem()
	{
		return room_num * 10;
	}
	void clear()
	{
		for (int i = 0; i < room_num; i++)
		{
			weight[i] = 0;
			src_fp[i] = 0;
			dst_fp[i] = 0;
			remain[i] = 0;
			ind[i] = 0;
		}

	}
	void get_room_src(int src_rfp[room_num]) // get the src fingerprint of each room 
	{
		for (int i = 0; i < room_num; i++)
		{
			src_rfp[i] = (((unsigned int)src_fp[i]) << 4 | (remain[i] >> 4));
		}

	}

	void get_room_dst(int dst_rfp[room_num]) // get the dst fingerprint of each room 
	{
		for (int i = 0; i < room_num; i++)
		{
			dst_rfp[i] = (((unsigned int)dst_fp[i]) << 4 | (remain[i] & 0xF));
		}
	}

	int get_src_fp(int r_num) // get the src fingerprint of a given room with index r_num
	{
		return (((unsigned int)src_fp[r_num]) << 4 | (remain[r_num] >> 4));
	}
	int get_dst_fp(int r_num) // get the dst fingerprint of a given room r_num
	{
		return (((unsigned int)dst_fp[r_num]) << 4 | (remain[r_num] & 0xF));
	}
	void set_src_fp(int r_num, int fp) // set the src fingerprint of a given room with index r_num to fp 
	{
		src_fp[r_num] = (fp >> 4);
		remain[r_num] = (remain[r_num] | ((fp & 0xF) << 4));
	}

	void set_dst_fp(int r_num, int fp) // set the dst fingerprint of a given room with index r_num to fp 
	{
		dst_fp[r_num] = (fp >> 4);
		remain[r_num] = (remain[r_num] | (fp & 0xF));
	}

	void clear_src_fp(int r_num) // reset the src fingerprint of a given room with index r_num to 0
	{
		src_fp[r_num] = 0;
		remain[r_num] = (remain[r_num] & 0xF);
	}

	void clear_dst_fp(int r_num) // reset the dst fingerprint of a given room with index r_num to 0
	{
		dst_fp[r_num] = 0;
		remain[r_num] = (remain[r_num] & 0xF0);
	}
	int get_src_ind(int r_num) // get the first value of the index pair in root r_num
	{
		return (ind[r_num] >> 4);
	}
	int get_dst_ind(int r_num) // get the second value of the index pair in root r_num
	{
		return (ind[r_num] & 0xF);
	}
	void set_src_ind(int r_num, int val) // set the first value of the index pair in root r_num to val 
	{
		ind[r_num] |= (val << 4);
	}
	void set_dst_ind(int r_num, int val) // set the second value of the index pair in root r_num to val
	{
		ind[r_num] |= val;
	}

	void clear_src_ind(int r_num) // reset the first value of the index pair in root r_num to 0
	{
		ind[r_num] &= 0xF;
	}
	void clear_dst_ind(int r_num, int val) // reset the second value of the index pair in root r_num to 0
	{
		ind[r_num] &= 0xF0;
	}
	bool compare_ind(int r_num, int val) // compare the index pair to a given index pair, and return if they are equal
	{
		return (ind[r_num] == val);
	}
	void clear_ind(int r_num) // reset the index pair in a given room to 0
	{
		ind[r_num] = 0;
	}
	void set_ind(int r_num, int val) // set the index pair in a given room to val 
	{
		ind[r_num] = val;
	}
};

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

#define fp_len 16
#include<time.h>
using namespace std;
// structure for GSS bucket with 16 bit fingerprint. 
//There are 2 variants, bucket and bucket_wit_index. The first is for GSS-block, and the latter is for GSS-original, where each room contain an index pair (i, j) indicating the index 
// of the bucket in the candidate bucket sequence of the stored edge.

class bucket
{
public:
	unsigned short src_fp[room_num]; // src fingerprint of each room
	unsigned short dst_fp[room_num]; // dst fingerprint of each room
	int weight[room_num]; // weight of the edge in each room 
	bucket()
	{
		for (int i = 0; i < room_num; i++)
		{
			weight[i] = 0;
			src_fp[i] = 0;
			dst_fp[i] = 0;
		}

	}
	int get_mem()
	{
		return room_num * 8;
	}
	void clear()
	{
		for (int i = 0; i < room_num; i++)
		{
			weight[i] = 0;
			src_fp[i] = 0;
			dst_fp[i] = 0;
		}

	}
	void get_room_src(int src_rfp[room_num]) // get the src fingerprint of each room 
	{
		for (int i = 0; i <room_num; i++)
			src_rfp[i] = src_fp[i];

	}

	void get_room_dst(int dst_rfp[room_num]) // get the dst fingerprint of each room 
	{
		for (int i = 0; i <room_num; i++)
			dst_rfp[i] = dst_fp[i];
	}

	int get_src_fp(int r_num) // get the src fingerprint of a given room
	{
		return src_fp[r_num];
	}
	int get_dst_fp(int r_num) // get the dst fingerprint of a given room
	{
		return dst_fp[r_num];
	}
	void set_src_fp(int r_num, int fp) // set the src fingerprint of room with index r_num to value fp
	{
		src_fp[r_num] = fp;
	}

	void set_dst_fp(int r_num, int fp) // set the dst fingerprint of room with index r_num to value fp
	{
		dst_fp[r_num] = fp;
	}

	void clear_src_fp(int r_num) // reset the src fingerprint of room with index r_num to value 0
	{
		src_fp[r_num] = 0;
	}

	void clear_dst_fp(int r_num) // reset the dst fingerprint of room with index r_num to value 0
	{
		dst_fp[r_num] = 0;
	}

};


class bucket_with_index
{
public:
	unsigned short src_fp[room_num];
	unsigned short dst_fp[room_num];
	unsigned char ind[room_num];  // compared to the above variant, there is a char ind for each room, which represens an index pair (i, j), i is stored in the higher 4 bit and j is in the lower 4 bit
	int weight[room_num];
	bucket_with_index()
	{
		for (int i = 0; i < room_num; i++)
		{
			weight[i] = 0;
			src_fp[i] = 0;
			dst_fp[i] = 0;
			ind[i] = 0;
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
			ind[i] = 0;
		}

	}
	void get_room_src(int src_rfp[room_num]) // get the src fingerprint of each room 
	{
		for (int i = 0; i <room_num; i++)
			src_rfp[i] = src_fp[i];

	}

	void get_room_dst(int dst_rfp[room_num])// get the dst fingerprint of each room 
	{
		for (int i = 0; i <room_num; i++)
			dst_rfp[i] = dst_fp[i];
	}

	int get_src_fp(int r_num) // get the src fingerprint of a given room
	{
		return src_fp[r_num];
	}
	int get_dst_fp(int r_num) // get the dst fingerprint of a given room
	{
		return dst_fp[r_num];
	}
	void set_src_fp(int r_num, int fp) // set the src fingerprint of room with index r_num to value fp
	{
		src_fp[r_num] = fp;
	}

	void set_dst_fp(int r_num, int fp) // set the dst fingerprint of room with index r_num to value fp
	{
		dst_fp[r_num] = fp;
	}

	void clear_src_fp(int r_num) // reset the src fingerprint of room with index r_num to value 0
	{
		src_fp[r_num] = 0;
	}

	void clear_dst_fp(int r_num) // reset the dst fingerprint of room with index r_num to value 0
	{
		dst_fp[r_num] = 0;
	}

	int get_src_ind(int r_num) // get the first value of the index pair in root r_num
	{
		return (ind[r_num] >> 4);
	}
	int get_dst_ind(int r_num) // get the second value of the index pair in root r_num
	{
		return (ind[r_num] &0xF);
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

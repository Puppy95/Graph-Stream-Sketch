#pragma once
#include<iostream>
#include<vector>
#include<map>
#include<memory.h> 
#include<algorithm> 
#include<set>
#include<string>

#ifndef n_bitlength
#define n_bitlength 32
#endif

#ifndef s_bitlength
#define s_bitlength 2
#endif

using namespace std;


class n_bitset
{
	char A[n_bitlength];
public:
	n_bitset()
	{
		memset(A, 0, sizeof(char)*n_bitlength);
	}
	void clear()
	{
		memset(A, 0, sizeof(char)*n_bitlength);
	}
	void set_bit(int i, int r, int Direct)
	{
		if (i < 0)
			return;
		int pos = i / 8;
		int offset = i % 8;
		A[pos] |= (1 << offset);
	}
	void clear_bit(int i, int r, int Direct)
	{
		int pos = i / 8;
		int offset = i % 8;
		A[pos] &= ~(1 << offset);

	}
	bool get_bit(int i, int r, int Direct)
	{
		int pos = i / 8;
		int offset = i % 8;
		if ((A[pos] >> offset) & 1 == 1)
			return true;
		else
			return false;
	}
	int get_mem()
	{
		return n_bitlength;
	}
};

class s_bitset
{
	char R[s_bitlength];
	char C[s_bitlength];
public:
	s_bitset()
	{
		memset(R, 0, sizeof(char)*s_bitlength);
		memset(C, 0, sizeof(char)*s_bitlength);
	}
	void clear()
	{
		memset(R, 0, sizeof(char)*s_bitlength);
		memset(C, 0, sizeof(char)*s_bitlength);
	}
	void set_bit(int i, int r, int Direct)
	{

		if (i < 0)
			return;
		int row = i / r;
		int col = i % r;
		int pos = row / 8;
		int offset = row % 8;
		R[pos] |= (1 << offset);

		pos = col / 8;
		offset = col % 8;
		C[pos] |= (1 << offset);
	}
	void clear_bit(int i, int r, int Direct)
	{
		if (i < 0)
			return;
		int row = i / r;
		int col = i % r;
		int pos = row / 8;
		int offset = row % 8;
		R[pos] &= ~(1 << offset);

		pos = col / 8;
		offset = col % 8;
		C[pos] &= ~(1 << offset);
	}
	bool get_bit(int i, int r, int Direct)
	{
		int row = i / r;
		int col = i % r;

		int r_pos = row / 8;
		int r_offset = row % 8;

		int c_pos = col / 8;
		int c_offset = col % 8;

		if (((R[r_pos] >> r_offset) & 1) == 1 && ((C[c_pos] >> c_offset) & 1) == 1)
			return true;
		else
			return false;
	}
	int get_mem()
	{
		return s_bitlength*2;
	}
};

class dn_bitset
{
	char S[n_bitlength];
	char P[n_bitlength];
public:
	dn_bitset()
	{
		memset(S, 0, sizeof(char)*n_bitlength);
		memset(P, 0, sizeof(char)*n_bitlength);
	}
	void clear()
	{
		memset(S, 0, sizeof(char)*n_bitlength);
		memset(P, 0, sizeof(char)*n_bitlength);
	}
	void set_bit(int i, int r, int Direct)
	{
		if (i < 0)
			return;
		int pos = i / 8;
		int offset = i % 8;
		if (Direct == Src_Direct)
			S[pos] |= (1 << offset);
		else if (Direct == Dst_Direct)
			P[pos] |= (1 << offset);
		else
			return;
	}

	void clear_bit(int i, int r, int Direct)
	{
		int pos = i / 8;
		int offset = i % 8;
		if (Direct == Src_Direct)
			S[pos] &= ~(1 << offset);
		else if (Direct == Dst_Direct)
			P[pos] &= ~(1 << offset);
		else
			return;
	}
	bool get_bit(int i, int r, int Direct)
	{
		int pos = i / 8;
		int offset = i % 8;
		if (Direct == Src_Direct)
			return ((S[pos] >> offset) & 1 == 1);
		else 
			return ((P[pos] >> offset) & 1 == 1);
	}
	int get_mem()
	{
		return n_bitlength*2;
	}
};


class ds_bitset
{
	char SR[s_bitlength];
	char SC[s_bitlength];
	char PR[s_bitlength];
	char PC[s_bitlength];
public:
	ds_bitset()
	{
		memset(SR, 0, sizeof(char)*s_bitlength);
		memset(SC, 0, sizeof(char)*s_bitlength);
		memset(PR, 0, sizeof(char)*s_bitlength);
		memset(PC, 0, sizeof(char)*s_bitlength);
	}
	void clear()
	{
		memset(SR, 0, sizeof(char)*s_bitlength);
		memset(SC, 0, sizeof(char)*s_bitlength);
		memset(PR, 0, sizeof(char)*s_bitlength);
		memset(PC, 0, sizeof(char)*s_bitlength);
	}
	void set_bit(int i, int r, int Direct)
	{

		if (i < 0)
			return;
		int row = i / r;
		int col = i % r;
		int pos = row / 8;
		int offset = row % 8;
		if (Direct==Src_Direct)
			SR[pos] |= (1 << offset);
		else if (Direct==Dst_Direct)
			PR[pos] |= (1 << offset);

		pos = col / 8;
		offset = col % 8;
		if (Direct == Src_Direct)
			SC[pos] |= (1 << offset);
		else if (Direct == Dst_Direct)
			PC[pos] |= (1 << offset);
	}
	void clear_bit(int i, int r, int Direct)
	{
		if (i < 0)
			return;
		int row = i / r;
		int col = i % r;
		int pos = row / 8;
		int offset = row % 8;
		if (Direct == Src_Direct)
			SR[pos] &= ~(1 << offset);
		else if (Direct == Dst_Direct)
			PR[pos] &= ~(1 << offset);

		pos = col / 8;
		offset = col % 8;
		if (Direct == Src_Direct)
			SC[pos] &= ~(1 << offset);
		else if (Direct == Dst_Direct)
			PC[pos] &= ~(1 << offset);
	}
	bool get_bit(int i, int r, int Direct)
	{
		int row = i / r;
		int col = i % r;

		int r_pos = row / 8;
		int r_offset = row % 8;

		int c_pos = col / 8;
		int c_offset = col % 8;

		if (Direct == Src_Direct)
			return (((SR[r_pos] >> r_offset) & 1) == 1 && ((SC[c_pos] >> c_offset) & 1) == 1);
		else
			return (((PR[r_pos] >> r_offset) & 1) == 1 && ((PC[c_pos] >> c_offset) & 1) == 1);
	}
	int get_mem()
	{
		return s_bitlength*4;
	}
};

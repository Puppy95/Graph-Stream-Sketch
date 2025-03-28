#include<iostream>
#include<vector>
#include<map>
#include<memory.h> 
#include<algorithm> 
#include<set>
#include<string>
#include<time.h>


#include"setting.h"
#include"../common/hashtable.h"
using namespace std;

//#define my_buffer icl_graph

// divided into blocks, with no node bitmap

class GSS
{
public:
	bucket** matrix;
	int r;
	int m;
	int range;
	int candidate;
	my_buffer* buffer;
	bool use_ht;
	hashtable *ht;
	unsigned long long multiple[16];
	unsigned long long adder[16];
	GSS(int matrix_size_, int r_, int candidate_, bool use_hashtable, int fp_size = fp_len, int table_size = initial_size)
	{
		r = r_;
		candidate = candidate_;
		m = sqrt(matrix_size_ / room_num / r / r) + 1;
		range = pow(2, fp_size)*m;
		use_ht = use_hashtable;
		if (use_hashtable)
			ht = new hashtable(table_size);
		else
			ht = new hashtable(initial_size);
		matrix = new bucket*[r*r];
		for (int i = 0; i<r*r; i++)
		{
			matrix[i] = new bucket[m*m];
			for (int j = 0; j<m*m; j++)
			{
				matrix[i][j].clear();
			}
		}
		buffer = new my_buffer;
		multiple[0] = 1;
		multiple[1] = 5;
		multiple[2] = 25;
		multiple[3] = 125;
		multiple[4] = 625;
		multiple[5] = 3125;
		multiple[6] = 15625;
		multiple[7] = 78125;
		multiple[8] = 390625;
		multiple[9] = 1953125;
		multiple[10] = 9765625;
		multiple[11] = 48828125;
		multiple[12] = 244140625;
		multiple[13] = 1220703125;
		multiple[14] = 6103515625;
		multiple[15] = 30517578125;
		adder[0] = 0;
		adder[1] = 739;
		adder[2] = 4434;
		adder[3] = 22909;
		adder[4] = 115284;
		adder[5] = 577159;
		adder[6] = 2886534;
		adder[7] = 14433409;
		adder[8] = 72167784;
		adder[9] = 360839659;
		adder[10] = 1804199034;
		adder[11] = 9020995909;
		adder[12] = 45104980284;
		adder[13] = 225524902159;
		adder[14] = 1127624511534;
		adder[15] = 5638122558409;
	}
	~GSS()
	{
		for (int i = 0; i<r*r; i++)
			delete[]matrix[i];
		delete[]matrix;
		delete buffer;
		delete ht;
	}

	int get_mem()
	{
		int mem = 8 * 32 + 4 * 4 + 8 + 8 + 8 + 1;
		mem += r*r*8;
		mem += matrix[0][0].get_mem() * r * r * m * m;
		if (buffer)
			mem += buffer->get_mem();
		if (ht)
			mem += ht->get_mem();
		return mem;
	}

	int get_buffer_size()
	{
		return buffer->get_edge_num();
	}

	void insert(string s, string d, int w)
	{
		unsigned int hash1 = MurmurHash((const unsigned char*)s.c_str(), s.length()) % range;
		unsigned int hash2 = MurmurHash((const unsigned char*)d.c_str(), d.length()) % range;
		unsigned int h1 = hash1%m;
		unsigned int g1 = hash1 / m;
		if (g1 == 0)
			g1++;
		unsigned int h2 = hash2%m;
		unsigned int g2 = hash2 / m;
		if (g2 == 0)
			g2++;
		unsigned int code_s = g1*m + h1;
		unsigned int code_d = g2*m + h2;
		if (use_ht){
			ht->insert(code_s, s);
			ht->insert(code_d, d);
		}
		int* tmp1 = new int[r];
		int* tmp2 = new int[r];
		for (int i = 0; i < r; i++)
		{
			tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % bigger_p;
			tmp2[i] = ((unsigned long long)g2*multiple[i] + adder[i]) % bigger_p;
		}
		int i = (code_s + code_d) % bigger_p;

		for (int k = 0; k < candidate; k++)
		{
			int index = i % (r*r);
			int index1 = index / r;
			int index2 = index%r;
			int p1 = (h1 + tmp1[index1]) % m;
			int p2 = (h2 + tmp2[index2]) % m;
			int pos = p1*m + p2;
			for (int j = 0; j < room_num; j++){
				int src = matrix[index][pos].get_src_fp(j);
				int dst = matrix[index][pos].get_dst_fp(j);
				if (src == g1&&dst == g2)
				{
					matrix[index][pos].weight[j] += w;
					return;
				}
				else if (src == 0 && dst == 0)
				{
					matrix[index][pos].set_src_fp(j, g1);
					matrix[index][pos].set_dst_fp(j, g2);
					matrix[index][pos].weight[j] = w;
					return;
				}
			}
			i = (i * 5 + 1071) % bigger_p;
		}
		buffer->insert(code_s, code_d, w);
	}

	void insert_with_delete(string s, string d, int w)
	{
		unsigned int hash1 = MurmurHash((const unsigned char*)s.c_str(), s.length()) % range;
		unsigned int hash2 = MurmurHash((const unsigned char*)d.c_str(), d.length()) % range;
		unsigned int h1 = hash1%m;
		int g1 = hash1 / m;
		if (g1 == 0)
			g1++;
		unsigned int h2 = hash2%m;
		int g2 = hash2 / m;
		if (g2 == 0)
			g2++;
		unsigned int code_s = g1*m + h1;
		unsigned int code_d = g2*m + h2;
		
		if(use_ht&&w>0)
		{
			ht->insert(code_s, s);
			ht->insert(code_d, d);
		}

		int* tmp1 = new int[r];
		int* tmp2 = new int[r];
		for (int i = 0; i<r; i++)
		{
			tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % bigger_p;
			tmp2[i] = ((unsigned long long)g2*multiple[i] + adder[i]) % bigger_p;
		}
		int i = (code_s + code_d) % bigger_p;

		int empty_pos = -1;
		int empty_index = -1;
		int empty_room = -1;
		bool find_empty = false;
		for (int k = 0; k<candidate; k++)
		{
			int index = i % (r*r);
			int index1 = index / r;
			int index2 = index%r;
			int p1 = (h1 + tmp1[index1]) % m;
			int p2 = (h2 + tmp2[index2]) % m;
			int pos = p1*m + p2;
			for (int j = 0; j<room_num; j++){
				int src = matrix[index][pos].get_src_fp(j);
				int dst = matrix[index][pos].get_dst_fp(j);
				if (src == g1&&dst == g2)
				{
					matrix[index][pos].weight[j] += w;
					if (matrix[index][pos].weight[j] <= 0)
					{
						matrix[index][pos].clear_src_fp(j);
						matrix[index][pos].clear_dst_fp(j);
						if (use_ht)
						{
							ht->decrease(code_s);
							ht->decrease(code_d);
						}
					}
					return;
				}
				else if (src == 0 && dst == 0)
				{
					if (!find_empty)
					{
						empty_index = index;
						empty_pos = pos;
						empty_room = j;
						find_empty = true;
					}
				}
			}
			i = (i * 5 + 1071) % bigger_p;
		}



		if (find_empty)// if there is any empty pos in the matrix, try to find the edge in the buffer, and if find, move it from the buffer to the matrix
		{
			int old_w = buffer->remove(code_s, code_d);
			int new_w = w + old_w;
			if (new_w == 0 && use_ht)
			{
				ht->decrease(code_s);
				ht->decrease(code_d);
			}
			else{
				if (use_ht && old_w == 0){
					ht->increase(code_s);
					ht->increase(code_d);
				}
				matrix[empty_index][empty_pos].set_src_fp(empty_room, g1);
				matrix[empty_index][empty_pos].set_dst_fp(empty_room, g2);
				matrix[empty_index][empty_pos].weight[empty_room] = new_w;
			}

		}
		else
		{
			int tmp = buffer->insert_with_delete(code_s, code_d, w);
			if (use_ht){
				if (tmp == add_edge)
				{
					ht->increase(code_s); 
					ht->increase(code_d);
				}
				else if (tmp == delete_edge)
				{
					ht->decrease(code_s);
					ht->decrease(code_d);
				}
			}
		}
		return;
	}

	bool remove(string s, string d)
	{
		unsigned int hash1 = MurmurHash((const unsigned char*)s.c_str(), s.length()) % range;
		unsigned int hash2 = MurmurHash((const unsigned char*)d.c_str(), d.length()) % range;
		unsigned int h1 = hash1%m;
		int g1 = hash1 / m;
		if (g1 == 0)
			g1++;
		unsigned int h2 = hash2%m;
		int g2 = hash2 / m;
		if (g2 == 0)
			g2++;
		unsigned int code_s = g1*m + h1;
		unsigned int code_d = g2*m + h2;
		int* tmp1 = new int[r];
		int* tmp2 = new int[r];
		for (int i = 0; i<r; i++)
		{
			tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % bigger_p;
			tmp2[i] = ((unsigned long long)g2*multiple[i] + adder[i]) % bigger_p;
		}
		int i = (code_s + code_d) % bigger_p;

		for (int k = 0; k<candidate; k++)
		{
			int index = i % (r*r);
			int index1 = index / r;
			int index2 = index%r;
			int p1 = (h1 + tmp1[index1]) % m;
			int p2 = (h2 + tmp2[index2]) % m;
			int pos = p1*m + p2;

			for (int j = 0; j<room_num; j++){
				int src = matrix[index][pos].get_src_fp(j);
				int dst = matrix[index][pos].get_dst_fp(j);
				if (src == g1&&dst == g2)
				{
					matrix[index][pos].clear_src_fp(j);
					matrix[index][pos].clear_dst_fp(j);
					if (use_ht)
					{
						ht->decrease(code_s);
						ht->decrease(code_d);
					}
					return true;
				}
			}
			i = (i * 5 + 1071) % bigger_p;
		}
		if (buffer->remove(code_s, code_d) > 0)
		{
			if (use_ht)
			{
				ht->decrease(code_s);
				ht->decrease(code_d);
			}
			return true;
		}
		else
			return false;
	}

	int  query(string s, string d)
	{
		unsigned int hash1 = MurmurHash((const unsigned char*)s.c_str(), s.length()) % range;
		unsigned int hash2 = MurmurHash((const unsigned char*)d.c_str(), d.length()) % range;
		unsigned int h1 = hash1%m;
		int g1 = hash1 / m;
		if (g1 == 0)
			g1++;
		unsigned int h2 = hash2%m;
		int g2 = hash2 / m;
		if (g2 == 0)
			g2++;
		unsigned int code_s = g1*m + h1;
		unsigned int code_d = g2*m + h2;
		int* tmp1 = new int[r];
		int* tmp2 = new int[r];
		for (int i = 0; i<r; i++)
		{
			tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % bigger_p;
			tmp2[i] = ((unsigned long long)g2*multiple[i] + adder[i]) % bigger_p;
		}
		int i = (code_s + code_d) % bigger_p;
		for (int k = 0; k<candidate; k++)
		{
			int index = i % (r*r);
			int index1 = index / r;
			int index2 = index%r;
			int p1 = (h1 + tmp1[index1]) % m;
			int p2 = (h2 + tmp2[index2]) % m;
			int pos = p1*m + p2;

			for (int j = 0; j<room_num; j++){
				int src = matrix[index][pos].get_src_fp(j);
				int dst = matrix[index][pos].get_dst_fp(j);
				if (src == g1&&dst == g2)
					return matrix[index][pos].weight[j];
				else if (src == 0 && dst == 0)
					return 0;
			}
			i = (i * 5 + 1071) % bigger_p;
		}
		return buffer->query(code_s, code_d);
	}
	int query_with_delete(string s, string d)
	{
		unsigned int hash1 = MurmurHash((const unsigned char*)s.c_str(), s.length()) % range;
		unsigned int hash2 = MurmurHash((const unsigned char*)d.c_str(), d.length()) % range;
		unsigned int h1 = hash1%m;
		int g1 = hash1 / m;
		if (g1 == 0)
			g1++;
		unsigned int h2 = hash2%m;
		int g2 = hash2 / m;
		if (g2 == 0)
			g2++;
		unsigned int code_s = g1*m + h1;
		unsigned int code_d = g2*m + h2;

		int* tmp1 = new int[r];
		int* tmp2 = new int[r];
		for (int i = 0; i<r; i++)
		{
			tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % bigger_p;
			tmp2[i] = ((unsigned long long)g2*multiple[i] + adder[i]) % bigger_p;
		}
		bool inserted = false;
		int i = (code_s + code_d) % bigger_p;
		for (int k = 0; k<candidate; k++)
		{
			int index = i % (r*r);
			int index1 = index / r;
			int index2 = index%r;
			int p1 = (h1 + tmp1[index1]) % m;
			int p2 = (h2 + tmp2[index2]) % m;
			int pos = p1*m + p2;


			for (int j = 0; j<room_num; j++){
				int src = matrix[index][pos].get_src_fp(j);
				int dst = matrix[index][pos].get_dst_fp(j);
				if (src == g1&&dst == g2)
					return matrix[index][pos].weight[j];
			}
			i = (i * 5 + 1071) % bigger_p;
		}
		return buffer->query(code_s, code_d);
	}
	void successor(string v, vector<string> &vec)
	{
		if (!ht)
		{
			cout << "cannot answer query without storing the original IDs" << endl;
			return;
		}
		unsigned int hash = MurmurHash((const unsigned char*)v.c_str(), v.length()) % range;
		unsigned int h1 = hash % m;
		int g1 = hash / m;
		if (g1 == 0)
			g1++;
		unsigned int code_v = g1*m + h1;

		if (use_ht){
			if (!ht->check(code_v))
				return;
		}
		int* tmp1 = new int[r];
		for (int i = 0; i < r; i++)
		{
			tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % bigger_p;
		}
		for (int i = 0; i < r*r; i++)
		{
			int index1 = i / r;
			int index2 = i%r;
			int p1 = (h1 + tmp1[index1]) % m;
			int pos = p1*m;
			for (int j = 0; j < m; j++)
			{
				for (int k = 0; k < room_num; k++){
					if (matrix[i][pos + j].get_src_fp(k) == g1)
					{
						int g2 = matrix[i][pos + j].get_dst_fp(k);
						int h2 = j;
						int shifter = (g2*multiple[index2] + adder[index2]) % bigger_p;
						int add = shifter / m;
						h2 += add*m;
						if (h2 < shifter)
							h2 += m;
						h2 -= shifter;
						unsigned int val = g2*m + h2;
						ht->get_ID(val, vec);
					}
				}
			}
		}
		vector<unsigned int> codes;
		buffer->successor(code_v, codes);
		for (int i = 0; i < codes.size(); i++)
			ht->get_ID(codes[i], vec);
	}

	void successor(unsigned int code_v, vector<pair<unsigned int, int>> &vec)
	{
		unsigned int h1 = code_v % m;
		int g1 = code_v / m;

		if (use_ht){
			if (!ht->check(code_v))
				return;
		}

			int* tmp1 = new int[r];
			for (int i = 0; i < r; i++)
			{
				tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % bigger_p;
			}
			for (int i = 0; i < r*r; i++)
			{
					//int count = 0;
					int index1 = i / r;
					int index2 = i%r;
					int p1 = (h1 + tmp1[index1]) % m;
					int pos = p1*m;
					for (int j = 0; j < m; j++)
					{
						for (int k = 0; k < room_num; k++){
							if (matrix[i][pos + j].get_src_fp(k) == g1)
							{
								int g2 = matrix[i][pos + j].get_dst_fp(k);
								int h2 = j;
								int shifter = (g2*multiple[index2] + adder[index2]) % bigger_p;
								int add = shifter / m;
								h2 += add*m;
								if (h2 < shifter)
									h2 += m;
								h2 -= shifter;
								unsigned int val = g2*m + h2;
								vec.push_back(make_pair(val, matrix[i][pos + j].weight[k]));
							}
						}
					}
			}
			buffer->weighted_successor(code_v, vec);
	}

	void precursor(string v, vector<string> &vec)
	{
		if (!ht)
		{
			cout << "cannot answer query without storing the original IDs" << endl;
			return;
		}
		unsigned int hash = MurmurHash((const unsigned char*)v.c_str(), v.length()) % range;

		unsigned int h2 = hash % m;
		int g2 = hash / m;
		if (g2 == 0)
			g2++;
		unsigned int code_v = g2*m + h2;
		if (use_ht){
			if (!ht->check(code_v))
				return;
		}
		int* tmp2 = new int[r];
		for (int i = 0; i < r; i++)
		{
			tmp2[i] = ((unsigned long long)g2*multiple[i] + adder[i]) % bigger_p;
		}
		for (int i = 0; i < r*r; i++)
		{
			int index1 = i / r;
			int index2 = i%r;
			int p2 = (h2 + tmp2[index2]) % m;
			for (int j = 0; j < m; j++)
			{
				int pos = p2 + j*m;
				for (int k = 0; k < room_num; k++){
					if (matrix[i][pos].get_dst_fp(k) == g2)
					{
						int g1 = matrix[i][pos].get_src_fp(k);
						int h1 = j;
						int shifter = (g1*multiple[index1] + adder[index1]) % bigger_p;
						int add = shifter / m;
						h1 += add*m;
						if (h1 < shifter)
							h1 += m;
						h1 -= shifter;
						unsigned int val = g1*m + h1;
						ht->get_ID(val, vec);
					}
				}
			}
		}
		vector<unsigned int> codes;
		buffer->precursor(code_v, codes);
		for (int i = 0; i < codes.size(); i++)
			ht->get_ID(codes[i], vec);
	}

	int SuccessorValueQuery(string s1) // s1 is the ID of the queried node, function for node query.
	{
		int weight = 0;
		unsigned int hash = MurmurHash((const unsigned char*)s1.c_str(), s1.length()) % range;
		unsigned int h1 = hash % m;
		int g1 = hash / m;
		if (g1 == 0)
			g1++;
		unsigned int code_v = g1*m + h1;

		int* tmp1 = new int[r];
		for (int i = 0; i < r; i++)
		{
			tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % bigger_p;
		}

		for (int i = 0; i < r*r; i++)
		{
			int index1 = i / r;
			int index2 = i%r;
			int p1 = (h1 + tmp1[index1]) % m;
			int pos = p1*m;
			for (int j = 0; j < m; j++)
			{
				for (int k = 0; k < room_num; k++){
					if (matrix[i][pos + j].get_src_fp(k) == g1)
					{
						weight += matrix[i][pos + j].weight[k];
					}
				}
			}
		}
		weight += buffer->successor_value(code_v);
		return weight;

	}

	int PrecursorValueQuery(string s2) // s1 is the ID of the queried node, function for node query.
	{
		int weight = 0;
		unsigned int hash = MurmurHash((const unsigned char*)s2.c_str(), s2.length()) % range;
		unsigned int h2 = hash % m;
		int g2 = hash / m;
		if (g2 == 0)
			g2++;
		unsigned int code_v = g2*m + h2;

		int* tmp2 = new int[r];
		for (int i = 0; i < r; i++)
		{
			tmp2[i] = ((unsigned long long)g2*multiple[i] + adder[i]) % bigger_p;
		}

		for (int i = 0; i < r*r; i++)
		{
			int index1 = i / r;
			int index2 = i%r;
			int p2 = (h2 + tmp2[index2]) % m;
			for (int j = 0; j < m; j++)
			{
				int pos = p2 + j*m;
				for (int k = 0; k < room_num; k++){
					if (matrix[i][pos].get_dst_fp(k) == g2)
					{
						weight += matrix[i][pos].weight[k];
					}
				}
			}
		}
		weight += buffer->precursor_value(code_v);
		return weight;
	}


	bool reach_query(string s1, string s2) // s1 is the ID of the source node, s2 is the ID of the destination node, return whether reachable.
	{
		unsigned int hash1 = MurmurHash((const unsigned char*)s1.c_str(), s1.length()) % range;
		unsigned int hash2 = MurmurHash((const unsigned char*)s2.c_str(), s2.length()) % range;
		unsigned int h1 = hash1 % m;
		int g1 = hash1 / m;
		if (g1 == 0)
			g1++;
		unsigned int code_s = g1*m + h1;

		unsigned int h2 = hash2 % m;
		int g2 = hash2 / m;
		if (g2 == 0)
			g2++;
		unsigned int code_d = g2*m + h2;

		int pos;
		unordered_map<unsigned int, bool> checked;
		queue<unsigned int> q;
		q.push(code_s);
		checked[code_s] = true;
		unordered_map<unsigned int, bool>::iterator	IT;

		while (!q.empty())
		{
			unsigned int code_v = q.front();
			q.pop();
			g1 = code_v / m;
			h1 = code_v % m;
			int* tmp1 = new int[r];
			for (int i = 0; i < r; i++)
			{
				tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % bigger_p;
			}
			for (int i = 0; i < r*r; i++)
			{
				int index1 = i / r;
				int index2 = i%r;
				int p1 = (h1 + tmp1[index1]) % m;
				int pos = p1*m;
				for (int j = 0; j < m; j++)
				{
					for (int k = 0; k < room_num; k++){
						if (matrix[i][pos + j].get_src_fp(k) == g1)
						{
							int g2 = matrix[i][pos + j].get_dst_fp(k);
							int h2 = j;
							int shifter = (g2*multiple[index2] + adder[index2]) % bigger_p;
							int add = shifter / m;
							h2 += add*m;
							if (h2 < shifter)
								h2 += m;
							h2 -= shifter;
							unsigned int val = g2*m + h2;
							if (val == code_d)
								return true;
							if (!checked[val]) {
								checked[val] = true;
								q.push(val);
							}
						}
					}
				}
			}
			vector<unsigned int> codes;
			buffer->successor(code_v, codes);
			for (int i = 0; i < codes.size(); i++)
			{
				if (codes[i] == code_d)
					return true;
				if (!checked[codes[i]]) {
					checked[codes[i]] = true;
					q.push(codes[i]);
				}
			}
		}

		return false;
	}

	void recover(icl_graph* g)
	{
		for (int i = 0; i < r*r; i++)
		{
			int index1 = i / r;
			int index2 = i%r;
			for (int j = 0; j < m; j++)
			{
				for (int k = 0; k < m; k++)
				{
					int pos = j*m + k;

					for (int l = 0; l < room_num; l++)
					{
						int g1 = matrix[i][pos].get_src_fp(l);
						int g2 = matrix[i][pos].get_dst_fp(l);
						if(g1==0||g2==0)
							continue;

						int h1 = j;
						int shifter = (g1*multiple[index1] + adder[index1]) % bigger_p;
						int add = shifter / m;
						h1 += add*m;
						if (h1 < shifter)
							h1 += m;
						h1 -= shifter;
						unsigned int val1 = g1*m + h1;



						int h2 = k;
						shifter = (g2*multiple[index2] + adder[index2]) % bigger_p;
						add = shifter / m;
						h2 += add*m;
						if (h2 < shifter)
							h2 += m;
						h2 -= shifter;
						unsigned int val2 = g2*m + h2;

						g->new_insert(val1, val2, matrix[i][pos].weight[l]);

					}
				}

			}

		}
		vector<weighted_edge> vec;
		buffer->get_edges(vec);
		for (int i = 0; i < vec.size(); i++)
			g->new_insert(vec[i].src, vec[i].dst, vec[i].weight);
		return;
	}

	void SSSP_compute(unsigned int root, unordered_map<unsigned int, int>& distance)
	{
		priority_queue<node_distance, vector<node_distance>, cmp> q;
		q.push(node_distance(root, 0));
		while (!q.empty()) {
			node_distance nd = q.top();
			q.pop();
			unsigned int v = nd.id;
			if (distance.find(v) != distance.end())
				continue;
			distance[v] = nd.distance;

			vector<pair<unsigned int, int>> vec;
			successor(v, vec);
			for (int i = 0; i < vec.size(); i++)
			{
				unsigned int d = vec[i].first;
				int w = vec[i].second;
				q.push(node_distance(d, w + nd.distance));
			}
		}
		return;
	}
	void SSSP(string root, unordered_map<string, int>& distance)
	{
		if (!ht)
		{
			cout << "cannot answer query without storing the original IDs" << endl;
			return;
		}
		unsigned int hash = MurmurHash((const unsigned char*)root.c_str(), root.length()) % range;
		unsigned int h1 = hash % m;
		int g1 = hash / m;
		if (g1 == 0)
			g1++;
		unsigned int code_r = g1 * m + h1;
		unordered_map<unsigned int, int> coded_distance;
		SSSP_compute(code_r, coded_distance);
		unordered_map<unsigned int, int>::iterator it;
		for (it = coded_distance.begin(); it != coded_distance.end(); it++)
		{
			vector<string> s_vec;
			ht->get_ID(it->first, s_vec);
			for (int i = 0; i < s_vec.size(); i++)
			{
				distance[s_vec[i]] = it->second;
			}
			s_vec.clear();
			vector<string>().swap(s_vec);
		}
		coded_distance.clear();
	}

};




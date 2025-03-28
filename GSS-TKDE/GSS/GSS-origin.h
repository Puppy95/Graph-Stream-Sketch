#include<iostream>
#include<vector>
#include<map>
#include<memory.h> 
#include<algorithm> 
#include<algorithm>
#include "setting.h"
#include "../common/hashtable.h"
#include<set>
#include<string>
#include<time.h>
#include <chrono>
using namespace std;

// code for the original version of GSS, with no node bitmap or block partition. room number in each bucket, fingerprint length, and type of buffer can be changed in setting.h

// note that we use string as node IDs in the code. If the graph data has been prepocessed and the node ID is already integers, we can also
// change the data type of the node ID into unsigned int.  

class GSS
{
public:
	bucket_with_index* matrix; // matrix
	int r;
	int m;
	int range; // length of address sequence for each node
	int candidate;  // number of candidate bucket for each edge
	my_buffer* buffer; // buffer
	bool use_ht; // indicating if we use a hashtable to store original node IDs
	hashtable *ht; // hash table
	unsigned long long multiple[16]; // the array of multple and adder is used to produce address sequence in a pseudo-random procedure.
	unsigned long long adder[16];
	GSS(unsigned int matrix_size_, int r_, int candidate_, bool use_hashtable, int fp_size = fp_len, int table_size = initial_size) // the hash table size is set approximately to node number in the streamin graph.
	{
		// the above parameters are length of the matrix, length of the address sequence for each node, number of candidate buckets, whether using hashtable to  
		r = r_;
		candidate = candidate_;
		m = sqrt(matrix_size_ / room_num) + 1;
		range = pow(2, fp_size)*m;
		use_ht = use_hashtable;
		if (use_hashtable)
			ht = new hashtable(table_size);
		else
			ht = new hashtable(initial_size);
		matrix = new bucket_with_index[m*m];
		for (int i = 0; i<m*m; i++)
		    matrix[i].clear();
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
		delete[]matrix;
		delete buffer;
		delete ht;
	}

	int get_mem()
	{
		int mem = 8 * 32 + 4 * 4 + 8 + 8 + 8 + 1;
		mem += matrix[0].get_mem() * m * m;
		if (buffer)
			mem += buffer->get_mem();
		if (ht)
			mem += ht->get_mem();
		return mem;
	}
	
	int get_edge_num() // get the number of edges in GSS, note that we count the graph sketch edge here, namely edges afer hash map.
	{
		int count = 0;
		for (unsigned int j = 0; j < m; j++)
		{
			for (unsigned int k = 0; k < m; k++)
			{
				int pos = j*m + k;
				for (int l = 0; l < room_num; l++)
				{
					int g1 = matrix[pos].get_src_fp(l);
					int g2 = matrix[pos].get_dst_fp(l);
						
					if(g1==0||g2==0)
						continue;
					else
						count++;					// count the number of unempty bucket
				}
			}
		}
		return count+buffer->get_edge_num(); // add the number of edges in the buffer to the count
	}
	int get_node_num()
	{
		return ht->get_num();
	}

	int get_buffer_size()
	{
		return buffer->get_edge_num();
	}
	void insert(string s, string d, int w) // default edge insertion function. It can be used when there is only edge insertions in the streaming graph.
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
		int tmp1[64];
		int tmp2[64];
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
			int ind = (index1 << 4) + index2;
			for (int j = 0; j < room_num; j++){
				int src = matrix[pos].get_src_fp(j);
				int dst = matrix[pos].get_dst_fp(j);
				if (src == g1&&dst == g2&&matrix[pos].compare_ind(j, ind))
				{
					matrix[pos].weight[j] += w;
					return;
				}
				else if (src == 0 && dst == 0)
				{
					matrix[pos].set_src_fp(j, g1);
					matrix[pos].set_dst_fp(j, g2);
					matrix[pos].weight[j] = w;
					matrix[pos].set_ind(j, ind);
					return;
				}
			}
			i = (i * 5 + 1071) % bigger_p;
		}
		int tmp = buffer->insert(code_s, code_d, w);
	}

	void insert_with_delete(string s, string d, int w) // insert function used in the situation when there are both edge insertions and edge deletions. Different from 
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
		
		if (use_ht&&w>0){ // only insert node ID into the hashtable if this is an edge insertion.
			ht->insert(code_s, s);
			ht->insert(code_d, d);
		}

		int tmp1[64];
		int tmp2[64];
		for (int i = 0; i<r; i++)
		{
			tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % large_p;
			tmp2[i] = ((unsigned long long)g2*multiple[i] + adder[i]) % large_p;
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
			int ind = (index1 << 4) + index2;
			for (int j = 0; j<room_num; j++){
				int src = matrix[pos].get_src_fp(j);
				int dst = matrix[pos].get_dst_fp(j);
				if (src == g1&&dst == g2&&matrix[pos].compare_ind(j, ind))
				{
					matrix[pos].weight[j] += w;
					if (matrix[pos].weight[j] <= 0)
					{
						matrix[pos].clear_src_fp(j);
						matrix[pos].clear_dst_fp(j);
						matrix[pos].clear_ind(j);
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
						empty_index = ind;
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
				matrix[empty_pos].set_src_fp(empty_room, g1);
				matrix[empty_pos].set_dst_fp(empty_room, g2);
				matrix[empty_pos].weight[empty_room] = new_w;
				matrix[empty_pos].set_ind(empty_room, empty_index);

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
		int tmp1[64];
		int tmp2[64];
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
			int ind = (index1 << 4) + index2;

			for (int j = 0; j<room_num; j++){
				int src = matrix[pos].get_src_fp(j);
				int dst = matrix[pos].get_dst_fp(j);
				if (src == g1&&dst == g2&&matrix[pos].compare_ind(j, ind))
				{
					matrix[pos].clear_src_fp(j);
					matrix[pos].clear_dst_fp(j);
					matrix[pos].clear_ind(j);
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
		int tmp1[64];
		int tmp2[64];
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
			int ind = (index1 << 4) + index2;

			for (int j = 0; j<room_num; j++){
				int src = matrix[pos].get_src_fp(j);
				int dst = matrix[pos].get_dst_fp(j);
				if (src == g1&&dst == g2&&matrix[pos].compare_ind(j, ind))
					return matrix[pos].weight[j];
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

		int tmp1[64];
		int tmp2[64];
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
			int ind = (index1 << 4) + index2;

			for (int j = 0; j<room_num; j++){
				int src = matrix[pos].get_src_fp(j);
				int dst = matrix[pos].get_dst_fp(j);
				if (src == g1&&dst == g2&&matrix[pos].compare_ind(j, ind))
					return matrix[pos].weight[j];
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
		int tmp1[64];
		for (int i = 0; i < r; i++)
		{
			tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % bigger_p;
		}
		for (int i = 0; i < r; i++)
		{
			int p1 = (h1 + tmp1[i]) % m;
			int pos = p1*m;
			for (int j = 0; j < m; j++)
			{
				for (int k = 0; k < room_num; k++){
					if (matrix[pos + j].get_src_fp(k) == g1&&matrix[pos+j].get_src_ind(k)==i)
					{
						int g2 = matrix[pos + j].get_dst_fp(k);
						int h2 = j;
						int index2 = matrix[pos + j].get_dst_ind(k);
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

		int tmp1[64];
		for (int i = 0; i < r; i++)
		{
			tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % bigger_p;
		}
		for (int i = 0; i < r; i++)
		{
			int p1 = (h1 + tmp1[i]) % m;
			int pos = p1*m;
			for (int j = 0; j < m; j++)
			{
				for (int k = 0; k < room_num; k++){
					if (matrix[pos + j].get_src_fp(k) == g1&&matrix[pos + j].get_src_ind(k) == i)
					{
						int g2 = matrix[pos + j].get_dst_fp(k);
						int h2 = j;
						int index2 = matrix[pos + j].get_dst_ind(k);
						int shifter = (g2*multiple[index2] + adder[index2]) % bigger_p;
						int add = shifter / m;
						h2 += add*m;
						if (h2 < shifter)
							h2 += m;
						h2 -= shifter;
						unsigned int val = g2*m + h2;
						vec.push_back(make_pair(val, matrix[pos + j].weight[k]));
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
		int tmp2[64];
		for (int i = 0; i < r; i++)
		{
			tmp2[i] = ((unsigned long long)g2*multiple[i] + adder[i]) % bigger_p;
		}
		for (int i = 0; i < r; i++)
		{
			int p2 = (h2 + tmp2[i]) % m;
			for (int j = 0; j < m; j++)
			{
				int pos = p2 + j*m;
				for (int k = 0; k < room_num; k++){
					if (matrix[pos].get_dst_fp(k) == g2&&matrix[pos].get_dst_ind(k)==i)
					{
						int g1 = matrix[pos].get_src_fp(k);
						int h1 = j;
						int index1 = matrix[pos].get_src_ind(k);
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

		int tmp1[64];
		for (int i = 0; i < r; i++)
		{
			tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % bigger_p;
		}

		for (int i = 0; i < r; i++)
		{
			int p1 = (h1 + tmp1[i]) % m;
			int pos = p1*m;
			for (int j = 0; j < m; j++)
			{
				for (int k = 0; k < room_num; k++){
					if (matrix[pos + j].get_src_fp(k) == g1 &&matrix[pos +j].get_src_ind(k)==i)
					{
						weight += matrix[pos + j].weight[k];
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

		int tmp2[64];
		for (int i = 0; i < r; i++)
		{
			tmp2[i] = ((unsigned long long)g2*multiple[i] + adder[i]) % bigger_p;
		}

		for (int i = 0; i < r; i++)
		{
			int p2 = (h2 + tmp2[i]) % m;
			for (int j = 0; j < m; j++)
			{
				int pos = p2 + j*m;
				for (int k = 0; k < room_num; k++){
					if (matrix[pos].get_dst_fp(k) == g2&&matrix[pos].get_dst_ind(k)==i)
					{
						weight += matrix[pos].weight[k];
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
			int tmp1[64];
			for (int i = 0; i < r; i++)
			{
				tmp1[i] = ((unsigned long long)g1*multiple[i] + adder[i]) % bigger_p;
			}
			for (int i = 0; i < r; i++)
			{
				int p1 = (h1 + tmp1[i]) % m;
				int pos = p1*m;
				for (int j = 0; j < m; j++)
				{
					for (int k = 0; k < room_num; k++){
						if (matrix[pos + j].get_src_fp(k) == g1&&matrix[pos+j].get_src_ind(k)==i)
						{
							int g2 = matrix[pos + j].get_dst_fp(k);
							int h2 = j;
							int index2 = matrix[pos + j].get_dst_ind(k);
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
			g->resize(ht->w);
			for (unsigned int j = 0; j < m; j++)
			{
				for (unsigned int k = 0; k < m; k++)
				{
					int pos = j*m + k;

					for (int l = 0; l < room_num; l++)
					{
						int g1 = matrix[pos].get_src_fp(l);
						int g2 = matrix[pos].get_dst_fp(l);
						
						if(g1==0||g2==0)
							continue;
							
						int index1 = matrix[pos].get_src_ind(l);
						int index2 = matrix[pos].get_dst_ind(l);
						

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

						g->new_insert(val1, val2, matrix[pos].weight[l]);

					}
				}

			}
		vector<weighted_edge> vec;
		buffer->get_edges(vec);
		for (int i = 0; i < vec.size(); i++)
			g->new_insert(vec[i].src, vec[i].dst, vec[i].weight);
		vec.clear();
		vector<weighted_edge>().swap(vec);
		return;
	}
	
		void allNodeNeighbor(string filename, vector<string> &querynodes, int type)
	{
		icl_graph* g = new icl_graph();
		recover(g);
		
		ofstream fout(filename.c_str());
		cout<<querynodes.size()<<endl;
		for(int i=0;i<querynodes.size();i++)
		{
		string s1 = querynodes[i];
		unsigned int hash = MurmurHash((const unsigned char*)s1.c_str(), s1.length()) % range;
		unsigned int h1 = hash % m;
		int g1 = hash / m;
		if (g1 == 0)
			g1++;
		unsigned int code_v = g1*m + h1;
		vector<unsigned int> codes;
		vector<string> IDs;
		if(type==0) // successor 
			g->successor(code_v, codes);
		else // precursor
			g->precursor(code_v, codes);
		for(int j = 0; j < codes.size(); j++)
			ht->get_ID(codes[j], IDs);
		codes.clear();
		vector<unsigned int>().swap(codes);
		fout<<s1<<' '<<IDs.size()<<endl;
	//	for(int j=0;j<IDs.size();j++)
		//	fout<<IDs[j]<<' ';
		//fout<<endl;
		IDs.clear();
		vector<string>().swap(IDs);
		}
		fout.close();
		delete g;
	}
		
		void BatchReachability(string filename, vector<string> &srcs, vector<string> &dsts, int step=-1)
		{
			icl_graph* g = new icl_graph();
			recover(g);
			ofstream fout(filename.c_str());
			for(int i=0;i<srcs.size();i++)
		{
		string s1 = srcs[i];
		string s2 = dsts[i];
		unsigned int hash1 = MurmurHash((const unsigned char*)s1.c_str(), s1.length()) % range;
		unsigned int h1 = hash1 % m;
		int g1 = hash1 / m;
		if (g1 == 0)
			g1++;
		unsigned int code_s = g1*m + h1;
		
		unsigned int hash2 = MurmurHash((const unsigned char*)s2.c_str(), s2.length()) % range;
		unsigned int h2 = hash2 % m;
		int g2 = hash2 / m;
		if (g2 == 0)
			g2++;
		unsigned int code_d = g2*m + h2;
		
		fout<<s1<<' '<<s2<<' '<<g->reach_query(code_s, code_d, step)<<endl;
		}
			fout.close();
			delete g;
			
		}
		
		
	
		void SSSP_rebuild(string root, unordered_map<string, int>& distance)
		{
			icl_graph* g = new icl_graph();
			recover(g);
			unsigned int hash = MurmurHash((const unsigned char*)root.c_str(), root.length()) % range;
			unsigned int h1 = hash % m;
			int g1 = hash / m;
			if (g1 == 0)
				g1++;
			unsigned int code_r = g1 * m + h1;
			unordered_map<unsigned int, int> coded_distance;
			g->SSSP(code_r, coded_distance);
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
			delete g;

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




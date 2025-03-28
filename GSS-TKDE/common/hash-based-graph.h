#pragma once
#include <iostream>
#include <unordered_map>
#include <stdlib.h>
#include<map>
#include <assert.h>
using namespace std;

class edge_row
{
public:
	unordered_map<unsigned int, int> edges;
	edge_row(){}
	~edge_row()
	{
		edges.clear();
	}
	int get_mem()
	{
		return edges.bucket_count() * 8 + edges.size() * 16 + sizeof(unordered_map<unsigned int, int>);
	}
	int query(int dst)
	{
		unordered_map<unsigned int, int>::iterator it = edges.find(dst);
		if (it != edges.end())
		{
			return it->second;
		}
		else
			return 0;
	}
	int insert_with_delete(unsigned int dst, int weight)
	{
		unordered_map<unsigned int, int>::iterator it = edges.find(dst);
		if (it != edges.end())
		{
			it->second += weight;
			if (it->second <= 0)
			{
				edges.erase(it);
				return delete_edge;
			}
			else
			{
				return modify_edge;
			}
		}
		else
		{
			edges[dst] = weight;
			return add_edge;
		}
	}
	int insert(unsigned int dst, int weight)
	{
		unordered_map<unsigned int, int>::iterator it = edges.find(dst);
		if (it != edges.end())
		{
			it->second += weight;
			return modify_edge;
		}
		else
		{
			edges[dst] = weight;
			return add_edge;
		}
	}
	int remove(unsigned int dst)
	{
		unordered_map<unsigned int, int>::iterator it = edges.find(dst);
		if (it != edges.end())
		{
			int tmp = it->second;
			edges.erase(it);
			return tmp;
		}
		else
			return 0;

	}

	void scan(vector<unsigned int> &neighbors)
	{
		unordered_map<unsigned int, int>::iterator it;
		for (it = edges.begin(); it != edges.end(); it++)
		{
			neighbors.push_back(it->first);
		}
	}
	void weighted_scan(vector<pair<unsigned int, int> > &neighbor_weights)
	{
		unordered_map<unsigned int, int>::iterator it;
		for (it = edges.begin(); it != edges.end(); it++)
		{
			neighbor_weights.push_back(make_pair(it->first, it->second));
		}

	}
	int weight_sum()
	{
		int sum = 0;
		unordered_map<unsigned int, int>::iterator it;
		for (it = edges.begin(); it != edges.end(); it++)
		{
			sum += it->second;
		}
		return sum;

	}
	int edge_count()
	{
		return edges.size();
	}
	int total_space()
	{
		return edges.bucket_count();
	}
};

class half_hb_graph
{
public:
	unordered_map<unsigned int, int> node_map;
	vector<edge_row*> list;
	int N;
	half_hb_graph(){ N = 0; }
	~half_hb_graph()
	{
		for (int i = 0; i < list.size(); i++)
			delete list[i];
		node_map.clear();
		list.clear();
	}
	int get_mem()
	{
		unsigned int total = node_map.bucket_count() * 8 + node_map.size() * 16 + sizeof(unordered_map<unsigned int, int>);
		total += list.capacity() * 8 + sizeof(vector<edge_row*>);
		for (int i = 0; i < list.size(); i++)
			total += list[i]->get_mem();
		total += 4;
		return total;
	}
	int query(unsigned int src, unsigned int dst)
	{
		unordered_map<unsigned int, int>::iterator it = node_map.find(src);
		if (it != node_map.end())
		{
			return list[it->second]->query(dst);
		}
		else
			return 0;
	}
	int insert_with_delete(unsigned int src, unsigned int dst, int weight)
	{
		unordered_map<unsigned int, int>::iterator it = node_map.find(src);
		if (it != node_map.end())
		{
			int tmp = list[it->second]->insert_with_delete(dst, weight);
			if (tmp == add_edge)
				N++;
			else if (tmp == delete_edge)
				N--;
			return tmp;
		}
		else
		{
			assert(weight > 0);
			edge_row* tmp = new edge_row();
			tmp->insert(dst, weight);
			node_map[src] = list.size();
			list.push_back(tmp);
			N++;
			return add_edge;
		}
	}

	int insert(unsigned int src, unsigned int dst, int weight)
	{
		unordered_map<unsigned int, int>::iterator it = node_map.find(src);
		if (it != node_map.end())
		{
			int tmp = list[it->second]->insert_with_delete(dst, weight);
			if (tmp == add_edge)
				N++;
			return tmp;
		}
		else
		{
			assert(weight > 0);
			edge_row* tmp = new edge_row();
			tmp->insert(dst, weight);
			node_map[src] = list.size();
			list.push_back(tmp);
			N++;
			return add_edge;
		}

	}


	int remove(unsigned int src, unsigned int dst)
	{
		unordered_map<unsigned int, int>::iterator it = node_map.find(src);
		if (it != node_map.end())
		{
			int tmp = list[it->second]->remove(dst);
			if (tmp != 0)
				N--;
			return tmp;
		}
		else
			return 0;

	}
	void scan(unsigned int src, vector<unsigned int>& neighbors)
	{
		unordered_map<unsigned int, int>::iterator it = node_map.find(src);
		if (it != node_map.end())
		{
			return list[it->second]->scan(neighbors);
		}
		else
			return;
	}
	void weighted_scan(unsigned int src, vector<pair<unsigned int, int>> &neighbor_weights)
	{
		unordered_map<unsigned int, int>::iterator it = node_map.find(src);
		if (it != node_map.end())
		{
			return list[it->second]->weighted_scan(neighbor_weights);
		}
		else
			return;

	}
	int weight_sum(unsigned int src)
	{
		unordered_map<unsigned int, int>::iterator it = node_map.find(src);
		if (it != node_map.end())
		{
			return list[it->second]->weight_sum();
		}
		else
			return 0;

	}
	void get_edges(vector<weighted_edge>& vec)
	{
		unordered_map<unsigned int, int>::iterator it;
		for (it = node_map.begin(); it != node_map.end(); it++)
		{
			int src = it->first;
			unordered_map<unsigned int, int>::iterator eit;
			for (eit = list[it->second]->edges.begin(); eit != list[it->second]->edges.end(); eit++)
			{
				vec.push_back(weighted_edge(src, eit->first, eit->second));
			}
		}
	}
};

class hb_graph
{
public:
	half_hb_graph* hb;
	half_hb_graph* rhb;
	hb_graph()
	{
		hb = new half_hb_graph;
		rhb = new half_hb_graph;
	}
	~hb_graph()
	{
		delete hb;
		delete rhb;
	}
	int get_mem()
	{
		return hb->get_mem() + rhb->get_mem();
	}
	int insert_with_delete(unsigned int src, unsigned int dst, int weight)
	{
		int tmp1, tmp2;
		tmp1 = hb->insert_with_delete(src, dst, weight);
		tmp2 = rhb->insert_with_delete(dst, src, weight);
		assert(tmp1 == tmp2);
		return tmp1;
	}
	int insert(unsigned int src, unsigned int dst, int weight)
	{
		int tmp1, tmp2;
		tmp1 = hb->insert(src, dst, weight);
		tmp2 = rhb->insert(dst, src, weight);
		assert(tmp1 == tmp2);
		return tmp1;
	}
	int remove(unsigned int src, unsigned int dst)
	{
		int tmp = hb->remove(src, dst);
		if (tmp == 0)
			return tmp;
		else
			return rhb->remove(dst, src);
	}
	int query(unsigned int src, unsigned int dst)
	{
		return hb->query(src, dst);
	}
	void successor(unsigned int v, vector<unsigned int> &neighbors)
	{
		hb->scan(v, neighbors);
	}
	void precursor(unsigned int v, vector<unsigned int> &neighbors)
	{
		rhb->scan(v, neighbors);
	}
	void weighted_successor(unsigned int v, vector<pair<unsigned int, int>> &neighbors)
	{
		hb->weighted_scan(v, neighbors);
	}
	void weighted_precursor(unsigned int v, vector<pair<unsigned int, int>> &neighbors)
	{
		rhb->weighted_scan(v, neighbors);
	}

	int successor_value(unsigned int v)
	{
		return hb->weight_sum(v);
	}
	
	int precursor_value(unsigned int v)
	{
		return rhb->weight_sum(v);
	}
	void get_edges(vector<weighted_edge> &vec)
	{
		hb->get_edges(vec);
		return;
	}
	int get_edge_num()
	{
		return hb->N;
	}

};
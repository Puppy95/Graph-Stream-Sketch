#include<iostream>
#include<string>
#include<fstream>
#include<map>
#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<queue>
#ifndef Hash
#include "../common/hashfunction.h"
#define Hash
#endif

// linked list for streaming graph, note that we only store the out edge list in a list_graph structure. In order to support both successor query and precursor query, 
// 2 list_graphs are needed, with one storing the reverse graph.
using namespace std;
struct list_edge
{
	string n; // node ID for the dst node
	list_edge* next; // pointer to maintain the edge list
	int weight; // weight of the edge;
	list_edge(string id = "", int w = 0)
	{
		n = id;
		weight = w;
		next = NULL;
	}
};
struct list_node
{
	string n; // node ID 
	list_edge* head; // head pointed to the successor edge list
	list_node* next; // pointer to maintain the node list in the hash table
	list_node(string id = "")
	{
		n = id;
		head = NULL;
		next = NULL;
	}
};

struct string_distance
{
	string id;
	int distance;
	string_distance(string id_, int distance_)
	{
		id = id_;
		distance = distance_;
	}
};

struct strdis_cmp
{
	bool operator()(string_distance n1, string_distance n2)
	{
		if (n1.distance < 0)
			return true;
		else if (n2.distance < 0)
			return false;
		else
			return n1.distance > n2.distance;
	}
};

class list_graph
{
private:
	int n; // node number 
	int N; // edge number 
	int table_size; // table size
public:
	list_node** g; // pointer to the hash table of list_node*
	list_graph(int size) // size is the parameter for hash table size. 
	{ 
		table_size = size;
		g = new list_node*[table_size];
		for (int i = 0; i < table_size; i++)
			g[i] = NULL;
	}
	~list_graph(){
		for (int i = 0; i<table_size; i++)
		{
			if(g[i]){
			list_edge* tmp = g[i]->head;
			list_edge* next = tmp;
			while (tmp) // delete edges in the dge list
			{
				next = tmp->next;
				delete tmp;
				tmp = next;
			}
			delete g[i]; // delete the node 
			}
		}
		delete[] g;
	}
	int get_mem()
	{
		int mem = 20 + table_size * 8;
		for (int i = 0; i < table_size; i++)
		{
			list_node* tmp = g[i];
			while (tmp)
			{
				mem += sizeof(string) + 16;
				list_edge* cur = tmp->head;
				while (cur)
				{
					mem += sizeof(string) + 12;
					cur = cur->next;
				}
				tmp = tmp->next;
			}
		}
		return mem;
	}
	void insert(string s1, string s2, int weight); // add a new edge (s1, s2) with weight
	int query(string s1, string s2); // query the weight of edge (s1, s2)
	int	get_nodenum(); // get the number of node
	int get_edgenum(); // get the number of edge
	void successor(string v, vector<string> &vec); // query for the successors of a node 
	int nodequery(string v); // get the aggregation of the weight of v's out-edges.
	bool reach_query(string s, string d);
	void SSSP(string root, unordered_map<string, int>& distance);
};
int list_graph::get_nodenum()
{
	return n;
}
int list_graph::get_edgenum()
{
	return N;
}
void list_graph::insert(string s1, string s2, int w)
{
	int n1 = MurmurHash((const unsigned char*)s1.c_str(), s1.length()) % table_size; // hash map
	list_node* tmp = g[n1];
	while (tmp)
	{
		if (tmp->n == s1) // find the edge list of s1
		{
			list_edge* edge_cur = tmp->head;
			while (edge_cur)
			{
				if (edge_cur->n == s2) // find edge (s1, s2)
				{
					edge_cur->weight += w;
					return;
				}
				edge_cur = edge_cur->next;
			}
			N++; // add a new edge
			list_edge* new_edge = new list_edge(s2, w);
			new_edge->next = tmp->head;
			tmp->head = new_edge;
			return;
		}
		tmp = tmp->next;
	}
	n++;  // add a new edge and a new node
	N++;
	list_node* new_node = new list_node(s1);
	list_edge* new_edge = new list_edge(s2, w);
	new_node->head = new_edge;
	new_node->next = g[n1];
	g[n1] = new_node;

}
int list_graph::query(string s1, string s2) // query the weight of an edge 
{
	int n1 = MurmurHash((const unsigned char*)s1.c_str(), s1.length()) % table_size;
	list_node* tmp = g[n1]; // find the node list head with hash function
	while (tmp)
	{
		if (tmp->n == s1) // find the edge list of source node s1
		{
			list_edge* edge_cur = tmp->head;
			while (edge_cur) 
			{
				if (edge_cur->n == s2) // find the edge
				{
					return edge_cur->weight;
				}
				edge_cur = edge_cur->next;
			}
			break;
		}
		tmp = tmp->next;
	}
	return 0;
}

void list_graph::successor(string v, vector<string> &vec) // get the successor list of a node 
{
	int n1 = MurmurHash((const unsigned char*)v.c_str(), v.length()) % table_size;
	list_node* tmp = g[n1];
	while (tmp)
	{
		if (tmp->n == v)
		{
			list_edge* edge_cur = tmp->head;
			while (edge_cur)
			{
				vec.push_back(edge_cur->n);
				edge_cur = edge_cur->next;
			}
			break;
		}
		tmp = tmp->next;
	}
	return;
}

int list_graph::nodequery(string v) // get the sum of weights of the out edges of node v.
{
	int sum = 0;
	int n1 = MurmurHash((const unsigned char*)v.c_str(), v.length()) % table_size;
	list_node* tmp = g[n1];
	while (tmp)
	{
		if (tmp->n == v)
		{
			list_edge* edge_cur = tmp->head;
			while (edge_cur)
			{
				sum += edge_cur->weight;
				edge_cur = edge_cur->next;
			}
			break;
		}
		tmp = tmp->next;
	}
	return sum;

}

bool list_graph::reach_query(string s, string d)
{
	queue<string> q;
	q.push(s);
	unordered_set<string> visited;
	visited.insert(s);
	while (!q.empty())
	{
		string v = q.front();
		q.pop();
		if (v == d)
			return true;
		vector<string> vec;
		successor(v, vec);
		for (int i = 0; i < vec.size(); i++)
		{
			if (visited.find(vec[i]) == visited.end()) {
				q.push(vec[i]);
				visited.insert(vec[i]);
			}
		}
	}
	return false;
}
void list_graph::SSSP(string root, unordered_map<string, int>& distance)
{
	priority_queue<string_distance, vector<string_distance>, strdis_cmp> q;
	q.push(string_distance(root, 0));
	while (!q.empty()) {
		string_distance nd = q.top();
		q.pop();
		string v = nd.id;
		if (distance.find(v) != distance.end())
			continue;
		distance[v] = nd.distance;


		int n1 = MurmurHash((const unsigned char*)v.c_str(), v.length()) % table_size;
		list_node* tmp = g[n1];
		while (tmp)
		{
			if (tmp->n == v)
			{
				list_edge* edge_cur = tmp->head;
				while (edge_cur)
				{
					q.push(string_distance(edge_cur->n, edge_cur->weight + nd.distance));
					edge_cur = edge_cur->next;
				}
				break;
			}
			tmp = tmp->next;
		}
	}
	return;
}

#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "../common/hashtable.h"
#include "../common/icl_graph.h"
#endif // HASHTABLE_H
#include<math.h>
#include<string>
#include<iostream>
#include<memory.h>
#include<queue>
#define insert_ht true
using namespace std;

class TCM
{
private:
    unsigned int width;
    int hashnum;
    int **value;
    bool usetable;
    int tablesize;
public:
	hashtable* mapTable; 
    TCM(unsigned int w,int h, bool usetable, int tablesize=0);
    ~TCM()
    {
		for (int i=0; i<hashnum; ++i)
        {
                delete [] value[i];
        }
        delete [] value;
        delete [] mapTable;
    }
	int get_mem()
	{
		int mem = 16 + 8 + 1 + 8 + hashnum * 8;
		mem += width * width * hashnum * 4;
		if (mapTable)
			mem += mapTable->get_mem();
		return mem;
	}
    void insert(string v1, string v2,int weight); // insert an edge <v1, v2> with weight
    int query(string v1,string v2); // query the weight of a edge <v1, v2>
    int nodequery(string v1, int type); // query the sum of weight of adjacent edges of v1, type indicating successor or precursor, 0 means successor, 1 means precursor.
	int nodedegreequery(string v1, int type); // query the number of adjacent edges, namely degree of v1, 0 means successor, 1 means precursor. Note that in this function, we count 
	//degree in each tcm sketch, and get the minimum value of the reported results. 
    bool reach_query(string v1, string v2); // query the reachablility between node v1 and v2.
    void recover(icl_graph** g); // transform each matrix of TCM into a adjacency list, 
    void recover(icl_graph* g, int hash); // transform a specified matrix into an adjacency list. 
    void get_neighbor(string v1, vector<string>& vec, int type); // get the neighbor set of a node v1,  0 means successor, 1 means precursor. In this function we get the successor 
    // or precursor set from each tcm sketch, and compute their join. Note that this function can also be used to get the degree of a nodes, and it has higher accuracy than 
    // the node degree query function. But it is much slower than the node degree query function, as the computation cost of join is high.
    void BatchDegreeQuery(string filename, vector<string> &querynodes,int type); // get the degree of a set of nodes in querynodes and ouput the results to filename. Type 0 means 
    // successor and 1 means precursor. Compared to indivivual nodedegreequery, this function in fact transforms each matrix into an adjacency list with recover function and then carry out
	// degree query. Because when the matrix is large, count successors/ precursors in it will be costly. When the query set is large, like including all the vertices in the graph, it will save time
	// to transform the matrix into an adjacency list first. 
    void BatchTransQuery(string filename, vector<string> &srcs, vector<string> &dsts); // acarry out reachability query for a batch of nodes <src[i], dst[i]> and output the result to a file.
	void BatchNodeQuery(string filename, vector<string>& querynodes,int type);// carry out node query for a batch of nodes in querynodes, and output the result to a file.

};

void TCM::recover(icl_graph **g) // transform each compressed graph sketch from a matrix to a linked list
{
	for(int i=0;i<hashnum;i++){
	for(unsigned int j=0;j<width;j++)
	{
		for (unsigned int k = 0; k < width; k++)
		{
			if (value[i][j * width + k] > 0)
				g[i]->new_insert(j, k, value[i][j * width + k]);
		}
	}
}
}

void TCM::recover(icl_graph *g, int hash) // transform a specified graph sketch from a matrix to a set of linked list
{
	for(unsigned int j=0;j<width;j++)
	{
		for (unsigned int k = 0; k < width; k++)
		{
			if (value[hash][j * width + k] > 0)
				g->new_insert(j, k, value[hash][j * width + k]);
		}
	}
}

bool TCM::reach_query(string v1, string v2) // reachability query between v1 and v2
{
	for(int i=0;i<hashnum;i++) // check each matrix
	{
		bool found = false;
		bool* checked = new bool[width]; // used to mark if a node in the graph sketch has been visited
		for(int j=0;j<width;j++)
			checked[j] = false;
		unsigned int src=((*hfunc[i])((const unsigned char*)v1.c_str(),v1.length()))%width;
		unsigned int dest = ((*hfunc[i])((const unsigned char*)v2.c_str(), v2.length())) % width;
		queue<int> q;
		unsigned int v1 = src;
		unsigned int v2 = dest;
		unsigned int hash = v1*width+v2;
		if(value[i][hash]>0) // if src and dst are linked by an edge, it is reachable in this graph sketch
			continue;
		else // else carry out a standard BFS
		{
			q.push(v1);
			checked[v1]=true; 
			while(!q.empty())
			{
				v1 = q.front();
				if(value[i][v1*width+dest]>0) 
				{
					found = true;
					break;
				}
				for(unsigned int j=0;j<width;j++)
				{
					hash = v1*width+j;
					if(value[i][hash]>0&&checked[j]==false) // add all the 1-hop successors which have not been visited into the queue
					{
						q.push(j);
						checked[j]=true;
					}
				}
				q.pop();
			}
			if(!found)
				return false;  // if src cannot reach dst in a graph sketch, we can report not reachable, as there is no false negative in tcm
		}
		delete [] checked;
		queue<int>().swap(q);
	}
	return true;
}
TCM::TCM(unsigned int w,int h, bool UseTable, int TableSize)
{
    width = w;
    hashnum = h;
    value = new int*[hashnum];
	mapTable = new hashtable[hashnum]; // note that tcm needs multiple hash tables to store node mapping in different graph sketched
	usetable=UseTable;
	tablesize=TableSize;
    for (int i=0; i<hashnum; ++i)
    {
        value[i] = new int[width*width];
        memset(value[i],0,sizeof(int)*width*width);
        if(usetable)
		mapTable[i].init(tablesize); 
    }      
}

void TCM::insert(string v1, string v2,int weight) // insert an edge <v1, v2> with weight to tcm 
{
	for (int i = 0; i < hashnum; i++)
	{
		unsigned int hash1 = ((*hfunc[i])((const unsigned char*)v1.c_str(), v1.length()))%width;
		unsigned int hash2 = ((*hfunc[i])((const unsigned char*)v2.c_str(), v2.length()))%width;
		if(usetable&&insert_ht) // record the mapping between original ID and graph sketch node ID.
		{mapTable[i].insert(hash1, v1);
		mapTable[i].insert(hash2, v2);
	}
		hash1 = hash1 % width;
		hash2 = hash2 % width;
		value[i][hash1*width + hash2] += weight;

	}
}

int TCM::query(string v1, string v2) // edge query, namely query for the weight of an edge v1, v2.
{
    int min=INT32_MAX;
    for(int i=0;i<hashnum;i++)
    {
    	unsigned int hash1 = ((*hfunc[i])((const unsigned char*)v1.c_str(), v1.length())) % width;
    	unsigned int hash2 = ((*hfunc[i])((const unsigned char*)v2.c_str(), v2.length())) % width;
    	int v = value[i][hash1*width+hash2];
    	if(v<min) // record the minimum value among all graph sketches.
    	min = v;
	}
    return min;
}

int TCM::nodequery(string v1, int type) // node query, namely get the sum of all edges 
{
	if(type==0) // successor query 
	{
		int min = INT32_MAX;
		for(int i=0;i<hashnum;i++)
		{
			unsigned int hash1 = ((*hfunc[i])((const unsigned char*)v1.c_str(), v1.length())) %width; // the mapped graph sketch node
			hash1 = hash1*width; 
			int sum=0;
			for(unsigned int j=0;j<width;j++) // add up all the weights of edges in the matrix row 
			{
				sum+=value[i][hash1+j];
			}
			if(sum<min) // record the minimum value among all graph sketches 
			min=sum;
			if(min==0)
				break;
		}
		return min;
	}
	else // precursor query
	{
		int min = INT32_MAX;
		for(int i=0;i<hashnum;i++)
		{
			unsigned int hash1 = ((*hfunc[i])((const unsigned char*)v1.c_str(), v1.length()))%width;
			int sum=0;
			for(unsigned int j=0;j<width;j++) // add up all the weights of edges in the matrix column
				sum+=value[i][j*width+hash1];
			if(sum<min) // record the minimum value among all graph sketches 
			min=sum;
			if(min==0)
				break;
		}
		return min;	
	}
 } 


int TCM::nodedegreequery(string v1, int type)
{
	int min = INT32_MAX;
	if (type == 0) // successor query
	{
		for (int i = 0; i<hashnum; i++)
		{
			unsigned int hash1 = ((*hfunc[i])((const unsigned char*)v1.c_str(), v1.length())) % width;
			hash1 = hash1*width;
			int sum = 0;
			for (unsigned int j = 0; j<width; j++)
			{
				if (value[i][hash1 + j] > 0)sum+=mapTable[i].count_ID(j); // if the bucket in column j is not empty, there is an edge between the mapped node and node j, 
				// we get all the original nodes mapped to j and add them to the successor sum.
			}
			if (sum < min) // compute the minimum value among all 
				min = sum;
			if (min == 0)
				break;
		}
	}
	else // precursor query, similar to successor query, but check column rather than row
	{
		for (int i = 0; i<hashnum; i++)
		{
			unsigned int hash1 = ((*hfunc[i])((const unsigned char*)v1.c_str(), v1.length())) % width;
			int sum = 0;
			for (unsigned int j = 0; j < width; j++)
			{
				if (value[i][j*width + hash1] > 0) sum+=mapTable[i].count_ID(j);
			
			}
			if (sum < min)
				min = sum;
			if (min == 0)
				break;
		}
	}
	return min;
}


void TCM::get_neighbor(string v1, vector<string>& vec, int type) // this function get the neighbor set of a node v1. It can also be used to get successor degree and has higher accuracy
// than the above degree query. But it is much slower, as it needs to compute join. The original tcm doese not provide degree query operator, and we think both the above degree query or this query make sense. 
{
	unordered_map<string, int> join_set; // the hash table used to compute join
	if (type == 0) // successor query 
	{
		for (int i = 0; i<hashnum; i++)
		{
			unsigned int hash1 = ((*hfunc[i])((const unsigned char*)v1.c_str(), v1.length())) % width;
			hash1 = hash1*width;
			int sum = 0;
			for (unsigned int j = 0; j<width; j++)
			{
				if (value[i][hash1 + j] > 0)
				{
					vector<string> tmp_vec;
					mapTable[i].get_ID(j, tmp_vec); // we get all the original node id for the successor graph sketch node
					for(int k=0;k<tmp_vec.size();k++)
					{
						unordered_map<string, int>::iterator it = join_set.find(tmp_vec[k]);
						if(it!=join_set.end()) // we record the presence of the original node id in the successor set of this sketch  
							it->second++; 
						else if(i==0) // if this it not the first graph sketch and we cannot find the original node id, it is in absence in at least one graph sketch. Due to the one-side error property of tcm, this node can be skipped
							join_set[tmp_vec[k]] = 1;
					} 
					tmp_vec.clear();
					vector<string>().swap(tmp_vec);
				}
			}
		}
	}
	else // precursor query, similar to successor query.
	{
		for (int i = 0; i<hashnum; i++)
		{
			unsigned int hash1 = ((*hfunc[i])((const unsigned char*)v1.c_str(), v1.length())) % width;
			int sum = 0;
			for (unsigned int j = 0; j < width; j++)
			{
					if (value[i][j*width+hash1] > 0)
				{
					vector<string> tmp_vec;
					mapTable[i].get_ID(j, vec);
					for(int k=0;k<tmp_vec.size();k++)
					{
						unordered_map<string, int>::iterator it = join_set.find(tmp_vec[k]);
						if(it!=join_set.end())
							it->second++;
						else if(i==0)
							join_set[tmp_vec[k]] = 1;
					} 
					tmp_vec.clear();
					vector<string>().swap(tmp_vec);
				}
			
			}
		}
	}
	unordered_map<string, int>::iterator it;
	for(it = join_set.begin();it!=join_set.end();it++)
	{
		if(it->second==hashnum) // only the original graph nodes which are 1-hop successors/precursors in all graph sketches are returned
			vec.push_back(it->first);
	}
	join_set.clear(); 
}

// the follows are batch queries where we first trasform the adjacency matrix into adjacency list and then answer a batch of degree/node/reachability query. When the matrix is large and sparse
// these queries can be slow in the matrix, and it is more efficient to transform to adjacency list first.
// Note that we donot provide batch get_neighbor  as the bottleneck of that query is to compute join rather than scanning matrix.
void TCM::BatchDegreeQuery(string filename, vector<string>& querynodes, int type)
{
	unordered_map<string, int> degree;
	for(int i =0;i<hashnum;i++)
	{
	icl_graph* g = new icl_graph();
	recover(g, i); // transform each matrix to adjacency list
	for (int cur = 0; cur < querynodes.size(); cur++)
	{
		string v1 = querynodes[cur];
		vector<unsigned int> vec;
		if (type == 0) // carry out 1-hop successor / precursor query in the adjacency list, and get neighbor set in the graph sketch
		{
			unsigned int hash1 = ((*hfunc[i])((const unsigned char*)v1.c_str(), v1.length())) % width;
			g->successor(hash1, vec);
		}
		else
		{
			unsigned int hash1 = ((*hfunc[i])((const unsigned char*)v1.c_str(), v1.length())) % width;
			g->precursor(hash1, vec);
		}
		unsigned int sum = 0;
		for(int j=0;j<vec.size();j++) // recover the graph sketch neighbor to original graph nodes, and add up the node number.
			sum += mapTable[i].count_ID(vec[j]);
		if(degree.find(v1)!=degree.end())
		{
			if(sum<degree[v1]) // record the minimum value in all graph sketches as result.
				degree[v1] = sum;
		}
		else
			  degree[v1] = sum;
		vec.clear();
		vector<unsigned int>().swap(vec);
	}
	delete g; 
	}
	ofstream fout(filename.c_str());
	for(int i = 0;i<querynodes.size();i++) // out put the result 
		fout<<querynodes[i]<<' '<<degree[querynodes[i]]<<endl;
	degree.clear();
	fout.close();
}


void TCM::BatchNodeQuery(string filename, vector<string>& querynodes,int type)
{
	unordered_map<string, int> node_value;
	icl_graph* g = new icl_graph();
	for(int i=0;i<hashnum;i++){
	recover(g, i); // transform each matrix to adjacency list
	for (int cur = 0; cur < querynodes.size(); cur++)
	{
		string v1 = querynodes[cur];
		int sum = 0;
		if (type == 0) // carry out successor / precursor node query 
		{
			unsigned int hash1 = ((*hfunc[i])((const unsigned char*)v1.c_str(), v1.length())) % width;
			sum = g->successor_value(hash1);
		}
		else
		{
			unsigned int hash1 = ((*hfunc[i])((const unsigned char*)v1.c_str(), v1.length())) % width;
			sum = g->precursor_value(hash1);
		}
		if(node_value.find(v1)!=node_value.end())
		{
			if(sum<node_value[v1])
				node_value[v1] = sum;
		}
		else
			node_value[v1] = sum;
	}
	delete g;
	}
	ofstream fout(filename.c_str());
	for(int i = 0;i<querynodes.size();i++)
		fout<<querynodes[i]<<' '<<node_value[querynodes[i]]<<endl;
	node_value.clear();
	fout.close();
}

void TCM::BatchTransQuery(string filename, vector<string>& srcs, vector<string>& dsts)
{
	unordered_map<unsigned int, bool> reach; // this map records the reachability result of (srcs[i], dsts[i])
	
	for(int i=0;i<hashnum;i++){
	icl_graph* g = new icl_graph();
	recover(g, i); // transform to adjacency list
	for (int cur = 0; cur < srcs.size(); cur++)
	{
		unordered_map<unsigned int, bool>::iterator it = reach.find(cur);
		if(it!=reach.end())
		{
			if(!it->second) // if this node pair has been determined to be unreachable, we do not need to query any more 
				continue;
		} 
		string v1 = srcs[cur];
		string v2 = dsts[cur];
		unsigned int hash1 = ((*hfunc[i])((const unsigned char*)v1.c_str(), v1.length())) % width;
		unsigned int hash2 = ((*hfunc[i])((const unsigned char*)v2.c_str(), v2.length())) % width;
		bool reachability = g->reach_query(hash1, hash2, -1); // carry out reachability query in this graph sketch. The third parameter is the hop constraint, namley how many hops we allow the qualified path to have. Here we use -1 to set no constriant
		if(it!=reach.end())
		{
			if(it->second) // if this node pair has been determined to be unreachable, we do not change the result. Otherwise we update the result to the new reachability, which may be unreachable.
				it->second = reachability;
		}
		else
			reach[cur] = reachability;
	}
	delete g;
	}
	
	ofstream fout(filename.c_str());
	for(int i = 0;i<srcs.size();i++)
		fout<<srcs[i]<<' '<<dsts[i]<<' '<<reach[i]<<endl;
	reach.clear();
	fout.close();
}

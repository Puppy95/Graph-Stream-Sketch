#pragma once
#include<iostream>
#include<string>
#include<fstream>
#include<map>
#include<vector>
#include<queue>
#include<unordered_map>
#include<assert.h>
#define add_edge 1
#define modify_edge 0
#define delete_edge -1
using namespace std;

struct weighted_edge
{
	unsigned int src;
	unsigned int dst;
	int weight;
	weighted_edge(unsigned int s = 0, unsigned int d = 0, int w = 0)
	{
		src = s;
		dst = d;
		weight = w;
	}
};
struct icl_edge
{
	unsigned int s;
	unsigned int d;
	icl_edge* s_last;
	icl_edge* d_last;
	icl_edge* s_next;
	icl_edge* d_next;
	int weight;
};
struct icl_node
{
	unsigned int ID;
	icl_edge* src_el;
	icl_edge* dst_el;	
}; 
class icl_graph
{
	private:
		int n;
		int N;
		
	public:
		int chong;
		unordered_map<unsigned int, int> index;

		vector<icl_node*> g;
		icl_graph();
		~icl_graph(){
		for(int i=0;i<g.size();i++)
		{
			icl_edge* tmp = g[i]->src_el;
			icl_edge* next = tmp;
			while(tmp)
			{
				next = tmp->s_next;
				delete tmp;
				tmp = next;
			}
		}
		g.clear();
		vector<icl_node*>().swap(g);
		index.clear();
		}
		int get_mem()
		{
			int mem = 12 + sizeof(unordered_map<unsigned int, int>) + sizeof(vector<icl_node*>);
			mem += index.bucket_count() * 8 + index.size() * 16;
			mem += g.capacity() * 8;
			for (int i = 0; i < g.size(); i++)
			{
				mem += 24;
				icl_edge* tmp = g[i]->src_el;
				while (tmp)
				{
					mem += 48;
					tmp = tmp->s_next;
				}
			}
			return mem;
		}
		int insert(unsigned int s1, unsigned int s2,int weight);
			void resize(int size)
		{
			index.reserve(size);
		} 
		int query(unsigned int s1, unsigned int s2);
		int	get_node_num();
		int get_edge_num();
		void successor(unsigned int v, vector<unsigned int> &vec);
		void weighted_successor(unsigned int v, vector<pair<unsigned int, int>>& vec);
		void precursor(unsigned int v, vector<unsigned int> &pre);
		int successor_value(unsigned int v);
		int precursor_value(unsigned int v);
		int insert_with_delete(unsigned int s1, unsigned int s2, int w);
		int remove(unsigned int s1, unsigned int s2);
		void new_insert(unsigned int s1, unsigned int s2, int w);
		void get_edges(vector<weighted_edge> &vec);
		void SSSP(unsigned int v, unordered_map<unsigned int, int>& distance);
		bool reach_query(unsigned int s, unsigned int d, int step);
		void unreach_range(unsigned int root, int step,vector<unsigned int> &vec)
		{
			bool* reachability = new bool[g.size()];
			for(int i=0;i<g.size();i++)
				reachability[i] = false;
			queue<unsigned int> q;
			unordered_map<unsigned int, int> steps;
			q.push(root);
			reachability[index[root]] = true;
			steps[root] = 0;
			while(!q.empty())
			{
				unsigned int v = q.front();
				q.pop();
				if(step!=-1)
				{
					if(steps[v]>=step)
						continue;
				}
				int ind = 0;
				if(index.find(v)!=index.end())
					ind = index[v];
				else
					continue;
				icl_edge* tmp = g[ind]->src_el;
				while(tmp)
				{
					int dst = tmp->d;
					if(steps.find(dst)==steps.end())
					{
						steps[dst] = steps[v]+1;
						q.push(dst);
						reachability[index[dst]] = true;
					}
					tmp = tmp->s_next;	
				} 
			}
			for(int i=0;i<g.size();i++)
			{
				if(!reachability[i])
					vec.push_back(g[i]->ID);
			}
			delete []reachability;
		}
 }; 
 
 icl_graph::icl_graph()
 {
 	n=0;
 	N=0;
 }
 int icl_graph::get_node_num()
 {
 	return n;
 }
 int icl_graph::get_edge_num()
 {
 	return N;
 }

 void icl_graph::new_insert(unsigned int s1, unsigned int s2, int w)
 {

	 int n1;
	 unordered_map<unsigned int, int>::iterator it;
	 it = index.find(s1);
	 if (it != index.end())
		 n1 = it->second;
	 else
	 {
		 index[s1] = n;
		 n1 = n;
		 n++;
		 icl_node *e = new icl_node;
		 e->src_el = NULL;
		 e->dst_el = NULL;
		 e->ID = s1;
		 g.push_back(e);
	 }

	 int n2;
	 it = index.find(s2);
	 if (it != index.end())
		 n2 = it->second;
	 else
	 {
		 index[s2] = n;
		 n2 = n;
		 n++;
		 icl_node* e = new icl_node;
		 e->src_el = NULL;
		 e->dst_el = NULL;
		 e->ID = s2;
		 g.push_back(e);
	 }

	 N++;
	 icl_edge* f = new icl_edge;
	 f->s = s1;
	 f->d = s2;
	 f->weight = w;
	 
	 f->s_next = g[n1]->src_el;
	 f->s_last = NULL;
	 if(g[n1]->src_el)
	 	g[n1]->src_el->s_last = f;
	 g[n1]->src_el = f;
	 
	 f->d_next = g[n2]->dst_el;
	 f->d_last = NULL;
	 if(g[n2]->dst_el)
	 	g[n2]->dst_el->d_last = f;
	 g[n2]->dst_el = f;
	 

 }
 int icl_graph::insert(unsigned int s1, unsigned int s2,int w)
 {
 		int n1;
 		unordered_map<unsigned int,int>::iterator it;
 		it = index.find(s1);
		if(it!=index.end())
			n1 = it->second;
		else
		{
			index[s1]=n;
			n1=n;
			n++;
			icl_node *e=new icl_node;
			e->src_el = NULL;
			e->dst_el = NULL;
			e->ID = s1;
			g.push_back(e); 
		}
		
		int n2;
		it = index.find(s2);
		if(it!=index.end())
			n2 = it->second;
		else
		{
			index[s2] = n;
			n2 = n;
			n++;
			icl_node* e = new icl_node;
			e->src_el = NULL;
			e->dst_el = NULL;
		 	e->ID = s2;
		 	g.push_back(e);
		}


		icl_edge* e=g[n1]->src_el;
		bool find = false;
		while(e)
		{
			if(e->d==s2)
			{
				e->weight+=w;
				find = true;
				break;
			}
			e=e->s_next;
		}
		if (!find)
		{
			N++;
			icl_edge* f = new icl_edge;
			f->s = s1;
			f->d = s2;
			f->weight = w;
			
			f->s_next = g[n1]->src_el;
	 		f->s_last = NULL;
	 		if(g[n1]->src_el)
	 			g[n1]->src_el->s_last = f;
	 		g[n1]->src_el = f;
	 
	 		f->d_next = g[n2]->dst_el;
	 		f->d_last = NULL;
	 		if(g[n2]->dst_el)
	 			g[n2]->dst_el->d_last = f;
	 		g[n2]->dst_el = f;
	 		
			return add_edge;
		}
		else
			return modify_edge;
 }

 int icl_graph::insert_with_delete(unsigned int s1, unsigned int s2, int w)
 {
	 int n1;
	 unordered_map<unsigned int, int>::iterator it;
	 it = index.find(s1);
	 if (it != index.end())
		 n1 = it->second;
	 else
	 {
		 index[s1] = n;
		 n1 = n;
		 n++;
		 icl_node *e = new icl_node;
		 e->src_el = NULL;
		 e->dst_el = NULL;
		 e->ID = s1;
		 g.push_back(e);
	 }

	 int n2;
	 it = index.find(s2);
	 if (it != index.end())
		 n2 = it->second;
	 else
	 {
		 index[s2] = n;
		 n2 = n;
		 n++;
		 icl_node* e = new icl_node;
		 e->src_el = NULL;
		 e->dst_el = NULL;
		 e->ID = s2;
		 g.push_back(e);
	 }


	 icl_edge* e = g[n1]->src_el;
	 icl_edge* sp = e;
	 bool find = false;
	 while (e)
	 {
		 if (e->d == s2)
		 {
			 e->weight += w;
			 if (e->weight <= 0)
			 {
				 if (g[n1]->src_el == e)
				{
					 g[n1]->src_el = e->s_next;
					 if(e->s_next)
					 	e->s_next->s_last = NULL;
				}
				 else{
					 sp->s_next = e->s_next;
					 if(e->s_next)
					 	e->s_next->s_last = sp;
				}

				 if (g[n2]->dst_el == e)
				 {
					 g[n2]->dst_el = e->d_next;
					 if(e->d_next)
					 	e->d_next->d_last = NULL;
					}
				 else{
					if(e->d_last)
						e->d_last->d_next = e->d_next;
					if(e->d_next)
						e->d_next->d_last = e->d_last;
				 }
				 delete e;
				 N--;
				 return delete_edge;
			 }
			 else
				 return modify_edge;
		 }
		 sp = e;
		 e = e->s_next;
	 }

	N++;
	icl_edge* f = new icl_edge;
	f->s = s1;
	f->d = s2;
	f->weight = w;
	
	f->s_next = g[n1]->src_el;
	f->s_last = NULL;
	if(g[n1]->src_el)
	 	g[n1]->src_el->s_last = f;
	g[n1]->src_el = f;
	 
	f->d_next = g[n2]->dst_el;
	f->d_last = NULL;
	if(g[n2]->dst_el)
	 	g[n2]->dst_el->d_last = f;
	g[n2]->dst_el = f;
	return add_edge;
 }
 int icl_graph::query(unsigned int s1, unsigned int s2)
 {
 	unordered_map<unsigned int,int>::iterator it;
 	int n1;
 	it = index.find(s1);
	if(it!=index.end())
		n1 = it->second;
	else return 0;
	icl_edge* e = g[n1]->src_el;
	while(e!=NULL)
	{
		if(e->d==s2)
			return e->weight;
		e = e->s_next;
	}
		if(e==NULL)
 		return 0;
 }

void icl_graph::successor(unsigned int v, vector<unsigned int> &vec)
{
	unordered_map<unsigned int, int>::iterator it;
	int n1;
	it = index.find(v);
	if(it!=index.end())
		n1 = it->second;
	else return;
	icl_edge* e = g[n1]->src_el;
	while(e!=NULL)
	{
		vec.push_back(e->d);
		e= e->s_next;
	}
	return;
}

void icl_graph::weighted_successor(unsigned int v, vector<pair<unsigned int, int>>& vec)
{
	unordered_map<unsigned int, int>::iterator it;
	int n1;
	it = index.find(v);
	if (it != index.end())
		n1 = it->second;
	else return;
	icl_edge* e = g[n1]->src_el;
	while (e != NULL)
	{
		vec.push_back(make_pair(e->d, e->weight));
		e = e->s_next;
	}
	return;
}

int icl_graph::successor_value(unsigned int v)
{
	int sum = 0;
	unordered_map<unsigned int, int>::iterator it;
	int n1;
	it = index.find(v);
	if (it != index.end())
		n1 = it->second;
	else return 0;
	icl_edge* e = g[n1]->src_el;
	while (e != NULL)
	{
		sum += e->weight;
		e = e->s_next;
	}
	return sum;

}

void icl_graph::precursor(unsigned int v, vector<unsigned int> &pre)
{
	unordered_map<unsigned int, int>::iterator it;
	int n1;
	it = index.find(v);
	if(it!=index.end())
		n1 = it->second;
	else return;
	icl_edge* e = g[n1]->dst_el;
	while(e!=NULL)
	{
		pre.push_back(e->s);
		e= e->d_next;
	}
	return;
}
int icl_graph::precursor_value(unsigned int v)
{
	int sum = 0;
	unordered_map<unsigned int, int>::iterator it;
	int n1;
	it = index.find(v);
	if (it != index.end())
		n1 = it->second;
	else return 0;
	icl_edge* e = g[n1]->dst_el;
	while (e != NULL)
	{
		sum += e->weight;
		e = e->d_next;
	}
	return sum;

}

int icl_graph::remove(unsigned int src, unsigned int dst)
{
	int n1;
	unordered_map<unsigned int, int>::iterator it;
	it = index.find(src);
	if (it != index.end())
		n1 = it->second;
	else
		return 0;

	int n2;
	it = index.find(dst);
	if (it != index.end())
		n2 = it->second;
	else
		return 0;


	icl_edge* e = g[n1]->src_el;
	icl_edge* sp = e;
	while (e)
	{
		if (e->d == dst)
		{
		 if (g[n1]->src_el == e)
		 {
				g[n1]->src_el = e->s_next;
				if(e->s_next)
					e->s_next->s_last = NULL;
		 }
		 else
		 {
				sp->s_next = e->s_next;
				if(e->s_next)
					e->s_next->s_last = sp;
		 }

		 if (g[n2]->dst_el == e)
		 {
			g[n2]->dst_el = e->d_next;
			if(e->d_next)
				e->d_next->d_last = NULL;
		 }
		 else
		 {
			if(e->d_last)
				e->d_last->d_next = e->d_next;
			if(e->d_next)
				e->d_next->d_last = e->d_last;	 
		 }
		 int tmp = e->weight;
		 delete e;
		 N--;
		 return tmp;
		 }
		sp = e;
		e = e->s_next;
	}
	return 0;
}

void icl_graph::get_edges(vector<weighted_edge> &vec)
{
	for (int i = 0; i < g.size(); i++)
	{
		icl_edge* tmp = g[i]->src_el; 
		while (tmp)
		{
			vec.push_back(weighted_edge(g[i]->ID, tmp->d, tmp->weight));
			tmp = tmp->s_next;
		}
	}
	return;
}


struct node_distance
{
	unsigned int id;
	int distance;
	node_distance(int id_, int distance_)
	{
		id = id_;
		distance = distance_;
	}
};

struct cmp
{
	bool operator()(node_distance n1, node_distance n2)
	{
		if (n1.distance < 0)
			return true;
		else if (n2.distance < 0)
			return false;
		else
			return n1.distance>n2.distance;
	}
};

void icl_graph::SSSP(unsigned int root, unordered_map<unsigned int, int>& distance)
{
	priority_queue<node_distance, vector<node_distance>, cmp> q;
	q.push(node_distance(root, 0));
	while (!q.empty()){
		node_distance nd = q.top();
		q.pop();
		unsigned int v = nd.id;
		if (distance.find(v) != distance.end())
			continue;
		distance[v] = nd.distance;

		unordered_map<unsigned int, int>::iterator it;
		it = index.find(v);
		if (it == index.end())
			continue;
		else
		{
			icl_edge* tmp = g[it->second]->src_el;
			while (tmp)
			{
				q.push(node_distance(tmp->d, tmp->weight + nd.distance));
				tmp = tmp->s_next;
			}
		}
	}
	return;
}

bool icl_graph::reach_query(unsigned int s, unsigned int d, int step)
{
	queue<unsigned int> q;
	unordered_map<unsigned int, int > steps;
	q.push(s);
	steps[s] = 0;
	while(!q.empty())
	{
		unsigned int v = q.front();
		q.pop();
		if(step!=-1)
		{
			if(steps[v]>=step)
				continue;
		}
		unordered_map<unsigned int, int>::iterator it = index.find(v);
		if(it==index.end())
			continue;
		else
			{
				int ind = it->second;
				icl_edge* tmp = g[ind]->src_el;
				while(tmp)
				{
					int dst = tmp->d;
					if(dst==d)
					{
						queue<unsigned int>().swap(q);
						steps.clear();
						return true;
					}
					if(steps.find(dst)==steps.end())
					{
						steps[dst] = steps[v]+1;
						q.push(dst);
					}
					tmp = tmp->s_next;
				}
			}	
	}
	steps.clear();
	return false;	
 } 

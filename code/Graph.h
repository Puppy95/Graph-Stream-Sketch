#include<iostream>
#include<map>
#include<vector>
#include<set> 
#include<string>
#include<algorithm>
using namespace std;
struct edge
{
	public:
	int node;
	set<int> label;
	edge(int n, int l)
	{
		node=n;
		label.insert(l);
	}
};
class node
{
	public:
	int id;
	int label;
	vector<edge> pre;
	vector<edge> suc;
	node(int i, int l)
	{
		id=i;
		label=l;
	}
	void insertp(int n, int l)
	{
		bool find=false;
		for(int i=0;i<pre.size();i++)
		{
			if(pre[i].node==n)
			{
				find=true;
				pre[i].label.insert(l);
			}	
		}
		if(!find)
		{
			edge e(n,l);
			pre.push_back(e);
		}
	}
	void inserts(int n, int l)
	{
		bool find=false;
		for(int i=0;i<suc.size();i++)
		{
			if(suc[i].node==n)
			{
				find=true;
				suc[i].label.insert(l);
			}	
		}
		if(!find)
		{
			edge e(n,l);
			suc.push_back(e);
		}
	}
	void has_pre(int n,set<int> &P)
	{
		for(int i=0;i<pre.size();i++)
		{
			if(pre[i].node==n)
			{	
			 P.insert(pre[i].label.begin(),pre[i].label.end());
			 return;
			}
		}
		return;
	}
	void has_suc(int n,set<int> &S)
	{
		for(int i=0;i<suc.size();i++ )
		{
			if(suc[i].node==n)
			{	
			 S.insert(suc[i].label.begin(),suc[i].label.end());
			 return;
			}
		}
		return;
	}
 };
 class graph
 {
 	public:
 	vector<node> Node;
 	map<int, int> nodemap;
 	int nodenum;
 	graph(){
 		nodenum=0;
	 };
	 void insert_node(int n, int l)
	 {
	 	map<int,int>::iterator it=nodemap.find(n);
	 	if(it==nodemap.end())
	 	{
		    node N(n, l);
	 		Node.push_back(N);
	 		nodemap[n]=nodenum;
	 		nodenum++;
	 	}
	 }
	 void insert_edge(int s, int d, int l)
	 {
	 	for(int i=0;i<Node.size();i++)
	 	{
	 		if(Node[i].id==d)
	 			Node[i].insertp(s,l);
	 		if(Node[i].id==s)
	 			Node[i].inserts(d,l);
		 }
	}
  } ;

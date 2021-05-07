#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<set>
#include<map>
#include<cmath>
#include<stdlib.h>
#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "hashTable.h"
#endif // HASHTABLE_H
struct GSnode
{
	unsigned int h;
	GSnode* next;
};
struct degree_node
{
	unsigned int h;
	int degree;
};
bool comp_degree(const degree_node &dn1, const degree_node &dn2)
{
	return dn1.degree<dn2.degree;
}
class GSketch
{
	private:
	map<unsigned int, int> degree; 
	map<unsigned int, int> index;
	map<unsigned int, int> idnums;
	vector<degree_node> sorted_nodes;
	int nodenum;
	int triangle_num;
	int edge_num;
	vector<GSnode*> sketch;
	public:
	GSketch(){nodenum=0;
	edge_num=0;
	triangle_num=0; 
	};
	~GSketch(){
	degree.clear();
	for(int i=0;i<nodenum;i++)
	{
		GSnode* np=sketch[i];
		GSnode* tmp;
		while(np!=NULL)
		{
			tmp=np->next;
			delete np;
			np=tmp;
		}
	}
	sketch.clear(); 
	index.clear();  
	idnums.clear();
	sorted_nodes.clear(); };
	void get_neighbors(unsigned int h, vector<hashvalue>& hv);
	void insert_edge(unsigned int h1, unsigned int h2);
	void insert_node(unsigned int h1, int IDnum);
	void clean();
	int countTriangle();	
};
void GSketch::insert_node(unsigned int h1, int IDnum)
{
	index[h1]=nodenum;
	degree[h1]=0;
	idnums[h1]=IDnum;
	GSnode* np=new GSnode;
	np->h=h1;
	np->next=NULL;
	sketch.push_back(np);
	nodenum++;
}
void GSketch::insert_edge(unsigned int h1, unsigned int h2)
{
	degree[h1]++;
	degree[h2]++;
	GSnode* np = sketch[index[h1]];
	for(;np!=NULL;np=np->next)
	{
		if(np->h==h2)
			break;
		if(np->next==NULL)
		{
			GSnode* newn=new GSnode;
			newn->h=h2;
			newn->next=NULL;
			np->next=newn;
			edge_num++;
			break;
		}
	}
	np = sketch[index[h2]];
	for(;np!=NULL;np=np->next)
	{
		if(np->h==h1)
			break;
		if(np->next==NULL)
		{
			GSnode* newn=new GSnode;
			newn->h=h1;
			newn->next=NULL;
			np->next=newn;
			edge_num++;
			break;
		}
	}
}
void GSketch::clean()
{
	map<unsigned int, int> sequence;
	map<unsigned int, int>::iterator it;
	for(it=degree.begin();it!=degree.end();it++)
	{
		degree_node dn;
		dn.h=it->first;
		dn.degree=it->second;
		sorted_nodes.push_back(dn);
	}
	sort(sorted_nodes.begin(),sorted_nodes.end(),comp_degree);
	for(int i=0;i<nodenum;i++)
		sequence[sorted_nodes[i].h]=i;
	for(int i=0;i<nodenum;i++)
	{
		GSnode* np=sketch[i];
		int seq_d=sequence[np->h];
		GSnode* parent=np;
		np=np->next;
		while(np!=NULL)
		{
			if(sequence[np->h]<=seq_d)
			{
				parent->next=np->next;
				delete np;
				np=parent->next;
				edge_num--;
			}
			else
			{
				parent=np;
				np=np->next;
			}
		}
	}
 }
 void GSketch::get_neighbors(unsigned int h, vector<hashvalue>& hv)
 {
 	map<unsigned int, int>::iterator it;
 	it=index.find(h);
 	if(it==index.end()) return;
 	GSnode* np=sketch[it->second];
 	np=np->next;
 	for(;np!=NULL;np=np->next)
 	{
 	//	cout<<"in"<<endl;
 		if(np->h==h)
 			cout<<"loop!"<<endl;
 		hashvalue new_hv;
 		new_hv.key=np->h;
 		new_hv.IDnum=idnums[np->h];
 		hv.push_back(new_hv);
	 }
	 return;
  } 
  int GSketch::countTriangle()
  {
  	int count=0;
  	for(int i=0;i<nodenum;i++)
  	{
  		unsigned int h1=sorted_nodes[i].h;
  		int num1=idnums[h1];
  		vector<hashvalue> set1;
  		get_neighbors(h1, set1);
  		sort(set1.begin(),set1.end(),mycomp);
  	//	cout<<set1.size()<<endl;
  		for(int j=0;j<set1.size();j++)
  		{
  			unsigned int h2=set1[j].key;
  			int num2=set1[j].IDnum;
  			vector<hashvalue> set2;
  			get_neighbors(h2,set2);
  			sort(set2.begin(),set2.end(),mycomp);
  			int num3=countjoin(set1,set2);
  		//	cout<<num1<<' '<<num2<<' '<<num3<<endl;
  			count+=num1*num2*num3;
		  }
	  }
	  return count;
  }

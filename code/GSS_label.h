#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<set>
#include<map>
#include<sstream>
#include<cmath>
#include<stdlib.h>
#include<bitset>
#include<memory.h>
#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "hashTable.h"
#include "GraphSketchUndirected.h" 
#include "VF2.h"
#endif // HASHTABLE_H
using namespace std;
#define prime 739
#define bigger_p 1048576
#define timer 5
#define M 80000
#define Roomnum 2
struct labelednode
{
	int ID;
	int label;
};
bool operator ==(const labelednode& ln1, const labelednode &ln2)
{
	return (ln1.ID==ln2.ID &&ln1.label==ln2.label);
}
struct labeled_Successor
{
	int Successor;
	int edge_label;
};
struct labeled_Precursor
{
	int Precursor;
	int edge_label;
};
struct basket
{
	unsigned int src[Roomnum];
	unsigned int dst[Roomnum];
	int  label[Roomnum];
	int weight[Roomnum];
	unsigned int idx;
};
struct mapnode
{
	int h;
	int g;
};
struct linknode
{
	unsigned int key;
	int label;
	int weight;
	linknode* next;
};

struct labeled_edge_info
{
	int weight;
	int label;
};
class label_GSS
{
private:
	hashTable<labelednode> mapTable;
	int w;
	int r;
	int p;
//	int k;
	int s;
	int f;
	int TABLESIZE;
	
	basket* value;

	public:
		vector<linknode*> buffer;
		map<int, int> index;
		int n;
		label_GSS(int width, int range, int p_num, int size,int f_num, int tablesize);
		~label_GSS()
		{
			delete[] value;
			cleanupBuffer();
		 }
		 void insert(int s1, int s2,int edgelabel, int nodelabel1, int nodelabel2, int weight);
		 void cleanupBuffer();
		 void edgeQuery(int s1, int s2, vector<labeled_edge_info>& info);
		 bool query(int s1, int s2);
		 int nodeValueQuery(int s1, int type);//src_type = 0 dst_type = 1
		 int nodeDegreeQuery(int s1, int type);//src_type = 0 dst_type = 1
		 void nodeSuccessorQuery(int s1, vector<labeled_Successor> &LS);
		 void nodePrecursorQuery(int s2, vector<labeled_Precursor> &LP); 
		 int countIDnums(unsigned int hash);
		 //int countIDnums(unsigned int hash); 
		 void graph_match(vector<graph*> &gq, string file,vector<int> &matchcount);
};

label_GSS::label_GSS(int width, int range, int p_num, int size,int f_num, int tablesize)
{
	w = width;
	r = range; /* r x r mapped baskets */
	p = p_num; /*candidate buckets*/
	//k = k_num;
	s = size; /*multiple rooms*/
	f = f_num; /*finger print lenth*/
	n = 0;
	TABLESIZE=tablesize;
	value = new basket[w*w];
	memset(value, 0, sizeof(basket)*w*w);
	mapTable.init(TABLESIZE);
}
void label_GSS::cleanupBuffer()
{
	vector<linknode*>::iterator IT = buffer.begin();
	linknode* e, *tmp;
	for (; IT != buffer.end(); ++IT)
	{
		e = *IT;
		while (e != NULL)
		{
			tmp = e->next;
			delete e;
			e = tmp;
		}
	}
}
void label_GSS::insert(int id1, int id2,int edgelabel, int nodelabel1, int nodelabel2, int weight)
{
		//cout<<id1<<' '<<id2<<endl;		
		stringstream ss1,ss2;
		ss1<<id1; 
		ss2<<id2;
		string s1=ss1.str();
		string s2=ss2.str();
		unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
		unsigned int hash2 = (*hfunc[0])((unsigned char*)(s2.c_str()), s2.length());
		int tmp = pow(2,f)-1;
		unsigned int g1 = hash1 & tmp;
		if(g1==0) g1+=1;
		unsigned int h1 = (hash1>>f)%w;
		unsigned int g2 = hash2 & tmp;
		if(g2==0) g2+=1;
		unsigned int h2 = (hash2>>f)%w;
		
		unsigned int k1 = (h1<<f)+g1;
		unsigned int k2 = (h2<<f)+g2;

//#ifdef addHashTable
		labelednode n1;
		n1.ID=id1;
		n1.label=nodelabel1;
		labelednode n2;
		n2.ID=id2;
		n2.label=nodelabel2;
		mapTable.insert(k1, n1);
		mapTable.insert(k2, n2);
//#endif // addHashTable
		int* tmp1 = new int[r];
		int* tmp2 = new int[r];
		tmp1[0] = g1;
		tmp2[0] = g2;
		for(int i=1;i<r;i++)
		{
			tmp1[i]=(tmp1[i-1]*timer+prime)%bigger_p;
			tmp2[i]=(tmp2[i-1]*timer+prime)%bigger_p;
		}
		bool inserted=false;
		long key = g1+g2; 
		for(int i=0;i<p;i++)
		{
			key = (key*timer+prime)%bigger_p;
			int index = key%(r*r);
			int index1 = index/r;
			int index2 = index%r; 
			int p1 = (h1+tmp1[index1])%w;
			int p2 = (h2+tmp2[index2])%w;


			int pos = p1*w + p2;
			for (int j = 0; j < s; j++)
			{
				if ( ( ((value[pos].idx>>(j<<3))&((1<<8)-1)) == (index1|(index2<<4)) ) && (value[pos].src[j]== g1) && (value[pos].dst[j] == g2) &&(value[pos].label[j]==edgelabel) )
				{
					value[pos].weight[j] += weight;
					delete []tmp1;
					delete []tmp2;
					return;
				}
				if (value[pos].src[j] == 0)
				{
					value[pos].idx |= ((index1 | (index2 << 4)) << (j<<3));
					value[pos].src[j] = g1;
					value[pos].dst[j] = g2;
					value[pos].weight[j] = weight;
					value[pos].label[j] = edgelabel;
					inserted = true;
					break;
				}
			}
			if(inserted)
				break;
		} 
		if(!inserted)
		{
			unsigned int k1 = (h1<<f)+g1;
			unsigned int k2 = (h2<<f)+g2;
		
			map<int, int>::iterator it = index.find(k1);
			if(it!=index.end())
			{
				int tag = it->second;
				linknode* node = buffer[tag];
				while(true)
				{
					if (node->key == k2 && node->label==edgelabel)
					{   
						node->weight += weight;
						break;
					}
					if(node->next==NULL)
					{
						linknode* ins = new linknode;
						ins->key = k2;
						ins->weight = weight;
						ins->label = edgelabel;
						ins->next = NULL;
						node->next = ins;
						break;
					}
					node = node->next;
				}
			}
			else
			{
				index[k1] = n;
				n++;
				linknode* node = new linknode;
				node->key = k1;
				node->weight = 0;
				node->label = 0;
				if (k1 != k2 || node->label!=0)//k1==k2 means loop
				{
					linknode* ins = new linknode;
					ins->key = k2;
					ins->weight = weight;
					ins->label = edgelabel;
					ins->next = NULL;
					node->next = ins;
				}
				else
				{ 
					node->weight += weight;
					node->label = edgelabel;
					node->next = NULL;
				}
				buffer.push_back(node); 
			}	
		}
		delete [] tmp1;
		delete [] tmp2;
	return;
}
void label_GSS::nodeSuccessorQuery(int id1, vector<labeled_Successor> &LS)
{
	stringstream ss1;
	ss1<<id1; 
	string s1=ss1.str();
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	int tmp=pow(2,f)-1;
	unsigned int g1=hash1 & tmp;
	if(g1 == 0) g1 = g1+1;
	unsigned int h1 = (hash1>>f)%w;
	unsigned int k1 = (h1 << f) + g1;
	int* tmp1 = new int[r];
	tmp1[0] = g1;
	for (int i = 1; i < r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
	}
	for (int i = 0; i < r; i++)
	{
		int p1 = (h1 + tmp1[i]) % w;
		for (int k = 0; k < w; k++)
		{
			int pos = p1*w + k;
			for (int j = 0; j < s; ++j)
			{
				if ((((value[pos].idx >> ((j << 3)))&((1 << 4) - 1)) == i) && (value[pos].src[j] == g1))
				{
					     int tmp_g = value[pos].dst[j];
					     int label = value[pos].label[j];
						 int tmp_s = ((value[pos].idx >> ((j << 3) + 4))&((1 << 4) - 1));
				
						 int shifter = tmp_g;
						 for (int v = 0; v < tmp_s; v++)
							 shifter = (shifter*timer + prime) % bigger_p;
						 unsigned int tmp_h = k;
						 while (tmp_h < shifter)
							 tmp_h += w;
						 tmp_h -= shifter;
						 unsigned int val = (tmp_h << f) + tmp_g;
						 vector<labelednode> IDs;
						 mapTable.getID(val, IDs);
						 for(int it=0;it<IDs.size();it++)
						 {
						 	labeled_Successor ls;
						 	ls.edge_label=label;
						 	ls.Successor=IDs[it].ID;
						 	LS.push_back(ls);
						 }
				}
			}
		}	
	}
		map<int, int>::iterator it = index.find(k1);
		if (it != index.end())
		{
			int tag = it->second;
			linknode* node = buffer[tag];
			node = node->next;
			while (node != NULL)
			{
				int label=node->label;
				vector<labelednode> IDs;
				mapTable.getID(node->key, IDs);
				for(int it=0;it<IDs.size();it++)
				{
					labeled_Successor ls;
					ls.edge_label=label;
					ls.Successor=IDs[it].ID;
					LS.push_back(ls);					
				}
				node=node->next;
			}
		}
		return; 
}
void label_GSS::nodePrecursorQuery(int id1, vector<labeled_Precursor>&LP)
{
	stringstream ss1;
	ss1<<id1; 
	string s1=ss1.str();
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	int tmp=pow(2,f)-1;
	unsigned int g1=hash1 & tmp;
	if(g1 == 0) g1 += 1;
	unsigned int h1=(hash1>>f)%w;
	unsigned int k1 = (h1 << f) + g1;
	int* tmp1 = new int[r];
	tmp1[0] = g1;
	for (int i = 1; i < r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
	}
	for (int i = 0; i < r; i++)
	{
		int p1 = (h1 + tmp1[i]) % w;
		for (int k = 0; k < w; k++)
		{
			int pos = p1 + k*w;
			for (int j = 0; j < s; ++j)
			{
				if ((((value[pos].idx >> ((j << 3)+4))&((1 << 4) - 1)) == i) && (value[pos].dst[j] == g1))
				{
					     int tmp_g = value[pos].src[j];
						 int tmp_s = ((value[pos].idx >> (j << 3))&((1 << 4) - 1));
						 int label = value[pos].label[j];	
							
						 int shifter = tmp_g;
						 for (int v = 0; v < tmp_s; v++)
							 shifter = (shifter*timer + prime) % bigger_p;
						 int tmp_h = k;
						 while (tmp_h < shifter)
							 tmp_h += w;
						 tmp_h -= shifter;
						 
						 vector<labelednode> IDs;
						 unsigned int val = (tmp_h << f) + tmp_g;
						 mapTable.getID(val, IDs);
						 
						 for(int it=0;it<IDs.size();it++)
						{
							labeled_Precursor lp;
							lp.edge_label=label;
							lp.Precursor=IDs[it].ID;
							LP.push_back(lp);					
						}
				}
			}
		}	
	}
			for (map<int, int>::iterator it = index.begin(); it != index.end(); ++it)
		{
			int tag = it->second;
			int src = it->first;
			linknode* node = buffer[tag];
			node = node->next;
			while (node != NULL)
			{
				if(node->key == k1)
				{
				//	int label=node->label;
					int label=node->label;
					vector<labelednode> IDs;
					mapTable.getID(src, IDs);
					for(int it=0;it<IDs.size();it++)
					{
						labeled_Precursor lp;
						lp.edge_label=label;
						lp.Precursor=IDs[it].ID;
						LP.push_back(lp);					
					}
					//mapTable.getID(src, IDs);
					break;	
				}
				node = node->next;
			}
		}
		return;
}
void label_GSS::edgeQuery(int id1, int id2, vector<labeled_edge_info> &info)
{
	stringstream ss1,ss2;
		ss1<<id1; 
		ss2<<id2;
		string s1=ss1.str();
		string s2=ss2.str();
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	unsigned int hash2 = (*hfunc[0])((unsigned char*)(s2.c_str()), s2.length());
	int tmp = pow(2, f) - 1;
	unsigned int g1 = hash1 & tmp;
	if (g1 == 0) g1 += 1;
	unsigned int h1 = (hash1 >> f) % w;
	unsigned int g2 = hash2 & tmp;
	if (g2 == 0) g2 += 1;
	unsigned int h2 = (hash2 >> f) % w;
	int* tmp1 = new int[r];
	int* tmp2 = new int[r];
	tmp1[0] = g1;
	tmp2[0] = g2;
	for (int i = 1; i<r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
		tmp2[i] = (tmp2[i - 1] * timer + prime) % bigger_p;
	}
	long key = g1 + g2;
	for (int i = 0; i<p; i++)
	{
		key = (key * timer + prime) % bigger_p;
		int index = key % (r*r);
		int index1 = index / r;
		int index2 = index%r;
		int p1 = (h1 + tmp1[index1]) % w;
		int p2 = (h2 + tmp2[index2]) % w;
		int pos = p1*w + p2;
		for (int j = 0; j<s; j++)
		{
		
			if ((((value[pos].idx >> (j << 3))&((1 << 8) - 1)) == (index1 | (index2 << 4))) && (value[pos].src[j] == g1) && (value[pos].dst[j] == g2))
			{
				int label=value[pos].label[j];
				labeled_edge_info lei;
				lei.label=label;
				lei.weight=value[pos].weight[j];
				//delete[] tmp1;
				//delete[] tmp2;
				info.push_back(lei);
			}
		}
		
	}
	unsigned int k1 = (h1 << f) + g1;
	unsigned int k2 = (h2 << f) + g2;
	map<int, int>::iterator it = index.find(k1);
	if (it != index.end())
	{
		int tag = it->second;
		linknode* node = buffer[tag];
		while (node!=NULL)
		{
			if (node->key == k2)
			{
				int label=node->label;
				labeled_edge_info lei;
				lei.label=label;
				lei.weight=node->weight;
				info.push_back(lei);
			}
			node = node->next;
		}
	}
		return;
}
bool label_GSS::query(int id1, int id2)
 {
 	stringstream ss1,ss2;
		ss1<<id1; 
		ss2<<id2;
		string s1=ss1.str();
		string s2=ss2.str();
	 unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	 unsigned int hash2 = (*hfunc[0])((unsigned char*)(s2.c_str()), s2.length());
	 int tmp = pow(2, f) - 1;
	 int g1 = hash1 & tmp;
	 if (g1 == 0) g1 += 1;
	 int h1 = (hash1 >> f) % w;
	 int g2 = hash2 & tmp;
	 if (g2 == 0) g2 += 1;
	 int h2 = (hash2 >> f) % w;
	 int pos;
	 map<int, bool> checked;
	 queue<mapnode> q;
	 mapnode e;
	 e.h = h1;
	 e.g = g1;
	 q.push(e);
	 checked[((h1 << f) + g1)] = true;
	 map<int, bool>::iterator	IT;

	 while (!q.empty())
	 {
		 e = q.front();
		 h1 = e.h;
		 g1 = e.g;
		 int* tmp1 = new int[r];
		 int* tmp2 = new int[r];
		 tmp2[0] = g2;
		 tmp1[0] = g1;
		 for (int i = 1; i<r; i++)
		 {
			 tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
			 tmp2[i] = (tmp2[i - 1] * timer + prime) % bigger_p;
		 }
		 for (int i1 = 0; i1<r; i1++)
		 {
			 int p1 = (h1 + tmp1[i1]) % w;
			 for (int i2 = 0; i2<r; i2++)
			 {
				 int p2 = (h2 + tmp2[i2]) % w;
				 int pos = p1*w + p2;
				 for (int i3 = 0; i3<s; i3++)
				 {
				
					 if ((((value[pos].idx >> (i3 << 3))&((1 << 8) - 1)) == (i1 | (i2 << 4))) && (value[pos].src[i3] == g1) && (value[pos].dst[i3] == g2))
					 {
					 	delete[] tmp1;
					 	delete[] tmp2;
						 return true;
					 }
				 }
			 }
		 }
		 unsigned int k1 = (h1 << f) + g1;
		 bool find = true;
		 if (find)
		 {
			 map<int, int>::iterator it = index.find(k1);
			 if (it != index.end())
			 {
				 int tag = it->second;
				 linknode* node = buffer[tag];
				 while (node != NULL)
				 {
					 if (node->key != k1)
					 {
						 int val = node->key;
						 int temp_h = (val) >> f;
						 int tmp = pow(2, f);
						 int temp_g = (val%tmp);
						 if ((temp_h == h2) && (temp_g == g2))
							 {
							 delete[] tmp1;
							 delete[] tmp2;
							 return true;
							} 
						 IT = checked.find(val);
						 if (IT == checked.end())
						 {
							 mapnode temp_e;
							 temp_e.h = temp_h;
							 temp_e.g = temp_g;
							 q.push(temp_e);
							 checked[val] = true;;
						 }
					 }
					 node = node->next;
				 }
			 }
		 }
		 for (int i1 = 0; i1<r; i1++)
		 {
			 int p1 = (h1 + tmp1[i1]) % w;
			 for (int i2 = 0; i2<w; i2++)
			 {
				 int pos = p1*w + i2;
				 for (int i3 = 0; i3<s; i3++)
				 {
		
				
					 if (value[pos].src[i3] == g1 && (((value[pos].idx >> (i3 << 3))&((1 << 4) - 1)) == i1))
					 {
						 int tmp_g = value[pos].dst[i3];
						 int tmp_s = ((value[pos].idx >> ((i3 << 3) + 4))&((1 << 4) - 1));
				
						 int shifter = tmp_g;
						 for (int v = 0; v < tmp_s; v++)
							 shifter = (shifter*timer + prime) % bigger_p;
						 int tmp_h = i2;
						 while (tmp_h < shifter)
							 tmp_h += w;
						 tmp_h -= shifter;


						 unsigned int val = (tmp_h << f) + tmp_g;
					 
						 IT = checked.find(val);
						 if (IT == checked.end())
						 {
						
							 mapnode tmp_e;
							 tmp_e.h = tmp_h;
							 tmp_e.g = tmp_g;
							 q.push(tmp_e);
							 checked[val] = true;
						
						 }
					 }
					
				 }
			 }
		 }
		 delete[] tmp1;
		 delete[] tmp2;
		 q.pop();
	 }
	 return false;
 }
/*type 0 is for successor query, type 1 is for precusor query*/
int label_GSS::nodeValueQuery(int id1, int type)
{
	stringstream ss1;
	ss1<<id1; 
	string s1=ss1.str();
	int weight = 0;
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	int tmp = pow(2, f) - 1;
	int g1 = hash1 & tmp;
	if (g1 == 0) g1 += 1;
	int h1 = (hash1 >> f) % w;
	int* tmp1 = new int[r];
	tmp1[0] = g1;
	for (int i = 1; i < r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
	}
	for (int i = 0; i < r; i++)
	{
		int p1 = (h1 + tmp1[i]) % w;
		for (int k = 0; k < w; k++)
		{
			if (type == 0)/*successor query*/
			{
				int pos = p1*w + k;
				for (int j = 0; j < s; ++j)
				{
					if (type == 0 && (((value[pos].idx >> ((j << 3)))&((1 << 4) - 1)) == i) && (value[pos].src[j] == g1))
					{
						weight += value[pos].weight[j];
					}
				}
			}
			else if (type == 1)/*precursor query*/
			{
				int pos = p1 + k*w;
				for (int j = 0; j < s; ++j)
				{
					if (type == 1 && (((value[pos].idx >> ((j << 3) + 4))&((1 << 4) - 1)) == i) && (value[pos].dst[j] == g1))
					{
						weight += value[pos].weight[j];
					}
				}
			}
		}
	}
	if (type == 0)
	{
		unsigned int k1 = (h1 << f) + g1;
		map<int, int>::iterator it = index.find(k1);
		if (it != index.end())
		{
			int tag = it->second;
			linknode* node = buffer[tag];
			node = node->next;
			while (node != NULL)
			{
				weight += node->weight;
				node = node->next;
			}
		}
	}
	else if (type==1)
	{
		unsigned int k1 = (h1 << f) + g1;
		for (map<int, int>::iterator it = index.begin(); it != index.end(); ++it)
		{
			int tag = it->second;
			linknode* node = buffer[tag];
			node = node->next;
			while (node != NULL)
			{
				if(node->key == k1)
					weight += node->weight;
				node = node->next;
			}
		}
	}
	return weight;
}
/*type 0 is for successor query, type 1 is for precusor query*/
int label_GSS::countIDnums(unsigned int hash)
{
	int num=0;
	hashTableNode<labelednode>* np=mapTable.table[hash%TABLESIZE];
	for(;np!=NULL;np=np->next)
	{
		if(np->key==hash)
			num++;
	}
	return num;
}
int label_GSS::nodeDegreeQuery(int id1, int type)
{
	stringstream ss1;
	ss1<<id1; 
	string s1=ss1.str();
	int degree = 0;
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	int tmp = pow(2, f) - 1;
	int g1 = hash1 & tmp;
	if (g1 == 0) g1 += 1;
	int h1 = (hash1 >> f) % w;
	int* tmp1 = new int[r];
	tmp1[0] = g1;
	for (int i = 1; i < r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
	}
	for (int i = 0; i < r; i++)
	{
		int p1 = (h1 + tmp1[i]) % w;
		for (int k = 0; k < w; k++)
		{
			if (type == 0)/*successor query*/
			{
				int pos = p1*w + k;
				for (int j = 0; j < s; ++j)
				{
					if ((((value[pos].idx >> ((j << 3)))&((1 << 4) - 1)) == i) && (value[pos].src[j] == g1))
					{
						++degree;
					}
				}
			}
			else if (type == 1)/*precursor query*/
			{
				int pos = p1 + k*w;
				for (int j = 0; j < s; ++j)
				{
					if ((((value[pos].idx >> ((j << 3) + 4))&((1 << 4) - 1)) == i) && (value[pos].dst[j] == g1))
					{
						++degree;
					}
				}
			}
		}
	}

	if (type == 0)
	{
		unsigned int k1 = (h1 << f) + g1;
		map<int, int>::iterator it = index.find(k1);
		if (it != index.end())
		{
			int tag = it->second;
			linknode* node = buffer[tag];
			node = node->next;
			while (node != NULL)
			{
				++degree;
				node = node->next;
			}
		}
	}
	else if (type == 1)
	{
		unsigned int k1 = (h1 << f) + g1;
		for (map<int, int>::iterator it = index.begin(); it != index.end(); ++it)
		{
			int tag = it->second;
			linknode* node = buffer[tag];
			node = node->next;
			while (node != NULL)
			{
				if (node->key == k1)
					++degree; 
				node = node->next;
			}
		}
	}
	return degree;
}
void hvinsert(unsigned int hash, std::vector<hashvalue> &v)
{
	bool find=false;
	for(int i=0;i<v.size();i++)
	{
		if (hash==v[i].key)
		{
			v[i].IDnum++;
			find=true;
			break;
		}
	}
	if(!find)
	{
		hashvalue hv;
		hv.key=hash;
		hv.IDnum=1;
		v.push_back(hv);
	}
	return;
}
int GSS::TriangleCounting()
{
	GSketch* gs = new GSketch;
	for(int i=0;i<tablesize;i++)
	{
		hashTableNode<string> *np=mapTable.table[i];
		vector<hashvalue> nodes;
		for(;np!=NULL;np=np->next)
		 	hvinsert(np->key,nodes);
		int nodenum=nodes.size();
		for(int j=0;j<nodenum;j++)
		{
			//cout<<nodes[j].key<<' '<<nodes[j].IDnum<<endl;
			gs->insert_node(nodes[j].key, nodes[j].IDnum);
		}
		nodes.clear();
	}
	for(int i=0;i<w;i++)
	{
		int row=i*w;
		for(int k=0;k<w;k++)
		{
			int pos=row+k;
			for(int j=0;j<s;j++)
			{
				if(value[pos].src[j]>0)
				{
					unsigned int tmp_g1 = value[pos].src[j];
					unsigned int tmp_s1 = ((value[pos].idx >> (j << 3))&((1 << 4) - 1));
					
					int shifter = tmp_g1;
					for (int v = 0; v < tmp_s1; v++)
						shifter = (shifter*timer + prime) % bigger_p;
					int tmp_h1 = i;
					while (tmp_h1 < shifter)
						tmp_h1 += w;
					tmp_h1 -= shifter;
						 
					unsigned int val1 = (tmp_h1 << f) + tmp_g1;
					
					 int tmp_g2 = value[pos].dst[j];
					 int tmp_s2 = ((value[pos].idx >> ((j << 3) + 4))&((1 << 4) - 1));
				
					 shifter = tmp_g2;
					 for (int v = 0; v < tmp_s2; v++)
						shifter = (shifter*timer + prime) % bigger_p;
					  int tmp_h2 = k;
					  while (tmp_h2 < shifter)
							 tmp_h2 += w;
					  tmp_h2 -= shifter;
					  unsigned int val2 = (tmp_h2 << f) + tmp_g2;
					  gs->insert_edge(val1,val2);
				}
			}
		}
	}
	for(int i=0;i<buffer.size();i++)
	{
		linknode* np=buffer[i];
		unsigned int src=np->key;
		np=np->next;
		for(;np!=NULL;np=np->next)
		{
		  unsigned int dst=np->key;
		  gs->insert_edge(src, dst);
		}
	}
	gs->clean();
	return gs->countTriangle();
	delete gs;
}
void label_GSS::graph_match(vector<graph*> &gq,string file, vector<int> &matchcount)
{ 
	graph* gd =new graph;
	int nodenum=0;
	for(int i=0;i<TABLESIZE;i++)
	{
		hashTableNode<labelednode> *np = mapTable.table[i];
		for(;np!=NULL;np=np->next)
		{
			/*labelednode ln = np->value;
		//	cout<<"node "<<ln.ID<<' '<<ln.label<<endl;
			gd->insert_node(ln.ID,ln.label);
			nodenum++;*/
			nodenum++;
		}
		delete np;	
	}
	for(int i=0;i<nodenum;i++)
		gd->insert_node(i,1);
	for(int i=0;i<nodenum;i++)
	{
		int ID = i;
		vector<labeled_Successor> successor;
		nodeSuccessorQuery(ID, successor);
		for(int j=0;j<successor.size();j++)
		{
			//cout<<"edge: "<<ID<<' '<<successor[j].Successor<<' '<<successor[j].edge_label<<endl;
			gd->insert_edge(ID,successor[j].Successor, successor[j].edge_label); 
		}
	}
		
	for(int j=0;j<gq.size();j++){
	simple_match* sm = new simple_match(gq[j],gd,file,j,0);
	sm->match(0);
	matchcount.push_back(sm->count);
	delete sm;
}
delete gd;
}



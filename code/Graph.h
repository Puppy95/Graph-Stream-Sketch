#include<iostream>
#include<string>
#include<fstream>
#include<map>
#include<vector>
#include<queue>
using namespace std;
struct edge
{
	int n;
	edge* next;
	int label;
	int weight;
};
class graph
{
	private:
		int n;
		int max_d;
		int N;
		
	public:
		int chong;
		vector<string> node;
		map<string, int> index;
		map<string, int> degree;
		map<string, int> weight;

		vector<edge*> g;
		graph();
		~graph(){
		vector<string>().swap(node);
		vector<edge*>().swap(g);
		index.clear();
		}
		void insert(string s1, string s2,int label,int weight);
		int query(string s1, string s2, int label);
		int	get_nodenum();
		int get_edgenum();
		int get_max_d();
		bool transquery(string s1, string s2, int label);
		int nodequery(string s1,int label,int type);
		int nodeValueQuery(string s1, int label, int type);
		void allNodeQuery(map<string, int>& successor, map<string, int>& presussor, int label);
 }; 
 graph::graph()
 {
 	n=0;
 	max_d=0;
 	N=0;
 }
int graph::get_max_d()
 {
 	return max_d;
 }
 int graph::get_nodenum()
 {
 	return n;
 }
 int graph::get_edgenum()
 {
 	return N;
 }
 void graph::insert(string s1, string s2, int l,int w)
 {
 		int n1, n2;
 		map<string,int>::iterator it;
 		it = index.find(s1);
		if(it!=index.end())
			n1 = it->second;
		else
		{
			node.push_back(s1);
			index[s1]=n;
			n1=n;
			n++;
			edge *e=new edge;
			e->next = NULL;
			e->n = n1;
			e->weight = 0;
			e->label = 0;
			g.push_back(e); 
		}

		//it = degree.find(s1);
		//if (it != degree.end())
		//	++it->second;
		//else
		//	degree[s1] = 1;

	/*	it = weight.find(s1);
		if (it != weight.end())
			it->second += w;
		else
			weight[s1] = w;*/

		it = index.find(s2);
		if(it!=index.end())
			n2 = it->second;
		else
		{
			node.push_back(s2); 
			index[s2]=n;
			n2=n;
			n++;
			edge *e=new edge;
			e->next = NULL;
			e->n = n2;
			e->weight = 0;
			e->label = 0;
			g.push_back(e);
		}

		edge* e=g[n1];
		int length=0;
		bool find = false;
		while(true)
		{
			length++;
			if(e->n==n2&&e->label==l)
			{
				e->weight+=w;
				find = true;
				break;
			}
			if(e->next==NULL)
				break;
			e=e->next;
		}
		if(!find)
		{
			it = degree.find(s1);
			if (it != degree.end())
				++it->second;
			else
				degree[s1] = 1;
			N++;
			edge* f=new edge;
			f->label=l;
			f->n=n2;
			f->weight=w;
			f->next=NULL;
			e->next=f;
			length++;
			if(length>max_d)
				max_d=length;
		}	
 }
 int graph::query(string s1, string s2,int label)
 {
 	map<string,int>::iterator it;
 	int n1,n2;
 	it = index.find(s1);
	if(it!=index.end())
		n1 = it->second;
	else return 0;
	it = index.find(s2);
	if(it!=index.end())
		n2 = it->second;
	else return 0;
	edge* e = g[n1];
	while(e!=NULL)
		{
			if(e->n==n2&&e->label==label)
				return e->weight;
			e = e->next;
		}
		if(e==NULL)
 		return 0;
 }
bool graph::transquery(string s1, string s2, int label) 
{
	int n1 = index[s1];
	int n2 = index[s2];

//	cout<<n1<<' '<<n2<<endl;
	
	edge* e;
    queue<int> q;
	q.push(n1);
	int s = n;
	bool* checked=new bool[s];
	for(int i=0;i<s;i++)
		checked[i]=false;
	checked[n1]=true;
	while(!q.empty())
	{
		int n=q.front();
		e = g[n];
		while(e!=NULL)
		{
			if(checked[e->n]==false&&e->label==label)
			{
				q.push(e->n);
				//cout<<"in!";
				checked[e->n]=true;
			}
			if(e->n==n2)
				return true;
			e=e->next;
		}
		q.pop();
		//if(q.empty())
		//	cout<<"found!"<<endl
	}
	return false;
}
//type = 0 is successor query ,type =1 is precursor query 
int graph::nodequery(string s1,int label,int type)
{
	int degree = 0;
	if (type = 0)
	{
		map<string, int>::iterator it;
		int n1;
		it = index.find(s1);
		if (it != index.end())
			n1 = it->second;
		else return 0;
		edge* e = g[n1];
		e = e->next;
		while (e != NULL)
		{
			if (e->label == label)
				++degree;
			e = e->next;
		}
	}
	else if (type == 1)
	{   
		map<string, int>::iterator it;
		int n2;
		it = index.find(s1);
		if (it != index.end())
			n2 = it->second;
		else return 0;
		vector<edge*>::iterator IT = g.begin();
		vector<edge*>::iterator END = g.end();
		for (; IT != END; ++IT)
		{
			edge* e = *IT;
			e = e->next;
			while (e != NULL)
			{
				if (e->n == n2)
					++degree;
				e = e->next;
			}
		}
	}
	return degree;
}
int graph::nodeValueQuery(string s1, int label, int type)
{
	int weight = 0;
	if (type = 0)
	{
		map<string, int>::iterator it;
		int n1;
		it = index.find(s1);
		if (it != index.end())
			n1 = it->second;
		else return 0;
		edge* e = g[n1];
		e = e->next;
		while (e != NULL)
		{
			if (e->label == label)
				weight+=e->weight;
			e = e->next;
		}
	}
	else if (type == 1)
	{
		map<string, int>::iterator it;
		int n2;
		it = index.find(s1);
		if (it != index.end())
			n2 = it->second;
		else return 0;
		vector<edge*>::iterator IT = g.begin();
		vector<edge*>::iterator END = g.end();
		for (; IT != END; ++IT)
		{
			edge* e = *IT;
			e = e->next;
			while (e != NULL)
			{
				if (e->n == n2)
					weight += e->weight;
				e = e->next;
			}
		}
	}
	return weight;
}
void graph::allNodeQuery(map<string, int>& successor, map<string, int>& presussor, int label)
{
	map<int, int> idx2successor;
	map<int, int> idx2precussor;
	vector<edge*>::iterator IT = g.begin();
	vector<edge*>::iterator END = g.end();
	for (; IT != END; ++IT)
	{
		edge* e = *IT;
		int head = e->n;

		map<int, int>::iterator it;

		it = idx2successor.find(head);
		if (it == idx2successor.end())
			idx2successor[head] = 0;
		e = e->next;
		while (e != NULL)
		{
			idx2successor[head] += 1;
			it = idx2precussor.find(e->n);
			if (it == idx2precussor.end())
				idx2precussor[e->n] = 0;
			idx2precussor[e->n] += 1;
			e = e->next;
		}
	}
	map<string, int>::iterator str2idx = index.begin();
	map<string, int>::iterator str2idxEND = index.end();
	for (;str2idx!= str2idxEND; str2idx++)
	{
		successor[str2idx->first] = idx2successor[str2idx->second];
		presussor[str2idx->first] = idx2precussor[str2idx->second];
	}
	return;
}
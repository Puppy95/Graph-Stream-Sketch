#include<iostream>
#include<map>
#include<vector>
#include<string>
#include<algorithm>
#include"Graph.h"
#include<stack>
#include<assert.h>
#include<fstream>
using namespace std;
struct neighbor
{
	int id;
	int count;
};
bool contain(set<int>& n1, set<int> &n2)
{
	set<int>::iterator it1;
	set<int>::iterator it2;
	for(it2=n2.begin();it2!=n2.end();it2++)
	{
		bool find=false;
		for(it1=n1.begin();it1!=n1.end();it1++)
		{
			if(*it1==*it2)
			{
				find=true;
				break;
			}
		}
		if(!find)
			return false;
	}
	return true;
}
class state
{
	public:
		int *qtod;// array to store the map function from the query graph to the data graph
		int *dtoq;// reverse
		vector<neighbor*> q_in;// nodes not in state, but is the predecessors of nodes in state int the query graph
		vector<neighbor*> q_out; // the vector to store nodes that is not in the mapping of this state yet, but is the successor of nodes in state in the query graph
		vector<neighbor*> d_in;
		vector<neighbor*> d_out;
		graph* qg;
		graph* dg;
		state(graph *q, graph*d)
		{
			qg=q;
			dg=d;
			int qs=q->Node.size();
			int ds=d->Node.size();
			qtod=new int[qs];
			dtoq=new int[ds];
			for(int i=0;i<qs;i++)
				qtod[i]=-1;
			for(int i=0;i<ds;i++)
				dtoq[i]=-1; 
		}
		bool try_add(int n, int m)
		{
			node n1=  qg->Node[qg->nodemap[n]];
			node n2=dg->Node[dg->nodemap[m]];
			
			bool print=false;
			/*if(n==11 &&m==879)
				print=true;*/

			if(n1.label!=n2.label)
				return false; 
			if((qtod[n]>=0)||(dtoq[m]>=0))
				return false;;
			if(n1.pre.size()>n2.pre.size())
				return false;
			if(n1.suc.size()>n2.suc.size())
				return false; 
			int sum_in_p_q=0; //used to record the size of the intersection of the predecessor set of n1 and the q_in
			int sum_in_s_q=0;//record the size of the intersection of successor set of n1 and q_in; the following is similar;
			int sum_out_p_q=0;
			int sum_out_s_q=0;
			int sum_in_p_d=0;
			int sum_in_s_d=0;
			int sum_out_p_d=0;
			int sum_out_s_d=0;
			for(int i=0;i<n1.pre.size();i++)
			{
				if(qtod[n1.pre[i].node]!=-1)
				{
					int pr=qtod[n1.pre[i].node];
					set<int> labels;
					n2.has_pre(pr, labels);
			
					if(!contain(labels,n1.pre[i].label))
						return false;
				}
			}
			for (int i=0;i<n1.suc.size();i++)
			{
				if(qtod[n1.suc[i].node]!=-1)
				{
					int su=qtod[n1.suc[i].node];
					set<int> labels;
					n2.has_suc(su, labels);
		
					if(!contain(labels,n1.suc[i].label))
						return false;
		
				}
			}
			if(print) cout<<"here1"<<endl;
				// there is a difference between induced substructure and embedded substructure in subgraph isomorphism, if it is induced subgraph, here we should 
			// carry out a similar procedure to check if every mapper node of predecessors of n2 in the data graph is predecessor of n1. Here I use embedded graph. 
			for(int i=0;i<q_in.size();i++)
			{
				int num=q_in[i]->id;
				set<int> labels;
				n1.has_pre(num,labels);
				if(labels.size()>0)
					sum_in_p_q++;
				labels.clear();
				assert(labels.size()==0);
				n1.has_suc(num,labels);
				if(labels.size()>0)
					sum_in_s_q++;
			}
			for(int i=0;i<d_in.size();i++)
			{
				int num=d_in[i]->id;
				set<int> labels;
				n2.has_pre(num,labels);
					if(print) cout<<num<<' '<<labels.size()<<endl;
				if(labels.size()>0)
					sum_in_p_d++;
				labels.clear();
				assert(labels.size()==0);
				n2.has_suc(num,labels);
				if(labels.size()>0)
					sum_in_s_d++;
			}	//calculate the size of the intersection of the precesssors of n1 and q_in, and the intersection of the precessors of n2 and d_in, if the formor is larger than
			// the latter, cut this branch;
			if(print)
				cout<<sum_in_p_d<<' '<<sum_in_p_q<<' '<<sum_in_s_d<<' '<<sum_in_s_q<<endl;
			if(sum_in_p_d<sum_in_p_q)
				return false;
			if(sum_in_s_d<sum_in_s_q)
				return false;
				
			if(print) cout<<"here2"<<endl;
			for(int i=0;i<q_out.size();i++)
			{
				int num=q_out[i]->id;
				set<int> labels;
				n1.has_pre(num,labels);
				if(labels.size()>0)
					sum_out_p_q++;
				labels.clear();
				assert(labels.size()==0);
				n1.has_suc(num,labels);
				if(labels.size()>0)
					sum_out_s_q++;
			}
			for(int i=0;i<d_out.size();i++)
			{
				int num=d_out[i]->id;
				set<int> labels;
				n2.has_pre(num,labels);
				if(labels.size()>0)
					sum_out_p_d++;
				labels.clear();
				assert(labels.size()==0);
				n2.has_suc(num,labels);
				if(labels.size()>0)
					sum_out_s_d++;
			}//calculate the size of the intersection of the successsors of n1 and q_out, and the intersection of the successors of n2 and d_out, if the formor is larger than
			// the latter, cut this branch;
			if(sum_out_p_d<sum_out_p_q)
				return false;
			if(sum_out_s_d<sum_out_s_q)
				return false;
			/*if(n2.pre.size()-sum_in_p_d-sum_out_p_d<n1.pre.size()-sum_in_p_q-sum_out_p_q)
				return false;
			if(n2.suc.size()-sum_in_s_d-sum_out_s_d<n1.suc.size()-sum_in_s_q-sum_out_s_q)
				return false;*/
			if(print) cout<<"here3"<<endl;
			qtod[n]=m;
			dtoq[m]=n; // store the map
			vector<neighbor*>::iterator it;
			 // remove n1 from q_in and q_out if it is in them, and add the unmapped precessors of n1 into q_in, the unmapped successors of n1 into q_out
			 for(it=q_in.begin();it!=q_in.end();it++)
			 {
			 	if((*it)->id==n)
			 	{
			 		q_in.erase(it);
			 		break;
				 }
			 }
			 for(it=q_out.begin();it!=q_out.end();it++)
			 {
			 	if((*it)->id==n)
			 	{
			 		q_out.erase(it);
			 		break;
				 }
			 }
			for(int i=0;i<n1.pre.size();i++)
			{
				if(qtod[n1.pre[i].node]>=0)
					continue;
				bool find=false;
				for(it=q_in.begin();it!=q_in.end();it++)
				{
					if(n1.pre[i].node==(*it)->id)
					{
						find =true;
						(*it)->count++;
						break;
					}
				}
				if(!find)
				{
					neighbor* ne = new neighbor;
					ne->id=n1.pre[i].node;
					ne->count=1;
					q_in.push_back(ne);
				}
			 }
			 for(int i=0;i<n1.suc.size();i++)
			{
				if(qtod[n1.suc[i].node]>=0)
					continue;
				bool find=false;
				for(it=q_out.begin();it!=q_out.end();it++)
				{
					if(n1.suc[i].node==(*it)->id)
					{
						find =true;
						(*it)->count++;
						break;
					}
				}
				if(!find)
				{
					neighbor* ne = new neighbor;
					ne->id=n1.suc[i].node;
					ne->count=1;
					q_out.push_back(ne);
				}
			 }
			  // remove n2 from d_in and d_out if it is in them, and add the unmapped precessors of n2 into d_in, the unmapped successors of n1 into d_out
			for(it=d_in.begin();it!=d_in.end();it++)
			 {
			 	if((*it)->id==m)
			 	{
			 		d_in.erase(it);
			 		break;
				 }
			 }
			 for(it=d_out.begin();it!=d_out.end();it++)
			 {
			 	if((*it)->id==m)
			 	{
			 		d_out.erase(it);
			 		break;
				 }
			 }
			 for(int i=0;i<n2.pre.size();i++)
			{
				if(dtoq[n2.pre[i].node]>=0)
					continue;
				bool find=false;
				for(it=d_in.begin();it!=d_in.end();it++)
				{
					if(n2.pre[i].node==(*it)->id)
					{
						find =true;
						(*it)->count++;
						break;
					}
				}
				if(!find)
				{
					neighbor* ne = new neighbor;
					ne->id=n2.pre[i].node;
					ne->count=1;
					d_in.push_back(ne);
				}
			 }
			 for(int i=0;i<n2.suc.size();i++)
			{
				if(dtoq[n2.suc[i].node]>=0)
					continue;
				bool find=false;
				for(it=d_out.begin();it!=d_out.end();it++)
				{
					if(n2.suc[i].node==(*it)->id)
					{
						find =true;
						(*it)->count++;
						break;
					}
				}
				if(!find)
				{
					neighbor* ne = new neighbor;
					ne->id=n2.suc[i].node;
					ne->count=1;
					d_out.push_back(ne);
				}
			 }
			 return true;
		}
		void del_map(int n, int m)//the function to delete map(n,m)
		{
			qtod[n]=-1;
			dtoq[m]=-1;// delete the map
			node n1=qg->Node[n];
			node n2=dg->Node[m];
			//find n1 precessors in q_in, and successors in q_out, decrease their count by 1; if the count is 0, it means this node is no more a precessor or successor of the mapped nodes,
			//remove it;
			int sum=0;
			for(int i=0;i<n1.pre.size();i++)
			{ 
				if(qtod[n1.pre[i].node]>=0)
				{	sum++;
					continue;
				}
				vector<neighbor*>::iterator it;
				for(it=q_in.begin();it!=q_in.end();it++)
				{
					if((*it)->id==n1.pre[i].node)
					{
						(*it)->count--;
						if((*it)->count==0)
						{
							q_in.erase(it);
							it--;
						}
					} 
				}
			}
			if(sum>0)//if there are mapped nodes as the precessor of n, n can be added into q_out;
			{neighbor* ne =new neighbor;
			 ne->id=n;
			 ne->count=sum;
			 q_out.push_back(ne);
			}
			sum=0;			
			for(int i=0;i<n1.suc.size();i++)
			{
				if(qtod[n1.suc[i].node]>=0)
				{	sum++;
					continue;
				}
				vector<neighbor*>::iterator it;
				for(it=q_out.begin();it!=q_out.end();it++)
				{
					if((*it)->id==n1.suc[i].node)
					{
						(*it)->count--;
						if((*it)->count==0);
						{
							q_out.erase(it);
							it--;
						}
					}
				}
			}
			if(sum>0)//if there are mapped nodes as the successor of n, n can be added into q_in;
			{neighbor* ne =new neighbor;
			 ne->id=n;
			 ne->count=sum;
			 q_in.push_back(ne);
			}
			sum=0;
			//similar operation to n2;
			for(int i=0;i<n2.pre.size();i++)
			{
				if(dtoq[n2.pre[i].node]>=0)
				{	sum++;
					continue;
				}
				vector<neighbor*>::iterator it;
				for(it=d_in.begin();it!=d_in.end();it++)
				{
					if((*it)->id==n2.pre[i].node)
					{
						(*it)->count--;
						if((*it)->count==0)
						{ 
							d_in.erase(it);
							it--;
						}
					}
				}
			}
			if(sum>0)//if there are mapped nodes as the precessor of m, m can be added into d_out;
			{neighbor* ne =new neighbor;
			 ne->id=m;
			 ne->count=sum;
			 d_out.push_back(ne);
			}
			sum=0;
			for(int i=0;i<n2.suc.size();i++)
			{
				if(dtoq[n2.suc[i].node]>=0)
				{
					sum++;
					continue;
				}
				vector<neighbor*>::iterator it;
				for(it=d_out.begin();it!=d_out.end();it++)
				{
					if((*it)->id==n2.suc[i].node)
					{
						(*it)->count--;
						if((*it)->count==0)
						{
							d_out.erase(it);
							it--;
						}
					}
				}
			}
			if(sum>0)//if there are mapped nodes as the successor of m, m can be added into d_in;
			{neighbor* ne =new neighbor;
			 ne->id=m;
			 ne->count=sum;
			 d_in.push_back(ne);
			}
		}
		
};

class simple_match
{
	public:
		graph* q;
		graph* d;
		state* s;
		string f;
		int count;
		int q_num;
		int d_num;
	simple_match(graph* qg, graph* dg, string file, int qnum, int dnum)
	{
		q=qg;
		d=dg;
		s=new state(q,d);
		f=file;
		q_num=qnum;
		d_num=dnum;
		count=0;
	 }
	 void output()
	 {
	 	ofstream fout((const char*)(f.c_str()),ios::app);
	 	if(count==0)
			fout<<"Query_graph: "<<q_num<<", Data_graph:"<<d_num<<endl;
		fout<<"map "<<count<<endl;
		for(int i=0;i<q->Node.size();i++)
			fout<<i<<' '<<s->qtod[i]<<endl;
		count++;
	 }
	 void match(int n)
	 {
	 	if(count>1000)
	 		return;
	 	int i;
	 	for(i=0;i<d->Node.size();i++)
	 	{ 
	 	if(s->try_add(n, i))
	 	{
	 		int next=-1;
	 		for(int j=0;j<q->Node.size();j++)
	 		{
	 			if(q->Node[j].id==n)
				{
					node n1=q->Node[j];
					for(int k=0;k<n1.suc.size();k++)
					{
						if(s->qtod[n1.suc[k].node]<0)
						{
							next=n1.suc[k].node;
							break;
						}
					}
					if(next<0)
					{
						for(int k=0;k<n1.pre.size();k++)
					{
						if(s->qtod[n1.pre[k].node]<0)
						{
							next=n1.pre[k].node;
							break;
						}
					}
					}
					break;
				}	 
			 }
			 if(next<0)
			 {
			 	for(int j=0;j<q->Node.size();j++)
			 	{
			 		if(s->qtod[q->Node[j].id]<0)
			 		{
			 			next=q->Node[j].id;
			 			break;
					 }
				 }
			  }
			if(next<0)
				output();
			else 
	 		 	match(next);
	 		s->del_map(n,i);
		}
		//cout<<s->try_add(n,0); 
		} 
	 }
};
	

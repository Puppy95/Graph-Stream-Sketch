#include<string>
#include<vector>
#include<algorithm>
#include "hashfunction.h"
using namespace std;
unsigned int (*hfunc[14])(const unsigned char*, unsigned int)=
{
    BOB1,
	BOB2,
	BOB3,
	BOB4,
    PJW,
    DJB,
    CRC32,
    OCaml,
    SML,
    STL,
    FNV32,
    Hsieh,
    RSHash	
};
struct  hashvalue
{
	unsigned int key;
	int IDnum;
};
bool mycomp(const hashvalue &hv1, const hashvalue &hv2)
{
	return hv1.key<hv2.key;
}
bool operator==(const hashvalue &hv1, const hashvalue &hv2)
{
	return hv1.key==hv2.key;
}
int countjoin( vector<hashvalue> &V1, vector<hashvalue> &V2)
{
 int i1=0,i2=0;
 int count=0;
 while(i1<V1.size())
 {
 	if (i2>=V2.size()) return count;
 	while(V2[i2].key<V1[i1].key)
 	{
 		i2++;
 		if (i2>=V2.size()) return count;
 	}
 	if(V2[i2].key==V1[i1].key)
 	{
 		count+=V1[i1].IDnum;
 	    i1++;
 		i2++;
 		continue;
	 }
	else if(V2[i2].key>V1[i1].key)
	{
 			i1++;
	}
  }
  return count; 
}
template<class T>
class hashTableNode
{
public:
	T value;
	unsigned int key;
	hashTableNode<T> *next;
};
template<class T>
class hashTable
{
public:
	hashTableNode<T> **table;
	int tableSize;
	hashTable(int s):tableSize(s)
	{
		table = new hashTableNode<T>*[s];
		memset(table, NULL, tableSize * sizeof(hashTableNode<T>*));
	}
	hashTable()
	{
	}
	void init(int s)
	{
		tableSize = s;
		table = new hashTableNode<T>*[s];
		for(int i=0;i<s;i++)
			table[i]=NULL;
	//	memset(table, NULL, tableSize * sizeof(hashTableNode<T>*));
	}
	~hashTable()
	{
		cleanupHashTable();
		delete [] table;
	}
	void cleanupHashTable()
	{
		hashTableNode<T>*np, *tmp;
		for (int i = 0; i < tableSize; ++i)
		{
			if (table[i] != NULL)
			{
				np = table[i];
				while (np != NULL)
				{
					tmp = np->next;
					delete np;
					np = tmp;
				}
			}
		}
	}
	void insert(unsigned int hash, T value)
	{
		hashTableNode<T> *np;
		bool inTable;
		np = table[hash%tableSize];
		inTable = false;
		for (; np != NULL; np = np->next)
		{
			if (np->key == hash && np->value == value)
			{
				inTable = true;
				break;
			}
		}
		if (!inTable)
		{
			hashTableNode<T>* newNode = new hashTableNode<T>;
			newNode->key = hash;
			newNode->value = value;
			newNode->next = table[hash%tableSize];
			table[hash%tableSize] = newNode;
		}
	}
	void getID(unsigned int hash, vector<T>&IDs) 
	{
		hashTableNode<T> *np;
		np=table[hash%tableSize];
		for(;np != NULL; np=np->next)
		{
			if(np->key==hash)
			{
				IDs.push_back(np->value);
			}
		}
		return;
	}
	int countIDnums(unsigned int hash)
{
	int num=0;
	hashTableNode<T> *np;
	np=table[hash%tableSize];
	for(;np!=NULL;np=np->next)
	{
		if(np->key==hash)
			num++;
	}
	return num;
}
};


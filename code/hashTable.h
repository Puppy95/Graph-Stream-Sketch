#include<string>
using namespace std;
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
		memset(table, NULL, tableSize * sizeof(hashTableNode<T>*));
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
};
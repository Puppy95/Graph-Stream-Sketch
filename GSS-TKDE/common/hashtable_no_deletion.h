#pragma once
#include<string>
#include<vector>
#include<algorithm>
#include<unordered_map>
#ifndef Hash
#include "hashfunction.h"
#define Hash
#endif
using namespace std;
#define MAX_INT 0xFFFFFFFF
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
	hashTableNode<T> *table;
	int tableSize;
	hashTable(int s):tableSize(s)
	{
		table = new hashTableNode<T>[s];
		for (int i = 0; i < s; i++) {
			table[i].key = MAX_INT;
			table[i].next = NULL;
		}
	}
	hashTable()
	{
		table = NULL;
		tableSize = 0;
	}
	int get_mem()
	{
		int mem = 4+8;
		for (int i = 0; i < tableSize; i++)
		{
			hashTableNode<T>* tmp = &table[i];
			while (tmp)
			{
				mem += 12 + sizeof(T);
				tmp = tmp->next;
			}
		}
		return mem;
	}
	void init(int s)
	{
		tableSize = s;
		table = new hashTableNode<T>[s];
		for (int i = 0; i < s; i++) {
			table[i].key = MAX_INT;
			table[i].next = NULL;
		}
	//	memset(table, NULL, tableSize * sizeof(hashTableNode<T>*));
	}
	~hashTable()
	{
		cleanupHashTable();
		if(table!=NULL)
			delete [] table;
	}
	void cleanupHashTable()
	{
		hashTableNode<T>*np, *tmp;
		for (int i = 0; i < tableSize; ++i)
		{
			if (table[i].next != NULL)
			{
				np = table[i].next;
				while (np != NULL)
				{
					tmp = np->next;
					delete np;
					np = tmp;
				}
			}
		}
	}
	void new_insert(unsigned int hash, T val)
	{
		unsigned int address = hash % tableSize;
		if (table[address].key == MAX_INT)
		{
			table[address].key = hash;
			table[address].value = val;
			table[address].next = NULL;
		}
		else{
		hashTableNode<T>* newNode = new hashTableNode<T>;
		newNode->key = hash;
		newNode->value = val;
		newNode->next = table[address].next;
		table[address].next = newNode;
		}
	}
	void insert(unsigned int hash, T value)
	{
		hashTableNode<T> *np;
		bool inTable;
		unsigned int address = hash % tableSize;
		np = &table[address];
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
			if (table[address].key == MAX_INT)
			{
				table[address].key = hash;
				table[address].value = value;
				table[address].next = NULL;
			}
			else {
				hashTableNode<T>* newNode = new hashTableNode<T>;
				newNode->key = hash;
				newNode->value = value;
				newNode->next = table[address].next;
				table[address].next = newNode;
			}
		}
	}
	void getID(unsigned int hash, vector<T>&IDs) 
	{
		unsigned int address = hash % tableSize;

		hashTableNode<T> *np;
		np=&table[hash%tableSize];
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
		int sum = 0;
		unsigned int address = hash % tableSize;

		hashTableNode<T>* np;
		np = &table[hash % tableSize];
		for (; np != NULL; np = np->next)
		{
			if (np->key == hash)
			{
				sum++;
			}
		}
		return sum;
}
};


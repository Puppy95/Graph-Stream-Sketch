#pragma once
#include<iostream>
#include<vector>
#include<map>
#include<memory.h> 
#include<algorithm> 
#define initial_size 8

#include<set>
#include<string>

#ifndef Hash
#include "hashfunction.h"
#define Hash
#endif
using namespace std;
unsigned int (*hfunc[14])(const unsigned char*, unsigned int)=
{
    BOB1,
    CRC32,
    OCaml,
    SML,
	BOB2,
	BOB3,
	BOB4,
    PJW,
    DJB,
    STL,
    FNV32,
    Hsieh,
    RSHash	
}; 

struct ID_list
{
	string ID;
	ID_list* next;
	ID_list(string id = "")
	{
		ID = id;
		next = NULL;
	}
};
struct room
{
	unsigned int hash;
	int degree;
	ID_list ID_head;
	room* next;
	room(unsigned int h_ = 0, int d_ = 0)
	{
		hash = h_;
		degree = d_;
		next = NULL;
	}
	room& operator =(room &r)
	{
		hash = r.hash;
		degree = r.degree;
		ID_head = r.ID_head;
		next = r.next;
		return *this;
	}
	int get_mem()
	{
		int mem = 16;
		ID_list* tmp = &ID_head;
		while (tmp)
		{
			mem += 8 + sizeof(string); //note that here we assumpt that all the string IDs are shorter than 16, in this case the string will not allocate new memory in heap but will directly stored in the buffer 
			// of string struct.
			tmp = tmp->next;
		}
		return mem;
	}
	~room()
	{
		ID_list* tmp = ID_head.next;
		ID_list* cur = tmp;
		while (tmp)
		{
			cur = tmp;
			tmp = tmp->next;
			delete cur;
		}
		ID_head.next = NULL;

	}
};
class hashtable
{
public:
	room** table;
	int w;
	hashtable(int w_)
	{
		w = w_;
		table = new room*[w];
		for (int i = 0; i < w; i++)
			table[i] = NULL;
	}
	hashtable()
	{
		w = 0;
		table = NULL;
	}
	int get_mem()
	{
		int mem = 8 + 4;
		for (int i = 0; i < w; i++)
		{
			mem += 8;
			room* tmp = table[i];
			while (tmp)
			{
				mem += tmp->get_mem();
				tmp = tmp->next;
			}
		}
		return mem;
	}
	void init(int w_)
	{
		w = w_;
		table = new room*[w];
		for (int i = 0; i < w; i++)
			table[i] = NULL;
		
	}
	~hashtable()
	{
		for (int i = 0; i<w; i++)
		{
			room* tmp = table[i];
			room* next = tmp;
			while (tmp)
			{
				next = tmp->next;
				delete tmp;
				tmp = next;
			}
		}
		delete[]table;
	}
	void insert(unsigned int h, string key)
	{
		int address = h%w;
		if (!table[address])
		{
			room* tmp = new room;
			tmp->hash = h;
			tmp->ID_head = ID_list(key);
			tmp->degree = 0;
			tmp->next = NULL;
			table[address] = tmp;
		}
		else
		{
			room* tmp = table[address];
			room* p = table[address];
			while (tmp)
			{
				if (tmp->hash == h)	// if the key also matches.
				{
					if (tmp->ID_head.ID == "")
					{
						tmp->ID_head.ID = key;
						return;
					}
					else
					{
						ID_list* cur = &tmp->ID_head;
						while (cur)
						{
							if (cur->ID == key)
								return;
							cur = cur->next;
						}
					}
					ID_list* new_ID = new ID_list(key);
					new_ID->next = tmp->ID_head.next;
					tmp->ID_head.next = new_ID;
					return;
				}
				p = tmp;
				tmp = tmp->next;
			}
			room* nr = new room;
			nr->hash = h;
			nr->ID_head = ID_list(key);
			nr->degree = 0;
			nr->next = NULL;
			p->next = nr;
			return;
		}

	}
	
	int get_num()
	{
		int count = 0;
		for(int i=0;i<w;i++)
		{
			room* tmp = table[i];
			while(tmp)
			{
				count++;
				tmp = tmp->next;
			}
		}
		return count;
	} 
	void increase(unsigned int h)
	{
		int address = h%w;
		room* tmp = table[address];
		while(tmp)
		{
			if(tmp->hash==h)
			{
				tmp->degree++;
				return;
			}
			tmp = tmp->next;
		}
		return;
	}
	void get_ID(int h, vector<string> &IDs)
	{
		int address = h%w;
		room* tmp = table[address];
		while (tmp)
		{
			if (tmp->hash == h)
			{
				ID_list* cur = &tmp->ID_head;
				while (cur)
				{
					IDs.push_back(cur->ID);
					cur = cur->next;
				}
				return;
			}
			tmp = tmp->next;
		}
		return;
	}
	unsigned int count_ID(int h)
	{
		unsigned int sum =0;
		int address = h%w;
		room* tmp = table[address];
		while (tmp)
		{
			if (tmp->hash == h)
			{
				ID_list* cur = &tmp->ID_head;
				while (cur)
				{
					sum++;
					cur = cur->next;
				}
				return sum;
			}
			tmp = tmp->next;
		}
		return 0;
	}
	bool check(int h)
	{
		int address = h%w;
		room* tmp = table[address];
		while (tmp)
		{
			if (tmp->hash == h)
			{
				return true;
			}
			tmp = tmp->next;
		}

		return false;
	}
	void decrease(unsigned int h)
	{
		int address = h%w;
		if (table[address]->hash == h)
		{
			table[address]->degree--;
			if (table[address]->degree == 0)
			{
				if (table[address]->next)
				{
					room* tmp = table[address];
					table[address] = table[address]->next;
					delete tmp;
					return;
				}
				else
				{
					table[address] = NULL;
					return;
				}
			}
		}
		room* tmp = table[address]->next;
		room* p = table[address];
		while (tmp)
		{
			if (tmp->hash == h)
			{
				tmp->degree--;
				if (tmp->degree == 0)
				{
					p->next = tmp->next;
					delete tmp;
				}
			}
			p = tmp;
			tmp = tmp->next;
		}
		return;
	}

};

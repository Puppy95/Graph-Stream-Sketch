#pragma once
#include<iostream>
#include<vector>
#include<map>
#include<memory.h> 
#include<algorithm> 
#include<set>
#include<string>
#define initial_size 8
#include<time.h>
using namespace std;




struct bs_ID
{
	string ID;
	bs_ID* next;
	bs_ID(string id = "")
	{
		ID = id;
		next = NULL;
	}
};

struct bs_room
{
	unsigned int hash;
	int degree;
	bitset vb;
	bs_ID ID_head;
	bs_room* next;
	bs_room(unsigned int h_ = 0, int d_ = 0)
	{
		hash = h_;
		degree = d_;
		next = NULL;
	}
	~bs_room()
	{
		bs_ID* tmp = ID_head.next;
		bs_ID* cur = tmp;
		while (tmp)
		{
			cur = tmp;
			tmp = tmp->next;
			delete cur;
		}
		ID_head.next = NULL;
	}
	int get_mem()
	{
		int mem = 16 + vb.get_mem();
		bs_ID* tmp = &ID_head;
		while (tmp)
		{
			mem += 8 + sizeof(string); //note that here we assumpt that all the string IDs are shorter than 16, in this case the string will not allocate new memory in heap but will directly stored in the buffer 
			// of string struct.
			tmp = tmp->next;
		}
		return mem;
	}
	bs_room& operator =(bs_room &r)
	{
		hash = r.hash;
		degree = r.degree;
		ID_head = r.ID_head;
		next = r.next;
		vb = r.vb;
		return *this;
	}
};


class hashtable_bs
{
public:
	bs_room** table;
	int w;
	hashtable_bs(int w_)
	{
		w = w_;
		table = new bs_room*[w];
		for (int i = 0; i < w; i++)
			table[i] = NULL;
	}
	~hashtable_bs()
	{
		for (int i = 0; i<w; i++)
		{
			bs_room* tmp = table[i];
			bs_room* next = tmp;
			while (tmp)
			{
				next = tmp->next;
				delete tmp;
				tmp = next;
			}
		}
	}
	int get_mem()
	{
		int mem = 8 + 4;
		for (int i = 0; i < w; i++)
		{
			mem += 8;
			bs_room* tmp = table[i];
			while (tmp)
			{
				mem += tmp->get_mem();
				tmp = tmp->next;
			}
		}
		return mem;
	}
	void increase(unsigned int h, int pos, int r, int direct)
	{
		int address = h%w;
		bs_room* tmp = table[address];
		while(tmp)
		{
			if(tmp->hash==h)
			{
				tmp->degree++;
				tmp->vb.set_bit(pos, r, direct);
				return;
			}
			tmp = tmp->next;
		}
		return;
	}
	void insert(unsigned int h, string key)
	{
		int address = h%w;
		if (!table[address])
		{
			bs_room* tmp = new bs_room;
			tmp->hash = h;
			tmp->ID_head = bs_ID(key);
			tmp->degree = 0;
			tmp->next = NULL;
			table[address] = tmp;
		}
		else
		{
			bs_room* tmp = table[address];
			bs_room* p = table[address];
			while (tmp)
			{
				if (tmp->hash == h)
				{
					if (tmp->ID_head.ID == "")
					{
						tmp->ID_head.ID = key;
						return;
					}
					else
					{
						bs_ID* cur = &tmp->ID_head;
						while (cur)
						{
							if (cur->ID == key)
								return;
							cur = cur->next;
						}
					}
					bs_ID* new_ID = new bs_ID(key);
					new_ID->next = tmp->ID_head.next;
					tmp->ID_head.next = new_ID;
					return;
				}
				p = tmp;
				tmp = tmp->next;
			}
			bs_room* nr = new bs_room;
			nr->hash = h;
			nr->ID_head = bs_ID(key);
			nr->degree = 0;
			nr->next = NULL;
			p->next = nr;
			return;
		}

	}
	void get_ID(unsigned int h, vector<string> &IDs)
	{
		int address = h%w;
		bs_room* tmp = table[address];
		while (tmp)
		{
			if (tmp->hash == h)
			{
				//cout << tmp->bs_ID.size() << endl;
				bs_ID* cur = &tmp->ID_head;
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
	bool check(unsigned int h)
	{
		int address = h%w;
		bs_room* tmp = table[address];
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
	bs_room* get_room(unsigned int h)
	{
		int address = h%w;
		bs_room* tmp = table[address];
		while (tmp)
		{
			if (tmp->hash == h)
				return tmp;
			tmp = tmp->next;
		}
		return NULL;
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
					bs_room* tmp = table[address];
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
		bs_room* tmp = table[address]->next;
		bs_room* p = table[address];
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

	void clear_bit(unsigned int h, int pos, int r, int direct)
	{
		int address = h%w;
		bs_room* tmp = table[address];
		while (tmp)
		{
			if (tmp->hash == h)
			{
				tmp->vb.clear_bit(pos, r, direct);
				return;
			}
			tmp = tmp->next;
		}

		return;
	}

};

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <cmath>
#include <stdlib.h>
#include <bitset>
#include <memory.h>
#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "GraphUndirected.h"
#endif // HASHTABLE_H
using namespace std;
#define prime 739
#define bigger_p 1048576
#define timer 5
#define M 80000

#define Roomnum 2 // This is the parameter to controll the maximum number of rooms in a bucket.

#include "Mapper.h"

struct basket
{
    short src[Roomnum];
    short dst[Roomnum];
    int weight[Roomnum];
    unsigned int idx;
};

struct Buffer
{
    struct LinkNode
    {
        unsigned int v;
        int weight;
        LinkNode *next;
    };

    struct Node {
        unsigned int u;
        LinkNode *firstEdge;
        Node *nextNode;
    };

    Node *node_header;

    Buffer() { node_header = NULL; }
    ~Buffer() {
        Node *cur = node_header;
        while(cur) {
            LinkNode *curEdge = cur->firstEdge;
            while(curEdge) {
                LinkNode *tmp = curEdge;
                curEdge = curEdge->next;
                delete tmp;
            }
            Node *tmp = cur;
            cur = cur->nextNode;
            delete tmp;
        }
    }

    Node* findNode(int u) {
        Node *cur = node_header;
        while (cur && cur->u != u) {
            cur = cur->nextNode;
        }
        return cur;
    }

    void addEdge(int u, int v, int w) {
        Node *nd = findNode(u);
        if (nd) {
            LinkNode *curEdge = nd->firstEdge;
            while(curEdge && curEdge->v != v) {
                curEdge = curEdge->next;
            }
            if (curEdge) {
                curEdge->weight += w;
            } else {
                LinkNode *newEdge = new LinkNode;
                newEdge->v = v;
                newEdge->weight = w;
                newEdge->next = nd->firstEdge;
                nd->firstEdge = newEdge;
            }
        } else {
            Node *newNode = new Node;
            LinkNode *newEdge = new LinkNode;
            newEdge->v = v;
            newEdge->weight = w;
            newEdge->next = NULL;

            newNode->u = u;
            newNode->firstEdge = newEdge;
            newNode->nextNode = node_header;
            node_header = newNode;
        }
    }

    int queryEdge(int u, int v) {
        Node *nd = findNode(u);
        if (nd == NULL) {
            return 0;
        }

        LinkNode *curEdge = nd->firstEdge;
        while(curEdge && curEdge->v != v) {
            curEdge = curEdge->next;
        }

        if (curEdge == NULL) {
            return 0;
        }

        //printf("u=%d v=%d w=%d\n", nd->u, curEdge->v, curEdge->weight);

        return curEdge->weight;
    }

    bool eraseEdge(int u, int v) {
        Node *nd = findNode(u);
        if (nd == NULL) {
            return false;
        }

        LinkNode *curEdge = nd->firstEdge;
        LinkNode *lastEdge = NULL;
        while(curEdge && curEdge->v != v) {
            lastEdge = curEdge;
            curEdge = curEdge->next;
        }

        if (curEdge == NULL) {
            return false;
        }

        if (curEdge == nd->firstEdge) {
            //printf("erase from header : %d %d\n", u, v);
            nd->firstEdge = curEdge->next;
            delete curEdge;
        } else {
            //printf("erase from chain : %d %d\n", u, v);
            lastEdge->next = curEdge->next;
            delete curEdge;
        }

        if (nd->firstEdge == NULL) {
            if (nd == node_header) {
                node_header = nd->nextNode;
                delete nd;
            } else {
                Node *cur = node_header;
                Node *last = NULL;
                while (cur != nd) {
                    last = cur;
                    cur = cur->nextNode;
                }
                last->nextNode = cur->nextNode;
                delete cur;
            }
        }
        return true;
    }

    void queryPrecursor(int u, vector<int>& retID) {
        Node *curNode = node_header;
        while (curNode) {
            LinkNode *curEdge = curNode->firstEdge;
            while (curEdge) {
                if (curEdge->v == u) {
                    retID.push_back(curNode->u);
                    break;
                }
                curEdge = curEdge->next;
            }
            curNode = curNode->nextNode;
        }
    }

    void querySuccessor(int u, vector<int>& retID) {
        Node *nd = findNode(u);
        if (nd) {
            LinkNode *cur = nd->firstEdge;
            while (cur) {
                retID.push_back(cur->v);
                cur = cur->next;
            }
        }
    }

    void print() {
        Node *cur = node_header;
        int cnt = 0;
        puts("<-----------------------------");
        while (cur) {
            printf("+++u=%d:\n", cur->u);
            LinkNode *curEdge = cur->firstEdge;
            while (curEdge) {
                printf("v=%d w=%d\n", curEdge->v, curEdge->weight);
                assert(++cnt < 500);
                curEdge = curEdge->next;
            }
            cur = cur->nextNode;
        }
        puts("----------------------------->");
    }
};

class GSS
{
private:
    int w; // Hash矩阵的大小
    int r; // Hash 序列的长度
    int p; // 从 Hash 序列中选出的候选的序列长度
    int s; // 一个单元中的内容个数
    int f; // 指纹长度

    Mapper *mapper;

    basket *value;

public:
    Buffer *buffer;
    map<unsigned, int> deg;
    GSS(int width, int range, int p_num, int size, int f_num);
    ~GSS()
    {
        delete buffer;
        delete mapper;
        delete[] value;
    }
    void insert(string s1, string s2, int weight);

    int edgeQuery(string s1, string s2);
    void nodeSuccessorQuery(string s1, vector<string> &IDs);
    void nodePrecursorQuery(string s2, vector<string> &IDs);

    bool erase(const string& s1, const string& s2);
private:
    inline void gen_gh(const string& str, unsigned int& g, unsigned int& h, unsigned int& k);
    inline void setRoomVal(int pos, int idx, int room, int src, int dst, int weight);
};

GSS::GSS(int width, int range, int p_num, int size, int f_num)
//the side length of matrix, the length of hash addtress list, the number of candidate bucekt
// the number of rooms, whether to use hash table, and the size of the table.
// Hash table which stores the original nodes can be omitted if not needed. For nodequery,
//  reachability, edgequery not needed. But needed for triangel counting, degree query, and successor / precursor queries.
{
    w = width;
    r = range; /* r x r mapped baskets */
    p = p_num; /*candidate buckets*/
    s = size;  /*multiple rooms*/
    f = f_num; /*finger print lenth*/



    buffer = new Buffer();
    value = new basket[w * w];
    memset(value, 0, sizeof(basket) * w * w);
    //mapper = new Mapper_Naive();
    mapper = new Mapper(w); // g 的值不能等于0，所以要从w开始
}

void GSS::gen_gh(const string& str, unsigned int& g, unsigned int& h, unsigned int& k) {
    //unsigned int tmp = pow(2, f) - 1;
    k = mapper->insert(str);
    if (k == -1)
        k = mapper->getID(str);
    //k = mapper->getID(str);
    g = k / w;
    // while(!g) {
    //     mapper->changeOne(str);
    //     k = mapper->getID(str);
    //     g = k / w;
    // }

    // 地址值域一定要比w小
    h = k % w;

}

inline void GSS::setRoomVal(int pos, int room, int idx, int src, int dst, int weight)
{
    value[pos].idx &= ~( ((1 << 8)-1) << (room << 3));
    value[pos].idx |= (idx << (room << 3));
    value[pos].src[room] = src;
    value[pos].dst[room] = dst;
    value[pos].weight[room] = weight;
}


bool GSS::erase(const string& s1, const string& s2) {
    unsigned int g1, g2, h1, h2, k1, k2;

    gen_gh(s1, g1, h1, k1);
    gen_gh(s2, g2, h2, k2);

//    printf("erase %d %d\n", k1, k2);

    int *tmp1 = new int[r];
    int *tmp2 = new int[r];

    tmp1[0] = g1;
    tmp2[0] = g2;
    for (int i = 1; i < r; i++)
    {
        tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
        tmp2[i] = (tmp2[i - 1] * timer + prime) % bigger_p;
    }

    bool erased = false;
    long key = g1 + g2;
    for (int i = 0; i < p; i++)
    {
        key = (key * timer + prime) % bigger_p;
        int index = key % (r * r);
        int index1 = index / r;
        int index2 = index % r;
        int p1 = (h1 + tmp1[index1]) % w;
        int p2 = (h2 + tmp2[index2]) % w;

        int pos = p1 * w + p2;
        for (int j = 0; j < s; j++)
        {
            int idx = (index1 | (index2 << 4));
            if ((((value[pos].idx >> (j << 3)) & ((1 << 8) - 1)) == idx)
                && (value[pos].src[j] == g1)
                && (value[pos].dst[j] == g2))
            {
                setRoomVal(pos, j, 0, 0, 0, 0); // 将该位置回收
                erased = true;
                break;
            }
        }
        if (erased) break;
    }

    if (erased == false) {
        erased = buffer->eraseEdge(k1, k2);
    }

    if (erased) {
        deg[k1]--;
        deg[k2]--;
        if (deg[k1] == 0) {
            deg.erase(k1);
            //mapper->erase(k1);
            mapper->erase(s1);
        }
        if (deg[k2] == 0 && k1 != k2) {
            deg.erase(k2);
            //mapper->erase(k2);
            mapper->erase(s2);
        }
    }

    delete [] tmp1;
    delete [] tmp2;

//    buffer->print();
    return erased;
}

void GSS::insert(string s1, string s2, int weight)
// s1 is the ID of the source node,
// s2 is the ID of the destination node, weight is the edge weight.
{

    unsigned int g1, g2, h1, h2, k1, k2;

    gen_gh(s1, g1, h1, k1);
    gen_gh(s2, g2, h2, k2);

    //printf("in insert s1 = %s g1 = %u h1 = %u k1 = %u", s1.c_str(), g1, h1, k1);
    //printf("in insert s2 = %s g2 = %u h2 = %u k2 = %u", s2.c_str(), g2, h2, k2);
    // g1, g2 指纹对
    // h1, h2 地址
    // k1, k2 哈希表的键值

    int *tmp1 = new int[r];
    int *tmp2 = new int[r];
    tmp1[0] = g1;
    tmp2[0] = g2;
    for (int i = 1; i < r; i++)
    {
        tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
        tmp2[i] = (tmp2[i - 1] * timer + prime) % bigger_p;
    }


    long key = g1 + g2;
    for (int i = 0; i < p; i++)
    {
        key = (key * timer + prime) % bigger_p;
        int index = key % (r * r);
        int index1 = index / r;
        int index2 = index % r;
        int p1 = (h1 + tmp1[index1]) % w;
        int p2 = (h2 + tmp2[index2]) % w;
        int idx = (index1 | (index2 << 4));

        int pos = p1 * w + p2;

        for (int j = 0; j < s; j++)
        {
            if ((((value[pos].idx >> (j << 3)) & ((1 << 8) - 1)) == idx)
                && (value[pos].src[j] == g1)
                && (value[pos].dst[j] == g2)) {

                value[pos].weight[j] += weight;
                goto FINISHED;
            }
        }
    }

    key = g1 + g2;
    for (int i = 0; i < p; i++)
    {
        key = (key * timer + prime) % bigger_p;
        int index = key % (r * r);
        int index1 = index / r;
        int index2 = index % r;
        int p1 = (h1 + tmp1[index1]) % w;
        int p2 = (h2 + tmp2[index2]) % w;
        int idx = (index1 | (index2 << 4));

        int pos = p1 * w + p2;
        for (int j = 0; j < s; ++j) {
            if (value[pos].src[j] == 0)  {
                // 注意这里需要先看看邻接表中是不是已经有已有的边了
                int tmp_weight = buffer->queryEdge(k1, k2);
                if (tmp_weight) {
                    buffer->eraseEdge(k1, k2);
                    weight += tmp_weight;
                }

                setRoomVal(pos, j, idx, g1, g2, weight);
                goto FINISHED;
            }
        }
    }


    buffer->addEdge(k1, k2, weight);

FINISHED:
    deg[k1]++;
    deg[k2]++;

    delete[] tmp1;
    delete[] tmp2;

    //buffer->print();
    return;
}

void GSS::nodeSuccessorQuery(string s1, vector<string> &IDs) // query the successors of a node, s1 is the ID of the queried node. results are put in the vector, hash table needed.
{
    vector<int> retNo;
    retNo.clear();
    unsigned int h1, g1, k1;

    gen_gh(s1, g1, h1, k1);

    int *tmp1 = new int[r];
    tmp1[0] = g1;
    for (int i = 1; i < r; i++) {
        tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
    }
    for (int i = 0; i < r; i++) {
        int p1 = (h1 + tmp1[i]) % w;
        for (int k = 0; k < w; k++) {
            int pos = p1 * w + k;
            for (int j = 0; j < s; ++j) {
                if ((((value[pos].idx >> ((j << 3))) & ((1 << 4) - 1)) == i)
                    && (value[pos].src[j] == g1)) {
                    int tmp_g = value[pos].dst[j];
                    int tmp_s = ((value[pos].idx >> ((j << 3) + 4)) & ((1 << 4) - 1));

                    int shifter = tmp_g;
                    for (int v = 0; v < tmp_s; v++)
                        shifter = (shifter * timer + prime) % bigger_p;
                    int tmp_h = k;
                    while (tmp_h < shifter)
                        tmp_h += w;
                    tmp_h -= shifter;
                    int val = tmp_h + tmp_g * w;
                    retNo.push_back(val);
                }
            }
        }
    }


    buffer->querySuccessor(k1, retNo);
    for (int i = 0; i < retNo.size(); ++i) {
        IDs.push_back(mapper->getVal(retNo[i]));
    }

    retNo.clear();
    delete[] tmp1;
    return;
}

void GSS::nodePrecursorQuery(string s1, vector<string> &IDs) // same as successor query
{
    vector<int> retNo;
    retNo.clear();

    unsigned int h1, g1, k1;

    gen_gh(s1, g1, h1, k1);

    int *tmp1 = new int[r];
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
            int pos = p1 + k * w;
            for (int j = 0; j < s; ++j)
            {
                if ((((value[pos].idx >> ((j << 3) + 4)) & ((1 << 4) - 1)) == i) && (value[pos].dst[j] == g1))
                {
                    int tmp_g = value[pos].src[j];
                    int tmp_s = ((value[pos].idx >> (j << 3)) & ((1 << 4) - 1));

                    int shifter = tmp_g;
                    for (int v = 0; v < tmp_s; v++)
                        shifter = (shifter * timer + prime) % bigger_p;
                    int tmp_h = k;
                    while (tmp_h < shifter)
                        tmp_h += w;
                    tmp_h -= shifter;

                    //int val = (tmp_h << f) + tmp_g;
                    int val = tmp_h + tmp_g * w;
                    retNo.push_back(val);
                }
            }
        }
    }

    buffer->queryPrecursor(k1, retNo);
    for (int i = 0; i < retNo.size(); ++i) {
        IDs.push_back(mapper->getVal(retNo[i]));
    }

    retNo.clear();
    delete[] tmp1;
    return;
}

int GSS::edgeQuery(string s1, string s2) // s1 is the ID of the source node, s2 is the ID of the destination node, return the weight of the edge
{
    unsigned int g1, h1, g2, h2, k1, k2;



    gen_gh(s1, g1, h1, k1);
    gen_gh(s2, g2, h2, k2);

    //printf("in Edgequery %d %d\n", k1, k2);

    int *tmp1 = new int[r];
    int *tmp2 = new int[r];
    tmp1[0] = g1;
    tmp2[0] = g2;
    for (int i = 1; i < r; i++)
    {
        tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
        tmp2[i] = (tmp2[i - 1] * timer + prime) % bigger_p;
    }
    long key = g1 + g2;

    for (int i = 0; i < p; i++)
    {
        key = (key * timer + prime) % bigger_p;
        int index = key % (r * r);
        int index1 = index / r;
        int index2 = index % r;
        int p1 = (h1 + tmp1[index1]) % w;
        int p2 = (h2 + tmp2[index2]) % w;
        int pos = p1 * w + p2;
        for (int j = 0; j < s; j++)
        {
            if ((((value[pos].idx >> (j << 3)) & ((1 << 8) - 1)) == (index1 | (index2 << 4)))
                && (value[pos].src[j] == g1)
                && (value[pos].dst[j] == g2))
            {
                delete[] tmp1;
                delete[] tmp2;
                return value[pos].weight[j];
            }
        }
    }

    delete[] tmp1;
    delete[] tmp2;
    //buffer->print();
    return buffer->queryEdge(k1, k2);
}

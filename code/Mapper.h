#include <string>
#include <cstdio>
#include <cstring>
#include <vector>
#include <queue>
#include <unordered_map>

#define MAGIC_STRING "a$d%1*$sd"
#define BAD_STRING "12#231$12"

class Mapper_Naive {
    // 朴素的想法：按加入顺序编号，删除的元素放在队列中即可

public:
    Mapper_Naive();
    bool insert(std::string str);
    bool erase(std::string str);
    bool erase(int id);
    bool changeOne(std::string str);
    int getID(std::string str);
    std::string getVal(int id) { return mapper[id]; }

private:
    std::vector<std::string> mapper;
    std::queue<int> eraselist;
};

Mapper_Naive::Mapper_Naive() {
    mapper.clear();
    while (eraselist.empty() == false) {
        eraselist.pop();
    }
}

bool Mapper_Naive::insert(std::string str) {
    if (getID(str) != -1) {
        return false;
    }
    if (eraselist.empty() == false) {
        int pos = eraselist.front();
        eraselist.pop();
        mapper[pos] = str;
//        printf("insert from queue\n");
        return true;
    }

//    printf("insert from vector\n");
    mapper.push_back(str);
    return true;
}

bool Mapper_Naive::erase(std::string str) {
    for (int i = 0; i < (int) mapper.size(); ++i) {
        if (mapper[i] == str) {
            eraselist.push(i);
            mapper[i] = MAGIC_STRING;
            return true;
        }
    }
    return false;
}

bool Mapper_Naive::erase(int id) {
    //printf("erase id = %d\n", id);
    if (mapper[id] == MAGIC_STRING)
        return false;
    mapper[id] = MAGIC_STRING;
    eraselist.push(id);
    return true;
}

int Mapper_Naive::getID(std::string str) {
    for (int i = 0; i < (int) mapper.size(); ++i) {
        if (mapper[i] == str) {
            return i;
        }
    }
    return -1;
}

bool Mapper_Naive::changeOne(std::string str) {

    int origin_ID = getID(str);
    assert(origin_ID != -1);

    mapper[origin_ID] = BAD_STRING;
    return insert(str);
}

class Mapper {
private:
    std::unordered_map<std::string, int> mapper;
    std::queue<int> validId;
    std::vector<std::string> idToStr;
    int startId, curId;

public:
    Mapper(int _startId) {
        while(validId.empty() == false)
            validId.pop();
        mapper.clear();
        curId = startId = _startId;
        idToStr.resize(startId);
    }

    int insert(const std::string& s) {
        // if (mapper.find(s) != mapper.end()) {
        //     cerr << "str: " << s << endl;
        // }
        // assert(mapper.find(s) == mapper.end());

        if (mapper.find(s) != mapper.end())
            return -1;

        int id = -1;
        if (validId.empty() == false) {
            id = validId.front();
            validId.pop();
            idToStr[id] = s;
        } else {
            id = curId++;
            idToStr.push_back(s);
            assert(curId == idToStr.size());
        }
        mapper[s] = id;
        return id;
    }

    int getID(const std::string& s) {
        return mapper.find(s) == mapper.end() ? -1 : mapper[s];
    }

    void erase(const std::string& s) {
        assert(mapper.find(s) != mapper.end());
        int id = mapper[s];
        mapper.erase(s);
        idToStr[id] = MAGIC_STRING;
        validId.push(id);
        //cout << "erase : " << s << ",id:" << id << endl;
    }

    std::string getVal(int a) {
        std::string ret = idToStr[a];

        // if (ret == MAGIC_STRING) {
        //     cout << "a:" << a << endl;
        // }

        assert(ret != MAGIC_STRING);
        return ret;
    }
};

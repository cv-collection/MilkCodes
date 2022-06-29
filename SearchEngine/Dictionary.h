#pragma once
#include <vector>
#include <map>
#include <set>
#include <stdlib.h>
#include "priority_queue.h"
using std::vector;
using std::string;
using std::map;
using std::set;

class Dictionary
{
    friend void parseIndex(Dictionary *pdic,const string &path);
    friend void parseWordfeq(Dictionary *pdic,const string &path);
public:
    static Dictionary *getInstance()
    {
        if(Instance == nullptr){//多线程不安全隐患
            Instance = new Dictionary();
            atexit(destroy);//程序退出时自动销毁
        }
        return Instance;
    }
    static void destroy()
    {
        if(Instance != nullptr){
            delete Instance;
            Instance = nullptr;
        }
    }
public:
    vector<CandidateResult> doQuery(const string &queryWord);
    //测试用---晚点放private里
    int distance(const string &, const string &);

private:
    static Dictionary *Instance;
    vector<std::pair<string,int>> _dict;
    map<string,set<int>> _index;

private:
    Dictionary() {
        init("./dictPath");
    }
    ~Dictionary() {}
    void init(const string &dictpath);//初始化词典
    vector<int> queryIndex(const string &queryWord);
};

//Dictionary饿汉形式的单例模式

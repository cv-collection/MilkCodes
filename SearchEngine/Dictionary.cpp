#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "string.h"
#include "Dictionary.h"

Dictionary *Dictionary::Instance = getInstance();//解决多线程不安全

vector<CandidateResult> Dictionary::doQuery(const string &queryWord){//查询
    vector<int> line_no = queryIndex(queryWord);
    vector<CandidateResult> Result;
    if(line_no.size() ==0){
        //无结果
        return Result;
    }
    for(size_t idx = 0;idx != line_no.size();++idx){
        CandidateResult node("",0,0);
        /* CandidateResult node("hello",6,8); */
        node._word = _dict[line_no[idx]].first;
        node._freq = _dict[line_no[idx]].second;
        node._dist = distance(queryWord,node._word);
        Result.push_back(node);
        /* std::cout<<"word = "<<node._word<<" freq= "<<node._freq<<" dist= "<<node._dist<<std::endl; */
    }
    return Result;
}

vector<int> Dictionary::queryIndex(const string &queryWord){
    vector<set<int>> ass;//存放set<int>集合
    for(size_t idx = 0;idx!=queryWord.size();++idx){
        //分字方式 中英混合
        string ch = queryWord.substr(idx,1);
        //

        auto it = _index.find(ch);
        if(it == _index.end()){//不存在该字符
            continue;
        }
        ass.push_back(it->second);
    }
    
    if(ass.size()==0){
        vector<int> tmp;
        return tmp;
    }
    vector<int> tmp = {ass[0].begin(),ass[0].end()}; 
    if(ass.size()==1){
        //vector<int> tmp = {ass[0].begin(),ass[0].end()};
        return tmp;
    }

    //vector<int> tmp = {ass[0].begin(),ass[0].end()};
    for(size_t idx = 1;idx != ass.size();++idx){
        auto itend = set_intersection(tmp.begin(),tmp.end(),ass[idx].begin(),ass[idx].end(),tmp.begin());
        vector<int>::iterator it = itend;
        while(itend != tmp.end()){
            tmp.erase(it);
        }
    }
    return tmp;
}

int Dictionary::distance(const string &word1, const string &word2){
    int n = word1.length();
    int m = word2.length();

    // 有一个字符串为空串
    if (n * m == 0) return n + m;

    // DP 数组
    vector<vector<int>> D(n + 1, vector<int>(m + 1));

    // 边界状态初始化
    for (int i = 0; i < n + 1; i++) {
        D[i][0] = i;
    }
    for (int j = 0; j < m + 1; j++) {
        D[0][j] = j;
    }

    // 计算所有 DP 值
    for (int i = 1; i < n + 1; i++) {
        for (int j = 1; j < m + 1; j++) {
            int left = D[i - 1][j] + 1;
            int down = D[i][j - 1] + 1;
            int left_down = D[i - 1][j - 1];
            if (word1[i - 1] != word2[j - 1]) left_down += 1;
            D[i][j] = std::min(left, std::min(down, left_down));

        }
    }
    return D[n][m];   
}

void Dictionary::init(const string &dictpath){
    DIR* dr = opendir(dictpath.c_str());
    if(dr == NULL){
        perror("opendir");
    }
    struct dirent *pdirent ;
    while((pdirent = readdir(dr))!=NULL){
        //排除.和..
        if(strcmp(pdirent->d_name, ".")==0||strcmp(pdirent->d_name, "..")==0){
            continue;
        }
        //可能孩子中也有需要查询的txt文件
        if(pdirent->d_type==DT_DIR){
            //组合新路径
            string newPath = dictpath+ "/" + string(pdirent->d_name);
            init(newPath);
        }
        string str(pdirent->d_name);
        string type= str.substr(str.length()-4,str.length());
        if(type==".txt") {
            //找到了需要的txt文件
            /* std::cout<<str<<std::endl; */
            string Path = dictpath + "/" + str;
            if(str.substr(str.length()-9,str.length()-15) == "Index")//索引文件
            {
                parseIndex(this,Path);
            }
            else{
                parseWordfeq(this,Path);
            }
        }

    }
    closedir(dr);
}

void parseIndex(Dictionary *pdic,const string &path){
    std::cout<<"parseing  "<<path<<std::endl;
    std::ifstream ifs(path);
    string linebuf;
    while(getline(ifs,linebuf))
    {
        string character = linebuf.substr(0,1);
        linebuf = linebuf.substr(2,linebuf.length());
        //linebuf的组成 字母-行号s
        std::istringstream iss(linebuf);
        set<int> line_no;
        int line;
        while(iss>>line){
            line_no.insert(line);
        }
        pdic->_index.insert({character,line_no});
        /* std::cout<<"insert "<<character<<" ok! and "<< character <<"'set size = "<<pdic->_index[character].size()<<std::endl; */
        /* std::cout<<"insert "<<*line_no.begin()<<std::endl; */
    }
}

void parseWordfeq(Dictionary *pdic,const string &path){
    std::cout<<"parseing  "<<path<<std::endl;
    std::ifstream ifs(path);
    string linebuf;
    while(getline(ifs,linebuf)){
        std::istringstream iss(linebuf);
        string word;
        iss>>word;
        int feq;
        iss>>feq;
        pdic->_dict.push_back({word,feq});
    }
    /* std::cout<<"vector size = "<<pdic->_dict.size()<<std::endl; */
}


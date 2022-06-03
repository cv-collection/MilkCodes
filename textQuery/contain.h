#pragma once
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
using std::vector;
using std::set;
using std::map;
using std::string;

typedef set<int> QueryResult;//假设返回结果就是行号--因为输出就是依据行号
class TextQuery{
    friend class WordQuery;
    friend class AndQuery;
    friend class OrQuery;
    friend class NotQuery;
    friend class Query;
public:
    TextQuery(const string &filename)
        :_filename(filename)
    {};
    QueryResult query(const string &word);
    void ReadFile(const string &filename);
private:
    string _filename;
    vector<string> lines;
    map<string,set<int>> wordAndline_no;
    map<string,int> currs;
};

void TextQuery::ReadFile(const string &filename){
    //把文件写入相应容器中
    int linenum=0;
    std::ifstream ifs(filename);
    string line;
    while(getline(ifs,line)){
        //1.vector<string> lines;
        lines.push_back(line);
        linenum++;
        std::istringstream iss(line);
        string nword;
        while(iss>>nword){
            set<int> line_no;
            //2.map<string,set<int>> wordAndline_no
            auto ret = wordAndline_no.insert({nword,line_no});
            ret.first->second.insert(linenum);
            //3.map<string,int> currs;
            currs.insert({nword,1});
            currs[nword]++;
        }
    }
}

QueryResult TextQuery::query(const string &word){
    QueryResult qres;
    //找对应的行号--wordAndline_no
    for(auto &ll:wordAndline_no[word]){
        qres.insert(ll);
    }
    return qres;
}

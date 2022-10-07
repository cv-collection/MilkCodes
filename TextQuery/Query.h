#pragma once
#ifndef QUERY_H_
#define QUERY_H_
#include <iostream>
#include<iostream>
#include <memory>
#include "contain.h"
using std::shared_ptr;
using std::cout;
using std::endl;


class Query_base
{
    friend class Query;
protected:
    virtual QueryResult eval(TextQuery t) const = 0;
    virtual string rep() const = 0;
};

class Query
{
    friend Query operator|(const Query &lhs,const Query &rhs);
    friend Query operator&(const Query &lhs,const Query &rhs);
    friend Query operator~(const Query &rhs);
public:
    void printfrom(const string &filename);
    Query(const string &);
    ~Query() {}
    //接口
    QueryResult eval(TextQuery t) const{
        return _p->eval(t);
    }
    string rep() const{
        return _p->rep();
    }
private:
    Query(shared_ptr<Query_base> p)
        :_p(p){}
    shared_ptr<Query_base> _p;
};

//按格式输出
void Query::printfrom(const string &filename){
    TextQuery QueryFile(filename); 
    QueryFile.ReadFile(filename);
    auto res = eval(QueryFile);
    cout<<"("<<rep()<<")"<<" occurs "<<res.size()<<" times."<<endl;
    for(auto &ll:res){
        cout<<"(line "<<ll<<"):"
            <<QueryFile.lines[ll-1]<<endl;
    }
}

class WordQuery
:public Query_base
{
public:
    WordQuery(const string &str)
        :_wd(str)
    {}
    virtual QueryResult eval(TextQuery t) const;
    virtual string rep() const;
private:
    string _wd;
};

Query::Query(const string &word)
    :_p(new WordQuery(word))
{
}

QueryResult WordQuery::eval(TextQuery t) const{
    t.ReadFile(t._filename);
    return t.query(_wd);
}

string  WordQuery::rep() const{
    return _wd;
}

class NotQuery
:public Query_base
{
public:
    NotQuery(const Query &q)
        :_wd(q.rep())
    {}
    virtual QueryResult eval(TextQuery t) const;
    virtual string rep() const;
private:
    string _wd;
};

QueryResult NotQuery::eval(TextQuery t) const{
    t.ReadFile(t._filename);
    QueryResult res1 = t.query(_wd);
    QueryResult res;
    for(size_t j = 1;j<t.lines.size()+1;j++){
        bool flag = true;
        for(auto &x:res1){
            if((int)j==x){
                flag = false;
                break;
            }
            else{
                continue;
            }
        }
        if(flag){
            res.insert(j);
        }
    }
    return res;
}

string NotQuery::rep() const{
    return string("~"+_wd);
}

inline Query operator~(const Query &rhs)
{
    return shared_ptr<Query_base>(new NotQuery(rhs));
}



class BinaryQuery
:public Query_base 
{
protected:
    BinaryQuery(const Query &lhs,const Query &rhs)
        :_lhs(lhs)
         ,_rhs(rhs){
         }
    Query _lhs,_rhs;
};

class AndQuery
:public BinaryQuery
{   
public:
    AndQuery(const Query &lhs,const Query &rhs)
        :BinaryQuery(lhs,rhs)    
    {}
    virtual QueryResult eval(TextQuery t) const;
    virtual string rep() const;
};

inline Query operator&(const Query &lhs,const Query &rhs)
{
    return shared_ptr<Query_base>(new AndQuery(lhs,rhs));
}

QueryResult AndQuery::eval(TextQuery t) const{
    t.ReadFile(t._filename);
    QueryResult res;
    auto res1 = _lhs.eval(t);
    auto res2 = _rhs.eval(t);
    for(auto &x : res1){
        for(auto &y : res2){
            if(x==y){
                //保留
                res.insert(x);
            }
        }
    }
    return res;
}

string AndQuery::rep() const{
    return string(_lhs.rep()+" & "+_rhs.rep());
}

class OrQuery
:public BinaryQuery
{
public:
    OrQuery(const Query &lhs,const Query &rhs)
        :BinaryQuery(lhs,rhs)    
    {}
    virtual QueryResult eval(TextQuery t) const;
    virtual string rep() const;
};

QueryResult OrQuery::eval(TextQuery t) const{
    t.ReadFile(t._filename);
    QueryResult res;
    auto res1 = _lhs.eval(t);
    auto res2 = _rhs.eval(t);
    //下面的写法不能查询NotQuery
    /* auto res1 = t.query(_lhs.rep()); */
    /* auto res2 = t.query(_rhs.rep()); */
    for(auto &x:res1){
        res.insert(x);
    }
    for(auto &x:res2){
        res.insert(x);
    }
    return res;
}

string OrQuery::rep() const{
    return string(_lhs.rep()+" | "+_rhs.rep());
}

inline Query operator|(const Query &lhs,const Query &rhs)
{
    return shared_ptr<Query_base>(new OrQuery(lhs,rhs));
}


#endif


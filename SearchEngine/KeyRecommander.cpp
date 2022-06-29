#include <iostream>
#include "KeyRecommander.h"
#include "Dictionary.h"
#include "priority_queue.h"
KeyRecommander::KeyRecommander(string &query,const TcpConnectionPtr &pconn)
:_queryWord(query)
,_conn(pconn)
{
    execute();
}

void KeyRecommander::execute(){
    //打开字典
    Dictionary *pdic = Dictionary::getInstance();
    vector<CandidateResult> res= pdic->doQuery(_queryWord);
    for(auto it = res.begin();it!=res.end();it++){
        _resultQue.emplace(*it);
    }
    //测试优先级队列
    int bestmatch = 5;
    while(_resultQue.size()&&bestmatch-->0){
        std::cout<< _resultQue.top()<<std::endl;
        _resultQue.pop();
    }
}

std::ostream &operator<<(std::ostream &os, const CandidateResult &rhs)
{
    os << "(" << rhs._word
       << ", " << rhs._freq
       << ", " << rhs._dist
       << ")";

    return os;
}

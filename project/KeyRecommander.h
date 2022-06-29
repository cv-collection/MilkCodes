#include <string>
#include <vector>
#include <queue> 
#include <set>
#include <memory>
#include "TcpConnection.h"
#include "priority_queue.h"

using std::string;
using std::vector;
using std::priority_queue;
using std::set;
using std::less;
using std::shared_ptr;

namespace std{
template<>
struct less<CandidateResult>
{
    bool operator()(const CandidateResult &lhs, const CandidateResult &rhs) const
    {
        if (lhs._dist > rhs._dist)
        {
            return true;
        }
        else if (lhs._dist == rhs._dist)
        {
            if (lhs._freq < rhs._freq)
            {
                return true;
            }
            else if (lhs._freq == rhs._freq)
            {
                int cnt1 = lhs._word.size();
                int cnt2 = rhs._word.size();
                int cnt = 0;
                if (cnt1 < cnt2)
                {
                    cnt = cnt2;
                }
                else
                {
                    cnt = cnt1;
                }
                for (int i = 0; i < cnt; i++)
                {
                    if (lhs._word[i] < rhs._word[i])
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
};
}//end of namespace

class KeyRecommander{
public:
    using TcpConnectionPtr = shared_ptr<TcpConnection>; 
private:
    string _queryWord;//等查询的单词
    TcpConnectionPtr _conn;//与客户端建立连接
    priority_queue<CandidateResult,vector<CandidateResult>,std::less<CandidateResult>> _resultQue;
public:
    KeyRecommander(string &query,const TcpConnectionPtr &pconn);
    void execute();
};

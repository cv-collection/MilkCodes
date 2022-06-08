#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>

using std::string;
using std::vector;
using std::endl;
using std::cout;
using std::unordered_set;

class Solution{
public:
    int ladderLength(string beginWord,string endWord,vector<string> &wordList);
};

int Solution::ladderLength(string beginWord,string endWord,vector<string> &wordList){
    int step = 1;
    unordered_set<string> wordDisk(wordList.begin(),wordList.end());
    //endWord不在字典中，直接return 0
    if(!wordDisk.count(endWord)){ return 0; }
    unordered_set<string> starton = {beginWord};
    unordered_set<string> backon = {endWord};
    while(!starton.empty()){//不为空
        unordered_set<string> stack;
        //把当前的starton从字典中删除，不能找已固定的元素
        for(auto &x: starton){
            wordDisk.erase(x);
        }
        for(auto &x: starton){
            for(size_t i=0;i<beginWord.size();i++){
                for(char ch='a';ch<='z';ch++){
                    string current = x;
                    current[i] = ch;//更改当前的起始词的一个字母
                    //若字典中遍历都不能找到，说明不存在只差一个字母的路径
                    if(!wordDisk.count(current)) continue; 
                    if(backon.count(current)) return ++step;
                    //把存在的可能中间路径入栈--存放剩下的单词
                    stack.insert(current);
                    cout<<current<<" insert to stack"<<endl;
                }
            }
        }
        //尽可能使得终点词对应的选择多，开始的选择少
        if(stack.size()>backon.size()){
            cout<<"stack!"<<endl;
        }
        starton = stack.size()<backon.size()?stack:backon;
        backon = stack.size()<backon.size()?backon:stack;
        step++;
        cout<<"stack.size = "<<stack.size()<<endl;
    }
    return 0;
}

void test(){
    string beginWord= "hit";
    string endWord = "cog";
    vector<string> wordList = {"hot","dot","dog","lot","log","cog"};
    cout << Solution().ladderLength(beginWord, endWord, wordList) << endl;
}

int main()
{
    test();
    return 0;
}


#pragma once
#include <vector>
#include <map>
#include <string>
#include "./simhash/include/simhash/Simhasher.hpp"
#include "Configuration.h"
using std::vector;
using std::map;
using std::string;
class WebPage
{
    friend bool operator==(const WebPage &lhs,const WebPage &rhs);
    friend bool operator<(const WebPage &lhs,const WebPage &rhs);
public:
    /* WebPage(string &doc,Configuration &config,WordSegmentation &); */
    WebPage(string &doc)
    :_Value(0)
    ,_doc(doc)
    {
        _topWords.reserve(10000);
        preprocess(doc);
        getHashValue();
    };
    /* void preprocess(string &doc,Configuration &config,WordSegmentation &); */
    void preprocess(string &doc);
    ~WebPage(){}
    int getDocId() const;
    string getTitle() const;
    string getUrc() const;
    string getContent() const;
    const uint64_t &getHashValue() ;
    string summary(const vector<string> &queryWords);
    size_t getSize() const;
    map<string,int> &getWordsMap();

public:
    uint64_t _Value;
private:
    string _doc;
    int _docId;
    string _docTitle;
    string _docUrl;
    string _docContent;
    string _docSummary;
    vector<string> _topWords;
    map<string,int> _wordsMap;
};


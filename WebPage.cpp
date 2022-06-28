#include "WebPage.h"
#include "tinyxml2.h"
bool operator==(const WebPage &lhs,const WebPage &rhs){
    using simhash::Simhasher;
    /* std::cout<<lhs._Value<<" vs "<<rhs._Value<<std::endl; */
    bool isequal = Simhasher::isEqual(lhs._Value, rhs._Value, 3);
    /* std::cout<< "海明距离阈值默认设置为5，则isEqual结果为：" << isequal << std::endl; */ 
    return isequal;
}
bool operator<(const WebPage &lhs,const WebPage &rhs){
    return lhs._docId<rhs._docId;
}

/* void preprocess(string &doc,Configuration &config,WordSegmentation &); */
void WebPage::preprocess(string &doc){
    using tinyxml2::XMLElement;
    using tinyxml2::XMLDocument;
    //解析doc文件放入各个容器中
    /* std::cout<<doc<<std::endl; */
    XMLDocument xdoc;
    xdoc.Parse(doc.c_str());
    /* std::cout<<"error?"<<xdoc.ErrorID()<<std::endl; */
    XMLElement *NodeDoc = xdoc.FirstChildElement("doc");
    if(!NodeDoc){
        return;
    }
    XMLElement *NodeDocid = NodeDoc->FirstChildElement("docid");
    if(NodeDocid){
        _docId = atoi(NodeDocid->GetText());
        /* std::cout<<_docId<<std::endl; */
    }
    XMLElement *NodeDoctitle= NodeDoc->FirstChildElement("title");
    if(NodeDoctitle){
        _docTitle = NodeDoctitle->GetText();
        /* std::cout<<_docTitle<<std::endl; */
    }
    XMLElement *NodeDocurl = NodeDoc->FirstChildElement("link");
    if(NodeDocurl){
        _docUrl= (NodeDocurl->GetText());
        /* std::cout<<_docUrl<<std::endl; */
    }
    XMLElement *NodeDoccontent = NodeDoc->FirstChildElement("description");
    if(NodeDoccontent){
        if(!NodeDoccontent->GetText()){
            _docContent = " ";
        }
        else{
            _docContent = NodeDoccontent->GetText();
        }
    }

}
int WebPage::getDocId() const{
    return _docId;
}
string WebPage::getTitle() const{
    return _docTitle;
}
string WebPage::getUrc() const{
    return _docUrl;
}
string WebPage::getContent() const{
    return _docContent;
}
const uint64_t &WebPage::getHashValue(){
    using simhash::Simhasher;
    Simhasher simhasher("./dict/jieba.dict.utf8", "./dict/hmm_model.utf8", "./dict/idf.utf8", "./dict/stop_words.utf8");
    size_t topN = 5;
    vector<std::pair<string ,double> > res;
    simhasher.extract(_doc, res, topN);
    simhasher.make(_doc, topN, _Value);
    //关键词入容器
    for(auto it = res.begin();it!=res.end();it++){
        _topWords.push_back(it->first);
    }
    return _Value;
} 

size_t WebPage::getSize() const{
    return _doc.size();
}
/* string WebPage::summary(const vector<string> &queryWords); */
/* map<string,int> &WebPage::getWordsMap(){ */

/* } */


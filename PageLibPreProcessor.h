#pragma once
/* #include <unordered_set> */
#include "WebPage.h"
#include "./simhash/include/simhash/Simhasher.hpp"
struct pageHash{
    size_t operator()(WebPage &pg) const {
        return (pg.getHashValue())^(pg.getHashValue());
    }
};
//模板的特化---unordered_set
/* namespace std{ */
/*   template <> */
/*   struct equal_to<WebPage> */
/*   { */
/*       bool operator()(const WebPage &lhs, const WebPage &rhs) const */
/*       { */
/*           return (rhs._Value==lhs._Value); */
/*       } */
/*   }; */

/* }; */
class PageLibPreProcessor
{
public:
    PageLibPreProcessor();
    ~PageLibPreProcessor() {}
    void cutRedundantPage();//去重
    void storeOnDisk();//写回文件

private:
    /* std::unordered_set<WebPage,pageHash> pageList; */
    vector<WebPage> pageList;
    std::map<int,std::pair<int,int>> offsetLib;
};


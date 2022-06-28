#include "PageLibPreProcessor.h"
#include "./tinyxml2.h"
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <sys/stat.h>
#include <fcntl.h>

PageLibPreProcessor::PageLibPreProcessor(){
    cutRedundantPage();
    storeOnDisk();
}

void PageLibPreProcessor::cutRedundantPage(){
    //1.文件去重
    using namespace simhash;
    std::ifstream ifs("./offset.dat");
    //读取偏移库文件，每次读取一行
    string linebuf;
    char buf[65535] = {};
    int fd = open("./pagelib.dat",O_RDONLY);
    if(fd == -1){
        perror("open dat");
    }
    while(getline(ifs,linebuf)){
        std::istringstream ifss(linebuf);
        int index,initpos,len;
        ifss>>index;
        ifss>>initpos;
        ifss>>len;
        /* cout<<index<<" "<<initpos<<" "<<len<<endl; */
        bzero(&buf,sizeof(buf));
        read(fd,&buf,len);
        string wpBuf(buf);
        WebPage curwp(wpBuf);
        /* pageList.insert(WebPage(wpBuf)); */
        bool flag = true;
        for(auto it = pageList.begin();it!=pageList.end();it++){
            if(*it == curwp){
                flag = false;
                cout<<"erase docid "<<curwp.getDocId()<<endl;
                break;
            }
        }
        if(flag){
            pageList.push_back(curwp);
        }
    }
    close(fd);
    ifs.close();
    //2.重写偏移库
    int curLen = 0;
    for(auto it = pageList.begin();it!=pageList.end();it++){
        int addLen = it->getSize();
        /* cout<<"addlen = "<<it->getSize()<<" "; */
        offsetLib.insert({it->getDocId(),{std::pair<int,int>(curLen,addLen)}});
        /* cout<<"input curlen = "<<curLen<<endl; */
        curLen += addLen;
    }
}


void PageLibPreProcessor::storeOnDisk(){
    std::ofstream ofs("new.dat");
    if(!ofs){
        perror("open new.dat");
    }
    for(auto it = pageList.begin();it!=pageList.end();++it){
        ofs<<"<doc><docid>"<<(*it).getDocId()<<"</docid><title>"<<
            (*it).getTitle()<<"</title><link>"<<(*it).getUrc()<<"</link><description>"
            <<(*it).getContent()<<"</description></doc>"<<std::endl;
    }

    std::ofstream ofss("newoff.dat");
    for(auto it = offsetLib.begin();it!=offsetLib.end();it++ ){
        /* std::cout<<it->second.first<<"--------"<<std::endl; */
        ofss<<it->first<<" "<<it->second.first<<" "<<it->second.second<<std::endl;
    }
    ofs.close();
    ofss.close();
}

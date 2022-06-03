#include "Query.h"

void test1(){
    Query query = Query("link");
    query.printfrom("china");
}

void test2(){
    Query query = Query("link")&Query("will");
    query.printfrom("china");
}

void test3(){
    Query query = Query("link")|Query("will");
    query.printfrom("china");
}

void test4(){
    Query query = ~Query("link")&Query("will");
    query.printfrom("china");
}

void test5(){
    Query query = ~Query("link")&Query("will")|Query("of");
    query.printfrom("china");
}

int main()
{
    cout<<"-----1-----"<<endl;
    test1();
    cout<<"-----2-----"<<endl;
    test2();
    cout<<"-----3-----"<<endl;
    test3();
    cout<<"-----4-----"<<endl;
    test4();
    cout<<"-----5-----"<<endl;
    test5();
    return 0;
}


# 文本查询程序

## 接口类

```c++
class Query{
	//重载运算符--实现多态
    friend Query operator~(const Query &);
    friend Query operator&(const Query &,const Query &);
    friend Query operator|(const Query &,const Query &);
public:
    //WordQuery类中把Query设置为友元类--从而可以调用WordQuery的构造函数
    Query(const string&);
	
    //接口函数--多态调用Query_base指针
    QueryResult eval(const TextQuery &t) const {
        return q->eval(t);
    }
    String rep() const {
        return q->rep();
    }
    
    void print(string filename){
        TextQuery(filename);
        TextQuery.readFile(filename);
    	//利用eval()返回的行号查询lines--vector<string> lines
    }
    
private：
    //将使用指针构造--设置为私有只能在堆上创建  Query *p = new Query(shared_ptr<Query_base>);
    Query(shared_ptr<Query_base> query)
    :q(query)
    {}
    shared_ptr<Query_base> q;//实现多态的条件--基类指针
};


```

## Query_base派生类

```c++
class WordQuery
:public Query_base
{
	friend class Query;
	WordQuery(string str)
	:_QueryWord(str)
	{}
	
    virtual QueryResult eval(const TextQuery& t) const{
    	//返回根据单词查找到的行号
    	t.readFile(t._filename);
    	return t.query(_QueryWord);
    } 
    virtual string &rep() const{
    	//返回当前的查询命令
    }
    
private:
	string _QueryWord;
}

//解决第一种多态--Query("hello")--派生类对象是WordQuery单个词查询
inline Query::Query(const string &str)
:q(new WordQuery(str))//Query_base *q= new WordQuery(str);--WordQuery是Query_base的派生类
{}

typedef QueryResult set<int> //记录所有查询后符合要求的行
    
    
//运算符重载实现第二种多态--NotQuery
inline Query operator~(const Query &rhs){
    return Query(shared_ptr<Query_base>(new NotQuery(rhs)));
}

class NotQuery{
    //类似WordQuery
    //eval实现不同--全部行号加入返回容器（除了判断出有查询单词的那行）
    virtual QueryResult eval(const TextQuery& t) const{
    	//返回根据单词查找到的行号
    	t.readFile(t._filename);
 		for(size_t i=0;i<t.lines.size()；i++){
            bool flag = true;
            for(auto &x:t.query(_wd)){
                if(x==i){
                    flag = false;
                    break;
                }
            }
            if(flag){
                res.insert(i);
            }
        }
        return res;
    }
    
};
```

## BinaryQuery的派生类

```c++
//利用运算符重载解决其他多态
//第三种--Query("hello")&Query("world")
inline Query operator&(const Query &lhs,const Query &rhs){
    return Query(shared_ptr<Query_base>(new AndQuery(lhs,rhs)));
}

class AndQuery
:public BinaryQuery
{
  	AndQuery(const Query &lhs,const Query &rhs)
  	:_lhs(lhs)
  	,_rhs(rhs)
  	{}
    private:
    	//继承自protected:		
    				//Query BinaryQuery::_lhs,BinaryQuery::_rhs; 

	//实现虚函数
	QueryResult eval(const TextQuery &t) const {
    	t.readFile(t._filename);
        lhs_ll=_lhs.eval();//左边查询的符合要求的行数
        rhs_ll=_rhs.eval();//右边...
        //求交集
        for(auto &x:lhs_ll){
        	for(auto &y:rhs_ll){
        		if(x==y) res.insert(x)
        	}
        }
        return res;
    }
    String rep() const {
        ...
    }
};

//第四种--~Query("hello")|Query("world")
inline Query operator|(const Query &lhs,const Query &rhs){
    return Query(shared_ptr<Query_base>(new OrQuery(lhs,rhs)));
}

class OrQuery
:public BinaryQuery
{
	//类似AndQuery
	//eval的实现不同--res.insert(x),res.insert(y)
}
```



## 流程

1.实现多态

Query("hello")->创建一个WordQuery的对象初始化私有的Query(shared_ptr\<Query_base\>) _p

重载运算符--return shared_ptr\<Query_base\>(new OrQuery/AndQuery/NotQuery); 隐式转换了，本质返回一个Query对象

2.Query中的Query_base指针是私有的--需要接口函数




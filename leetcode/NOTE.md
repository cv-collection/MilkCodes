## 127题

思想：首尾两边都进行搜索

通过三目运算符找到各自需要的容器

​	针对搜索时头部需要的是尽可能短的路，这样可以**保证可以选择的路一开始就相对少，减少了不必要的搜索，尾部可以尽可能的多给机会，增加可以匹配上的几率**

<p style = "color:red">判断只有一个字符不相同使用的是下标运算符替换</p>

分析流程

​	`第一次begin的时候，由于backon只有一个endWord元素，所以只要current逐个进栈（剩余没有固定位置且可以与begin建立联系的元素），这样栈的大小一般会大于1（backon容器的初始大小）那么就可以去假设endWord元素是begin头（交换头尾），即backon成为starton，backon变成stack。`

```c++
starton = (stack.size()<backon.size())?stack:backon;
backon = (stack.size()<backon.size())?backon:stack;
```

<img src="../assets/1654704685000.png"


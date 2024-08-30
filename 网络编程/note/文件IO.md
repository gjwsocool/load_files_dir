## 文件IO

### 1. open函数的使用

+ 所需头文件

  ```c
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  ```

+ 两个函数原型

  ```c
  int open(const char *pathname, int flags);//只能打开存在文件
  int open(const char *pathname, int flags, mode_t mode);//文件不存在会创建
  ```

  >open函数的返回值：
  >
  >+ 打开成功-----返回非负整数的**文件描述符**
  >
  >+ 打开失败-----返回-1并设置错误号给系统定义的全局变量**errno**

#### flag参数表格

| 第一组参数 | 不可组合 |
| :--------: | :------- |
|  O_RDONLY  | 只读     |
|  O_WRONLY  | 只写     |
|   O_RDWR   | 可读可写 |

| 第二组参数 | 可以组合                                                 |
| :--------: | -------------------------------------------------------- |
|  O_TRUNC   | 写之前，文件清空，文件指针指向文件头                     |
|  O_APPEND  | 写数据的内容**追加到文件末尾**\|\|不加该选项会覆盖原文件 |
|  O_CREATE  | 需要使用三个参数的open函数，文件不存在会创建             |
|   O_EXCL   | 和O_CREATE合用，文件存在会报错                           |

####  mode的参数----三个参数的open函数

> **三类用户**：文件属主，文件属组用户，其他用户
>
> 三种权限：读4，写2，执行1
>
> >ex:
> >
> >641----属主可读可写，属组用户可写，其他用户可执行

```c
int main(){
    int fd=open("MM.txt",O_RDWR|O_CREAT,0664);
    if(fd==-1){
        printf("open fails!\n");
    }
    else{
        printf("open successful!\n");
    }
    char buf[]="i am very well";
    int ret=write(fd,buf,strlen(buf));
    if(ret==-1){
        printf("write fails!\n");
    }
    else{
        printf("write successful!\n");
    }
    return 0;
}
```

```c
open("MM.txt",O_RDWR|O_CREAT|O_EXCL,0664);//注意参数
```

#### 文件描述符

什么是`文件描述符`？				
	open成功就会返回一个非负整数（0、1、2、3...）的文件描述符
					
	文件描述符指向了打开的文件，后续的read/write/close等函数的文件操作，都是通过文件描述符来实现的。

> 每个程序运行起来后，就是一个进程，系统会给每个进程分配0~1023的文件描述符范围.
>
> 也就是说每个进程打开文件时，**open所返回的文件描述符，是在0~1023范围中的某个数字。**
>
> **文件关闭后，被文件用的描述符会被释放**，等着下一次open时，被重复利用

                  应用程序
    				|
    	           	|
    	FILE*	fopen标准io函数（c库函数）
    				|
    			    |
    	int	open文件io函数（Linux系统函数）

##### open的文件描述符 与 fopen的文件指针

+ open是Linux的系统函数--***文件io函数***，open成功后，返回的**文件描述符**，指向了打开的文件
+ fopen是C库的标准io函数，fopen成功后，返回的是FILE *的**文件指针**，指向了打开的文件。

#### ernno

​         **函数调用**出错时，Linux系统使用**错误编号（整形数）**来标记具体出错的原因，每个函数有很多错误号，每个错误号代表了一种错误，产生这个错误时，会`自动的`将错误号赋值给**errno这个全局变量**。

+ errno是Linux系统定义的全局变量
+ 错误号和errno全局变量都被定义在了errno.h头文件
+ perror是一个C库函数，不是一个系统函数,功能是：自动将“错误号”换成对应的文字信息

```c
//主函数
int main(){
    int fd=open("MM.txt",O_RDWR|O_CREAT|O_EXCL,0664);
    if(fd==-1){
        perror("open fail!");
    }
    return 0;
}
```

> perror的工作原理------调用perror函数时，它会自动去一张对照表，将errno中保存的错误号，换成具体的文字信息并打印出来

![image-20221023200216574](/home/guojiawei/.config/Typora/typora-user-images/image-20221023200216574.png)
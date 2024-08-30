### exec加载器

#### exec的作用

> 父进程fork复制出子进程的内存空间后,子进程内存空间的代码和数据和父进程是相同的,要在子进程空间里面运行全新的代码,就需要用到exec加载器
>
> > 也可以在if(ret==0)**{}**里面直接写新代码，但是代码行过多不可行

+ 有了exec后，我们可以单独的另写一个程序，将其`编译`好后，使用`exec来加载`即可

#### exec函数族

+ 系统函数是execve()
+ execl、execv、execle、execlp、execvp都是基于execve封装得到的库函数

> exec的函数有很多个，它们分别是execve、execl、execv、execle、execlp、execvp，都是加载函数

##### execve()

```c
#include <unistd.h>//unix standard
//向子进程空间加载新程序代码（编译后的机器指令）
int execve(const char *pathname, char *const argv[],char *const envp[]);
/*参数：
1.filename：新程序（可执行文件）所在的路径名
2.argv：传给main函数的参数，比如我可以将命令行参数传过去
3.envp：环境变量表
*/
//返回值：函数调用成功不返回，失败则返回-1，且errno被设置
```

+ 可以是任何编译型语言所写的程序，比如可以是`c、c++、汇编`等，这些语言所写的程序被编译为`机器指令`后,都可以被execve这函数加载执行。
+ 正是由于这一点特性，我们才能够在C语言所实现的OS上，运行任何一种`编译型语言`所编写的程序。
+ java属于`解释性语言`，它所写的程序被编译后只是字节码，并不是能被CPU直接执行的机器指令，所以不能被execve直接加载执行，而是被虚拟机解释执行

> execve需要先加载运行`java虚拟机程序`，然后再由虚拟机程序去`将字节码解释为机器指令`，再由cpu去执行

##### 函数使用

```c
//先写exec函数要加载的子程序代码
//函数功能：打印出---参数argv和环境变量environ
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
extern char** environ;
int main(int argc,char** argv){
   int i=0;
   for(;i<argc;i++){
      printf("%s\t",argv[i]);
   }
   for(;environ[i]!=NULL;i++){
      printf("%s\n",environ[i]);
   }
   return 0;
}
```

```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
 //pid_t fork(void);
 //int execve(const char *pathname, char *const argv[],\
              char *const envp[]);
int main(int argc,char** argv,char** environ){
   pid_t res=0;
   res=fork();
   if(res>0){//父进程
       sleep(1);
   }
   else if(res==0){//子进程
      //execve----编译后的可执行文件，参数argv,环境变量environ
      execve("child",argv,environ);
   }
   return 0;
}
```

> + 把第一文件编译成child,即可执行
>
> + environ两种声明：extern char** 或者 main的第三个参数

> 当然可以自己写：argv和environ
>
> ```c
> #include <stdio.h>
> #include <sys/types.h>
> #include <unistd.h>
> int main(int argc,char** argv,char** environ){
>    int i;
>    for(i=0;i<argc;i++){
>       printf("%s\n",argv[i]);
>    }
>    printf("****************\n");
>    for(i=0;environ[i]!=NULL;i++){
>       printf("%s\n",environ[i]);
>    }
>    return 0;
> }
> ```
>
> ```c
> #include <stdio.h>
> #include <sys/types.h>
> #include <unistd.h>
>  //pid_t fork(void);
>  //int execve(const char *pathname, char *const argv[],\
>               char *const envp[]);
> int main(int argc,char** argv,char** environ){
>    pid_t res=0;
>    res=fork();
>    if(res>0){//父进程
>        sleep(1);
>    }
>    else if(res==0){//子进程
>       //execve----编译后的可执行文件，参数argv,环境变量environ
>       char* my_argv[]={"abc","efg","mnp",NULL};
>       char* my_environ[]={"aa=bbbbb","bb=ccccc",NULL};
>       execve("child",my_argv,my_environ);
>    }
>    return 0;
> }
> ```
>
> ```
> guojiawei@ubantu-gjw:~/Desktop/exec$ ./a.out 
> abc
> efg
> mnp
> ****************
> aa=bbbbb
> bb=ccccc
> ```

注意：

char* str={"",...,NULL}----必须有NULL

```
 命令行参数/环境表      命令行参数/环境表           命令行参数/环境表       
  终端窗口进程——————————>a.out（父进程）————————————>a.out（子进程）——————————————>新程序
	                                     fork                        exec
```

##### exec的作用

1. 新程序代码加载（拷贝）到子进程的内存空间
2. 替换掉原有的与父进程一模一样的代码和数据
3. 让子进程空间运行全新的程序。

##### 命令行执行./a.out,怎么运行程序的

1）窗口进程先fork出子进程空间

2）调用exec函数加载./a.out程序，并把命令行参数和环境变量表传递给新程序的main函数的形参

##### 双击快捷图标，程序是怎么运行起来的

（1）图形界面进程fork出子进程空间
（2）调用exec函数，加载快捷图标所指向程序的代码

> 以图形界面方式运行时，就没有命令行参数了，但是会传递`环境变量表`

### system()

> ​       如果我们需要创建一个`子进程`，让子进程运行`另一个程序`的话，可以自己`fork、execve`来实现，但是这样的操作很麻烦

system这个库函数封装了`fork和execve`函数，调用时会`自动的`创建子进程空间，并把新程序的代码加载到子进程空间中，然后运行起来。

```c
#include <stdlib.h>
//创建子进程，并加载新程序到子进程空间，运行起来
//参数：新程序的路径名
int system(const char *command);
```

```c
#include<stdio.h>
#include<stdlib.h>
int main(){
    system("ls");
    return 0;
}
```

当然也可以直接传入可执行路径

### 回收进程资源

为什么要回收进程的资源？

+ 程序代码在内存中动态运行起来后，才有了进程，进程既然结束了，就需要将代码占用的内存空间让出来（释放）
+ OS为了管理进程，为每个进程在内存中开辟了一个task_stuct结构体变量，进程结束了，那么这个结构体所占用的内存空间需要被释放

<font color=RED>父进程运行结束时，会负责释放子进程资源</font>

### 僵尸进程和孤儿进程

##### 僵尸进程

> 子进程终止了，但是父进程还活着，父进程在没有回收子进程资源之前，子进程就是`僵尸进程`

为什么子进程会变成僵尸进程？

> 子进程已经终止不再运行，但是父进程还在运行，它没有释放子进程占用的资源，所以就变成了占着资源的僵尸进程

##### 孤儿进程

> 子进程活着，但是父进程终止了，子进程就是孤儿进程

为了能够`回收孤儿进程终止后的资源`，孤儿进程会被托管给我们前面介绍的`pid==1的init`进程，每当被托管的子进程终止时，init会`立即主动`回收孤儿进程资源，回收资源的速度很快，**所以孤儿进程没有变成僵尸进程的机会**。

```c
#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
    //pid_t fork(void);
    pid_t res=0;
    res=fork();
    if(res>0){//父进程
        while(1);//父进程不死；
    }
    else if(res==0){//子进程
       
    }
    return 0;
}
```

![image-20221109224009516](/home/guojiawei/.config/Typora/typora-user-images/image-20221109224009516.png)

+ **R** 正在运行
+ **S** 处于休眠状态
+ **Z** 僵尸进程，进程运行完了，等待被回收资源。

```c
#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
    //pid_t fork(void);
    pid_t res=0;
    res=fork();
    if(res>0){//父进程
        
    }
    else if(res==0){//子进程
       while(1);//子进程不死；
    }
    return 0;
}
```

![image-20221109224226058](/home/guojiawei/.config/Typora/typora-user-images/image-20221109224226058.png)

### wait()

+ 这个函数是给父进程调用的

  主要有两个作用：

  + 主动获取子进程的“进程终止状态”
  + 主动回收子进程终止后所占用的资源

> 如果你理解了wait函数，你就能理解进程return/exit/_exit所返回的返回值，到底返回给了谁

#### 进程终止

```
正常终止：
（1）main调用return
（2）任意位置调用exit
（3）任意位置调用_exit
不管哪种方式来正常终止，最终都是通过_exit返回到OS内核的

异常终止：
如果是被某个信号终止的，就是异常终止
1）自杀：自己调用abort函数，自己给自己发一个SIGABRT信号将自己杀死。
2）他杀：由别人发一个信号，将其杀死。
```

#### 退出状态与进程终止状态

1. return、exit、_exit的返回值严格来说应该叫`退出状态`
2. 当退出状态被_exit函数交给OS内核，OS对其进行`加工`之后得到的才是`进程终止状态`

```
OS是怎么加工的:
1----正常终止
      进程终止状态 = 终止原因（正常终止）<< 8 | 退出状态的低8位（子进程返回）
2----异常终止
      进程终止状态 = 是否产生core文件位 | 终止原因（异常终止）<< 8 | 终止该进程的信号编号
core文件：某一些信号异常终止，会把进程在内存中的代码和数据拷贝到该文件中保存下来
```

> 父进程调用wait函数便可以得到这个“进程终止状态”

> 低八位有效：
>
> ```
> #include<stdio.h>
> #include<stdlib.h>
> int main(){
>     return 1000;
> }
> ```
>
> ```
> //执行结果：
> guojiawei@ubantu-gjw:~/Desktop/mmm$ echo $?
> 232
> ```

#### 父进程通过wait得到进程终止状态的作用

>  父进程得到进程终止状态后，就可以判断子进程终止的原因:
>
> 1. 如果是正常终止的，可以提取出返回值
> 2. 如果是异常终止的，可以提取出异常终止进程的信号编号
>
> > 父进程可以根据子进程的终止状态来判断子进程的终止原因，返回值等等，以决定是否`重新启动子进程`

当有OS支持时，进程return、exit、_exit正常终止时，所返回的返回值（退出状态），最终通过“进程终止状态”返回给了`父进程`

#### 如何获取

（1）父进程调用wait等子进程结束，如果子进程没有结束的话，父进程调用wait时会一直休眠的等（或者说阻塞的等）

（2）子进程终止返回内核，内核构建“进程终止状态”-----两个表达式

> + 进程终止状态 = 终止原因（正常终止）<< 8 | 退出状态的低8位
> + 进程终止状态 = 是否产生core文件位 | 终止原因（异常终止）<< 8 | 终止该进程的信号编号

（3）内核向父进程发送`SIGCHLD`信号，通知父进程子进程结束了，就·可以获取子进程的“进程终止状态”了。

> 1. 如果父进程没有调用wait函数的话，会忽略这个信号，表示不关心子进程的“进程终止状态”
>
> 2. 如果父进程正在调用wait函数等带子进程的“进程终止状态”的话，`wait会被SIGCHLD信号`唤醒，并获取进“进程终止状态”

#### 函数原型

```c
#include <sys/types.h>
#include <sys/wait.h>
//功能：获取子进程的终止状态，主动释放子进程占用的资源(自动完成)
//参数：用于存放“进程终止状态”的缓存
//返回值：成功返回子进程的PID，失败返回-1，errno被设置。
pid_t wait(int *wstatus);
```

使用：

```C
#include<stdio.h>
#include<stdlib.h>
int main(){
    printf("hello,world\n");
    return 20;//注意为了验证status被改变
}
```

```C
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc,char** argv){
    //pid_t fork(void);
    pid_t res=0;
    res=fork();
    if(res>0){//父进程
        //pid_t wait(int *wstatus);
        int status=0;
        wait(&status);
        printf("%d\n",status);
    }
    else if(res==0){//子进程
       extern char** environ;
       execve("./child",argv,environ);
    }
    return 0;
}
```

```
guojiawei@ubantu-gjw:~/Desktop/mmm$ gcc exec.c -o child
guojiawei@ubantu-gjw:~/Desktop/mmm$ gcc wait.c 
guojiawei@ubantu-gjw:~/Desktop/mmm$ ./a.out 
hello,world
5120
```

#### 如何提取终止原因

> 提取原理：相应屏蔽字&进程终止状态，屏蔽掉不需要的内容，留下的就是你要的信息

> 系统提供了相应的`带参宏`
>
> **w:wait if (exited;signaled)**
>
> + WIFEXITED(status)：提取出终止原因，判断是否是正常终止
>
> >如果表达式为真：表示进程是正常终止的
> >
> >使用`WEXITSTATUS(status)`，就可以从里面提取出return/exit/_exit返回的“退出状态”
>
> + WIFSIGNALED(status)：提取出终止原因，判断是否是被信号杀死的（异常终止）
>
> > 如果表达式为真：是异常终止的
> >
> > 使用`WTERMSIG(status)`，就可以从里面提取出终止该进程的信号编号

> ```c
> #include<stdio.h>
> #include<stdlib.h>
> int main(){
>     while(1);
>     return 20;
> }
> ```
>
> ```c
> #include <stdio.h>
> #include <stdlib.h>
> #include <sys/types.h>
> #include <unistd.h>
> #include <sys/wait.h>
> int main(int argc,char** argv){
>     //pid_t fork(void);
>     pid_t res=0;
>     res=fork();
>     if(res>0){//父进程
>         //pid_t wait(int *wstatus);
>         int status=0;
>         wait(&status);
>         printf("%d\n",status);
>         if(WIFEXITED(status)){
>             printf("exited:%d\n",WEXITSTATUS(status));
>         }
>         else if(WIFSIGNALED(status)){
>             printf("signal kill:%d\n",WTERMSIG(status));
>         }
>     }
>     else if(res==0){//子进程
>        extern char** environ;
>        execve("./child",argv,environ);
>     }
>     return 0;
> }
> ```
>
> ![image-20221110162532599](/home/guojiawei/.config/Typora/typora-user-images/image-20221110162532599.png)
>
> > guojiawei@ubantu-gjw:~/Desktop/mmm$ ./a.out 
> > 15
> > signal kill:15

#### wait的缺点

如果`父进程fork创建出了好多子进程`，wait只能获取`最先终止`的那个子进程的“终止”状态，其它的将无法获取，如果你想获取所有子进程终止状态，或者只想获取指定子进程的进程终止状态，需要使用wait的兄弟函数`waitpid`，它们的原理是相似的

### 进程状态

> 每个进程与其它进程并发运行时，该进程会在不同的“进程状态”之间进行转换

<img src="/home/guojiawei/.config/Typora/typora-user-images/image-20221110165114173.png" alt="image-20221110165114173" style="zoom:50%;" />

> 阻塞态不会直接切换成执行态，必须通过就绪态，然后再次等待被执行。

执行态：被PID==0(调度进程)的调用进程调度，开始占有CPU，被cpu执行，运行的时间片到后让出CPU，再次进入就绪态，然后PID==0的调度进程开始调度其它进程，CPU去执行其他进程的指令。

> 终止态：进程终止（正常终止、异常终止）
>
> > 如果进程终止时，父进程没有`回收进程资源`的话，终止的进程会变成僵尸进程

### java进程

java属于`解释型语言`，类似的像c#，python等都是属于解释型语言，而c/c++等则是属于`编译型语言`

+ 编译型语言:
  		将编译型语言的程序编译后，得到的直接就是机器指令，可以被CPU直接执行
+ 解释型语言
  		将解释型语言的程序编译后，得到的只是`字节码`，字节码并不是机器指令，并不能直接被cpu执行，只有当`字节码被虚拟机程序解释（翻译）为机器指令`后才能被cpu执行，解释的过程也被称为翻译的过程

#### 有OS支持时运行编译型和解释型语言的程序

##### 运行编译型语言的程序

	（1）父进程（命令行终端窗口、图形界面）会fork复制出子进程空间
	（2）调用exec加载器，直接将编译后代码拷贝到子进程空间，然后被CPU执行时，整个程序就运行起来了

> 在程序里面，我们自己也可以调用fork和exec函数来执行另外的新程序

#####  运行解释型语言的程序

```c
//java程序的运行
1）父进程（命令行窗口、图形界面）会fork复制出子进程空间
2）调用exec加载java虚拟机程序，将虚拟机程序的代码拷贝到子进程空间中
3) 当java虚拟机程序运行起来后，会自动的去解释编译得到的java字节码文件，将字节码翻译为机器指令，cpu再去执行翻译得到的机器指令(//每解释一句，cpu就会执行一句)
//其实最简单的理解就是，java虚拟机就代表了java进程
```

+ 当你运行另一个java程序时，又会自动地启动一个虚拟机程序来解释java字节码，此时另一个java进程又诞生了
+ 也就是说你执行多少个java进程，就会运行多少个java虚拟机，当然java虚拟机程序在硬盘上只有一份，只不过被多次启动而已

> 虚拟机一般是运行在OS上的，不过其实虚拟机也可以运行在没有OS的裸机上

#### 虚拟机程序是什么语言写的

​       虚拟机程序能够被exec`直接加载运行`，说明虚拟机必然是`编译型语言`写的，如果虚拟机使用解释性语言来写的话，编译后得到是字节码，字节码不是机器指令，此时还要另外安装一个程序来解释虚拟机程序，这就陷入了一个死循环                                                                                                                                                                                               所以java虚拟机必须使用`编译型语言`来写，比如使用c/c++编写，一般来说是c写的

### 多进程

```c
同时读鼠标和键盘
在一个进程即读键盘、也读鼠标的话，就会出现相互堵的情况
while(1){
		read(键盘);																	
		read(鼠标);}  
此时就可以fork创建出一个子进程，然后让父子进程并发的读数据
/*
 父进程             子进程
read(键盘)         read(鼠标)
*/
父子进程并发运行，各自读各自的，互不干扰，这样就不会出现相互堵的情况了
```

### 多线程

+ 涉及到并发时，往往会`多线程`来实现

  > 如果程序真的需要并发的话，我们会在进程内部创建多个线程，使用多个线程来实现并发

+ 多进程太过耗费计算机资源

```
	 主线程          次线程
	read(键盘)      read(鼠标)
```

#### 在什么情况下，程序会用到多进程呢

1. 编写框架
   				安卓的系统框架，中大型游戏底层框架等，软件框架属于结构性的代码，在实现框架结构的过程中，基本都要使用`多进程`来搭建这个框架。

2. 程序必须启动新程序

   + 比如windows图形界面、命令行，当我们在图形界面或者命令启动新程序时，必须创建子进程，然后在子进程空间中运行新代码，这个时候必须要涉及多进程

   + 对于大型的软件程序来说，往往包含很多种功能（功能套件），而且每一个`功能具有相当的独立性`，所以大型复杂的软件在实现时，如果将这些独立功能全都放到`单进程`里面来实现，很不现实，因为所有的东西都杂糅在一起，很不好实现

   + > 典型比如360、鲁大师、WPS、腾讯QQ，在任务管理器里面你会发现，它们有很多的进程

   > 对于大型软件来说，多进程 和 多线程都会涉及

3. 早期服务器，会通过多进程来支持多用户的访问

> 不过对于现在的服务器来说，更喜欢使用多线程来实现多客户的访问

### 进程关系

进程间的关系，大致有三种，即`父子关系`、`进程组关系`、`会话期关系`

+ 父子关系

> 已有进程调用fork创建出一个新的进程，那么这两个进程之间就是父子进程关系，子进程会继承和父进程的属性

+ 进程组关系

多个进程可以在一起组成一个`进程组`，其中某个进程会担任组长，`组长进程的pid`就是整个进程组的组ID

> 进程组的生命周期:
>
> 就算进程组的组长终止了，只要进程中还有一个进程存在，这个进程组就存在。进程组从开始被创建，到进程组最后一个进程结束，这段时间就是进程组的生命周期。

自己的程序fork出很多子进程时，所有的进程会默认的为一组，自己程序中的`原始父进程`会担任组长

> 调用`相关API`可以
>
> + 将一个非组长进程，设置为一个新的进程组组长
> + 或者说加入其它进程组

+ 会话期关系

**多个进程组在一起，就组成了会话期。**

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
int main(){
    pid_t res=0;
    printf("before fork\n");
    res=fork();
    if(res>0){//父进程
        printf("parent PID:%d\n",getpid());
        printf("parent ret:%d\n",res);
    }
    else if(res==0){//子进程
        printf("child PID:%d\n",getpid());
        printf("parent ret:%d\n",res);
        sleep(1);
    }
    printf("after fork\n");
    return 0;
}
```

><img src="/home/guojiawei/.config/Typora/typora-user-images/image-20221110175120391.png" alt="image-20221110175120391" style="zoom:50%;" />
>
>命令行终端窗口与最开始的父进程建立联系，最原始的父进程结束了，就会将终端交互权还给shell程序
>
>父进程先于子进程死掉（执行完），所以多打印了最后一句话，“after fork”前

### 守护进程

守护进程也被称为精灵进程

> 一些稍大型的软件来说，往往都是多进程的
>
> + 而且其中好多进程都是默默无闻运行的
>
> + 默默无闻运行的进程都需要做成`守护进程`
### 1:dup

>+ <font color=red>功能</font>
>
>复制某个已经打开的文件描述符，得到一个新的描述符，这个新的描述符，也指向被复制描述符所指向的文件。
>
><img src="/home/guojiawei/.config/Typora/typora-user-images/image-20221026195818155.png" alt="image-20221026195818155" style="zoom:67%;" />
>
>+ 函数原型
>
>  ```c
>   #include <unistd.h>
>   int dup(int oldfd);
>  //oldfd：会被复制的、已经存在的文件描述符。
>  ```

+ 返回值
  + **成功**------返回复制后的新文件描述符
  + **失败**-----返回-1，errno被设置

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#define FILE_IO  "io.txt"
void print_error(char* str){
    perror(str);
    exit(-1);
}
int main(){
    int fd1=open(FILE_IO,O_RDWR|O_TRUNC);
    int fd2;
    if(fd1==-1) print_error("open fail!\n");
    fd2=dup(fd1);
    printf("fd1=%d,fd2=%d\n",fd1,fd2);
    return 0;
}
```

```c
write(fd1,"hello\n",6);
write(fd2,"world\n",6);
```

输出结果：<img src="/home/guojiawei/.config/Typora/typora-user-images/image-20221026201237672.png" alt="image-20221026201237672" style="zoom:67%;" />

### 2:dup2

+ 函数原型

  ```c
  #include <unistd.h>
  int dup2(int oldfd, int newfd);
  ```

  + 功能同dup，只不过在dup2里面，我们可以自己**指定新文件描述符**
  + 如果这个新文件描述符已经被打开了，dup2会把它给关闭后，再使用  

+ 返回值
  					1）成功：返回复制后的新文件描述符
  			        2）失败：返回-1，并且errno被设置。

+ 参数
  					oldfd：会被复制的、已经存在的文件描述符。
  				  newfd：新的文件描述符

  ```c
  fd2=dup2(fd1,4);
  //fd2=4 dup2(fd1,fd2);	
  ```

<img src="/home/guojiawei/.config/Typora/typora-user-images/image-20221026202335684.png" alt="image-20221026202335684" style="zoom:50%;" />

<FONT COLOR=RED>这里不加O_APPEND,依然不会相互覆盖</FONT>

> 	   使用dup、dup2复制方式实现文件共享时，不管复制出多少个文件描述符，它们永远只有一个文件表，所以使用所有描述符去操作文件时，最后使用的都是通过同一个文件位移量，不管谁操作后文件位移量都会被更新，因此不会出现覆盖。

+ 也就是两个open去打开的，对应各自的文件表；而dup和duo2复制的是同一个文件表

<img src="/home/guojiawei/.config/Typora/typora-user-images/image-20221026203016523.png" alt="image-20221026203016523" style="zoom:80%;" />

<font color=red>**dup和dup2实现文件共享和重定位**</font>

### 重定位实现

​        某文件描述符原来指向了A文件，输出数据是输出到A文件，但是重定位后，文件描述符指向了B文件，输出时数据输出到了B文件，这就是重定位。
​        所谓重定位-------文件描述符所指向的文件该变了，使得数据输出的目标文件也随之变化					

思考：

​        printf函数底层调用是write，而且write的第一个参数固定是1，现在想把printf的结果从输出屏幕改成输出文件，该怎么办？

​        利用重定位解决，既然1不能改变，就把屏幕输出文件强制关闭---close(1),然后利用dup,把1的文件指向新的文件描述符，从而达到目的。

> <img src="/home/guojiawei/.config/Typora/typora-user-images/image-20221026204036836.png" alt="image-20221026204036836" style="zoom:67%;" />

```c
int main()
{
    int fd1 = open(FILE_IO, O_RDWR | O_TRUNC);
    int fd2;
    if (fd1 == -1)
        print_error("open fail!\n");
    close(1);
    fd2 = dup(fd1);
    //因为1被关闭了，最小没被使用的文件描述符就是1，fd2一定是1
    //注意打印到fd1指向的文件
    printf("fd2=%d\n", fd2);
    return 0;
}
```

<img src="/home/guojiawei/.config/Typora/typora-user-images/image-20221026204621092.png" alt="image-20221026204621092" style="zoom:67%;" />

```c
int main()
{
    int fd1 = open(FILE_IO, O_RDWR );
    int fd2= dup2(fd1,1);
    printf("hellos");
    return 0;
}//也可以实现由屏幕打印到文件----dup2,无需close(1)
```

### >就是重定位

+ ls > file.txt

  ls命令（程序）原本会调用write(1, ...)，将结果输出到标准输出文件（显示器），但是 >会调用dup2，把3复制到1上，实现重定位，让1也指向file.txt文件

### **文件的共享操作**

	（1）回顾单一进程多次open同一个文件，实现共享操作

	（2）多个进程多次open，共享操作同一个文件

	（3）在单个进程中，使用dup、dup2实现文件共享操作

### 3:fcntl函数

+ 函数原型

  ```c
  #include <unistd.h>
  #include <fcntl.h>
  int fcntl(int fd, int cmd, ... /* arg */ );
  //... /* arg */------意思是可选，有些功能用到这个参数，有的不会
  ```

+ 功能

  fcntl函数其实是File Control的缩写，**通过fcntl可以设置、或者修改已打开的文件性质**。

+ 返回值

  + 调用成功：返回值视具体参数而定，这个后面还会再介绍
  + 调用失败：返回-1，并把错误号设置给errno。							

+ 参数

  <font color=red>cmd：控制命令，通过指定不同的宏来修改fd所指向文件的性质。</font>

  （a）**F_*DUP*FD**
  							复制描述符，可用来用来模拟dup和dup2
  							返回值：返回复制后的新文件描述符号
  （b）**F_GETFL、F_SETFL**
  			        		获取、设置文件状态标志  

  ​                            比如在open时没有指定O_APPEND，可以使用fcntl函数来补设 

  ​                            返回值：返回文件的状态

  #### 什么时候需要fcntl来补设？

  > 当文件描述符不是你自己open得到，而是**调用别人给的函数**，别人的函数去open某个文件，然后再将文件描述符返回给你用，在这种情况下，我们是没办法去修改别人的函数，甚至在他调用的open函数里`补加文件状态标志`。此时就可以使用fcntl来补设了，使用fcntl补设时，你只需要知道`文件描述符`即可。	

#### 使用fcntl模拟dup,dup2

> 真要进行文件描述符复制时，往往都使用dup、dup2来实现，而不会使用fcntl

```c
int main()
{
    int fd= open(FILE_IO, O_RDWR );
    /*1：dup()函数实现
    close(1);
    dup(fd);*/
    close(1);
    //int fcntl(int fd, int cmd, ... /* arg */ );
    fcntl(fd,F_DUPFD,0);//0就是不用
    printf("hello");
    return 0;
}
```

~~~c
int main()
{
    int fd= open(FILE_IO, O_RDWR );
    /*1：dup2()函数实现
    dup2(fd,1);*/
    //int fcntl(int fd, int cmd, ... /* arg */ );
    close(1);
    fcntl(fd,F_DUPFD,1);
    printf("kchdskhgkl");
    return 0;
}
~~~

####  使用fcntl模拟O_APPEND

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#define FILE_IO  "io.txt"
int open_fun1(void){
    int fd= open(FILE_IO, O_RDWR );
    if(fd==-1){
        printf("open fails!!");
        return 0;
    }
    return fd;
}
int open_fun2(void){
    int fd= open(FILE_IO, O_RDWR );
    if(fd==-1){
        printf("open fails!!");
        return 0;
    }
    return fd;
}
int main()
{
    int fd1=open_fun1();
    int fd2=open_fun2();
    int i=0;
    while(i++<10){
        write(fd1,"hello\n",6);
        sleep(1);
        write(fd2,"world\n",6);
    }
    return 0;
}
```

最后：

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#define FILE_IO  "io.txt"
int open_fun1(void){
    int fd= open(FILE_IO, O_RDWR );
    if(fd==-1){
        printf("open fails!!");
        return 0;
    }
    return fd;
}
int open_fun2(void){
    int fd= open(FILE_IO, O_RDWR );
    if(fd==-1){
        printf("open fails!!");
        return 0;
    }
    return fd;
}
int main()
{
    int fd1=open_fun1();
    int fd2=open_fun2();
    //funtl补充标志，新标志覆盖原标志
    int flag=O_WRONLY|O_APPEND;
    fcntl(fd1,F_SETFL,flag);
    //保留原标志，补充新标志
    flag=fcntl(fd2,F_GETFL,0);
    flag=flag|O_TRUNC|O_APPEND;
    fcntl(fd2,F_SETFL,flag);
    int i=0;
    while(i++<10){
        write(fd1,"hello\n",6);
        sleep(1);
        write(fd2,"world\n",6);
    }
    return 0;
}
```


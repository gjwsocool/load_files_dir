### lstat

1. 功能

   > + 与stat几乎完全一样，都是从块设备上的inode节点空间中读取文件的属性信息
   >
   > + 但是与stat唯一不同的是，lstat会区分<font color=red>链接文件</font>
   >
   >   >当操作的文件是链接文件时：
   >   >
   >   >+ stat获取的文件属性是链接文件背后所指向的文件，而不是链接文件的。
   >   >+ lstat直接显示的是链接文件本身的文件属性

#### 什么是链接文件

​        如需要在系统上维护同一文件的两份或多份副本，除了保存多份单独的物理文件副本之外，还可以采用保存**一份物理文件副本和多个虚拟副本**的方法。                                                                                                                                          这种`虚拟的副本就称为链接`。链接是目录中指向文件真实位置的占位符。

> 在Linux中有两种不同类型的文件链接： 
>
> - 符号链接（软链接）
>   + 符号链接就是一个实实在在的文件，它指向存放在虚拟目录结构中某个地方的另一个文件。
>   + 这两个**通过符号链接在一起**的文件，彼此的**内容并不相同**。
>   + 使用 **ln -s** 命令创建符号链接时，源文件必须存在，链接文件必须不存在。
> - 硬链接

##### 具体操作

创建mm.txt的链接文件pmm

![image-20221103164721376](/home/guojiawei/.config/Typora/typora-user-images/image-20221103164721376.png)

用stat获取mm.txt和pmm的属性

![image-20221103164901317](/home/guojiawei/.config/Typora/typora-user-images/image-20221103164901317.png)

>发现stat获取链接文件的属性是其指向的文件；也就是stat获取的pmm属性其实是mm.txt

```c
int main(int argv,char** argc){
    if(argv!=2) printf("输入参数个数错误！\n");
    struct stat aaa={0};
    lstat(argc[1],&aaa);//改用lstat
    printf("%d %lu %d %d %ld  %s\n",\
            aaa.st_mode,aaa.st_nlink,aaa.st_uid,\
            aaa.st_gid,aaa.st_size,argc[1]);
    return 0;
}
```

![image-20221103165214923](/home/guojiawei/.config/Typora/typora-user-images/image-20221103165214923.png)

>lstat获取的链接文件pmm的属性是自身的

### fstat

> 与stat不同的是，fstat是使用<font color=red>文件描述符</font>来操作的，当你不知道文件路径名，但是你知道指向这个文件的描述符时，就可以使用fstat来操作

```c
int main(int argv,char** argc){
    if(argv!=2) printf("输入参数个数错误！\n");
    struct stat aaa={0};
    int fd=open(argc[1],O_RDWR);
    fstat(fd,&aaa);//fstat
    printf("%d %lu %d %d %ld  %s\n",\
            aaa.st_mode,aaa.st_nlink,aaa.st_uid,\
            aaa.st_gid,aaa.st_size,argc[1]);
    return 0;
}
```

> fstat和stat一样，不区分链接文件

##### ls背后调用的是lstat

### chown命令

![image-20221103173149637](/home/guojiawei/.config/Typora/typora-user-images/image-20221103173149637.png)

>功能：用于修改文件的属主
>
>+ 修改所属用户-------chown 新的所属用户 文件
>+ 修改所属组-----------chown :新的组 文件
>+ 同时修改----------chown 新的所属用户:新的组 文件

### unmask

##### open创建文件给权限777依然不给其他用户写权限

> open函数创建新的文件时，如果指定的是0777满级权限的话，实际创建文件权限为0775（rwxrwxr-x）
>
> --------
>
> 因为<font color=red>被文件权限掩码做了限制</font>。
> 111 111 111  （0777）&~000 000 010  （0002）文件权限掩码
> 				                         （111 111 101）				
>
> ----
>
> **限制其它用户的写权限**

`使用umask即可将文件权限掩码改为0`

```c
#include <sys/types.h>
#include <sys/stat.h>
//功能：修改文件权限掩码
//参数mask：新的文件权限掩码
//返回值是旧的文件权限掩码
mode_t umask(mode_t mask);
```

> ```c
> int main(void){
>     int fd=0;
>     mode_t ret=umask(0);
>     printf("oldUmask=%d\n",ret);
>     fd=open("hhh",O_RDWR|O_CREAT,0777);
>     //mode_t umask(mode_t mask);
>     printf("umask=%d",umask(ret));
>     return 0;
> }
> ```
>
> <img src="/home/guojiawei/.config/Typora/typora-user-images/image-20221103175900422.png" alt="image-20221103175900422" style="zoom:67%;" />

----

符号链接文件的文件大小 

> + 符号链接文件就是一个快捷键，背后指向了某个文件。
> + 符号链接文件的数据，就是所指向文件的文件名，所以它的文件大小指的就是这个名字的字符个数。
>
> ​                  pfile -> file.txt	

### 文件截断函数	

open时，可以指定了<font color=red>O_TRUNC</font>后，文件里面有数据的话，会将打开的文件截短（清空）为0.

> 文件截短的函数truncate，它不仅能够将文件截为0，还可以把文件截短为任意长度

```c
#include <unistd.h>
#include <sys/types.h>
// 函数功能----将文件长度截短为length所指定长度
// 返回值---成功返回0，失败返回-1，errno被设置
int truncate(const char *path, off_t length);
int ftruncate(int fd, off_t length);		
//truncate利用文件路径名操作，ftruncate利用文件描述符操作
```

```c
truncate("mm.txt",10);
   int fd=open("mm.txt",O_RDWR);
ftruncate(fd,3);
```

> 截断字符超过文件长度时候：
>
> ![image-20221103181205671](/home/guojiawei/.config/Typora/typora-user-images/image-20221103181205671.png)


​			
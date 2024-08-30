# git本地仓库和远程仓库

### :recycle:创建一个新的github仓库

![image-20240830153200224](/home/guojiawei/.config/Typora/typora-user-images/image-20240830153200224.png)

### :recycle:初始化本地仓库

> 创建一个文件夹，这里是：linux_Git_share

1. <font color=red>git config 配置用户名邮箱</font>

```shell
git config --global user.name "用户名"
git config --global user.email "邮箱"
```

> ![image-20240830155445156](/home/guojiawei/.config/Typora/typora-user-images/image-20240830155445156.png)

当然也可以配置颜色显示----不同阶段的文件

```shell
git config --global color.ui true
```

<mark>使用git config  --global --list 可以查看配置的信息</mark>

![image-20240830155737819](/home/guojiawei/.config/Typora/typora-user-images/image-20240830155737819.png)

> 可以不加--global
>
> `git config --list`

2. <font color=red>git init初始化本地仓库</font>

![image-20240830160357356](/home/guojiawei/.config/Typora/typora-user-images/image-20240830160357356.png)

### :recycle:代码的上传流程

#### 代码从本地仓库提交到远程仓库的步骤

![image-20240830161737905](/home/guojiawei/.config/Typora/typora-user-images/image-20240830161737905.png)

> 在本地目录的新文件需要<font color=red style=background-color:yellow>git add 文件名</font>后才能上传至本地仓库的暂存区
>
> 然后使用<font color=red style=background-color:yellow>git commit  -m  '注释'</font>才能正式提交至本地仓库。

> ![image-20240830163127733](/home/guojiawei/.config/Typora/typora-user-images/image-20240830163127733.png)
>
> > **git add**上传至本地仓库暂存区
>
> ![image-20240830163356796](/home/guojiawei/.config/Typora/typora-user-images/image-20240830163356796.png)
>
> > **git commit**正式提交至本地仓库
>
> ![image-20240830163708712](/home/guojiawei/.config/Typora/typora-user-images/image-20240830163708712.png)

#### 代码写错如何回滚

> 已经提交的内容如果修改后需要重新提交

+ 使用<font color=red style=background-color:yellow>git log或者git reflog </font>可以查看提交文件的版本

![image-20240830165510026](/home/guojiawei/.config/Typora/typora-user-images/image-20240830165510026.png)

+ 使用git reset回滚到指定版本

![image-20240830165500670](/home/guojiawei/.config/Typora/typora-user-images/image-20240830165500670.png)

> 此时查看reflog
>
> ![image-20240830170015253](/home/guojiawei/.config/Typora/typora-user-images/image-20240830170015253.png)

### :recycle:ssh连接本地仓库和远程仓库

> 生成密钥
>
> ![image-20240830184031431](/home/guojiawei/.config/Typora/typora-user-images/image-20240830184031431.png)
>
> ---
>
> 生成后，连续按下三次确认，不输入内容
>
> ![image-20240830184518289](/home/guojiawei/.config/Typora/typora-user-images/image-20240830184518289.png)
>
> ---
>
> 然后查看生成的公有密钥并复制
>
> ```c++
> cat .ssh/id_rsa.pub 
> ```
>
> ![image-20240830185308718](/home/guojiawei/.config/Typora/typora-user-images/image-20240830185308718.png)

#### 把公有密钥粘贴至github

![image-20240830185749363](/home/guojiawei/.config/Typora/typora-user-images/image-20240830185749363.png)

#### 添加远程仓库

```shell
git remote add 远程仓库名 github的ssh链接
```

![image-20240830190654461](/home/guojiawei/.config/Typora/typora-user-images/image-20240830190654461.png)

>![image-20240830191134693](/home/guojiawei/.config/Typora/typora-user-images/image-20240830191134693.png)
>
>---
>
><font color=red>git remote</font>返回仓库名就是成功了，-v可以查看详细信息
>
>![image-20240830191357834](/home/guojiawei/.config/Typora/typora-user-images/image-20240830191357834.png)

### :recycle:把本地代码推送到远程仓库

```shell
git push -u origin "master"
```

![image-20240830193409963](/home/guojiawei/.config/Typora/typora-user-images/image-20240830193409963.png)



### :recycle:下载代码到本地

```shell
git clone 链接
```

![image-20240830192340074](/home/guojiawei/.config/Typora/typora-user-images/image-20240830192340074.png)

### :recycle:git本地仓库的分支branch

+ <mark>git brunch可以查看当前本地仓库的分支</mark>

>![image-20240830225734930](/home/guojiawei/.config/Typora/typora-user-images/image-20240830225734930.png)

+ `创建一个分支mybranch`

> **git branch +分支名**

```shell
git branch mybranch
```

+ <mark>git checkout 分支名可以切换对应分支</mark>

![image-20240830230417416](/home/guojiawei/.config/Typora/typora-user-images/image-20240830230417416.png)

+ 将分支代码合并，不同分支合并

<mark>git merge 分支名</mark>

```shell
git merge mybranch 
```

![image-20240830232041556](/home/guojiawei/.config/Typora/typora-user-images/image-20240830232041556.png)

### :recycle:git的标签

> git tag可以直接查看当前标签，标签是给当前分支标记标签
>
> ![image-20240830233328885](/home/guojiawei/.config/Typora/typora-user-images/image-20240830233328885.png)

```shell
git tag -a "标签名" -m 注释
```

![image-20240830232843983](/home/guojiawei/.config/Typora/typora-user-images/image-20240830232843983.png)
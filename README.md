# IOspeedtest

Test C++ iostream speed,  and the impaction of sync_with_stdio()  and cin.tie()

[BLog](https://liyzcj.github.io/c++/2015/09/12/c++-test_cin_speed/)

## 摘要

实验七主要是测试不同方法读取速度的不同.

通过对不同的方法测速, 了解 `ios::sync_with_stdio(false)` 与 `iOS::tie()`对cin 函数读入速度的影响.

其中 `ios::sync_with_stdio()` 主要是为了实现 cin/cout 与 c语言的 scanf/pringf的同步, 意思就是在读取文件时的指针是同步的, 方便两个函数混用.

而 `ios::tie()` 是绑定输入输出流, 默认 cin 与 cout 是绑定在一起. 绑定的意思即每当一个对象有输入输出操作时, 就会同步刷新绑定对象的 streambuf.

实验过程: 首先使用 `generate()` 函数生成 1000,0000 个整数存入 `data.txt`.

然后再使用 `evaluate()` 函数来评估不同方法的读取所消耗的时间.

## 使用方法

所有方法如下十种:

| 代号         | 描述                                       |
|--------------|--------------------------------------------|
| cin          | freopen()到stdin, 再用cin读取读取;         |
| scanf        | freopen()到stdin, 再用scanf读取;           |
| fread        | 使用fread() 读取整个字符串再处理           |
| read         | 使用read() 读取整个字符串再处理            |
| mmap         | 使用mmap()映射进内存再处理                 |
| pascal       | 使用pascal读取                             |
| ifstream     | 直接使用ifstream读取文件;                  |
| cin~sync     | 使用cin,并关闭 sync_with_stdio(); 关闭同步 |
| cin~tie      | 使用cin, 并关闭 cin.tie(); 解除绑定        |
| cin\~tie\~sync | 等价于 cin\~sync + cin\~tie                  |

💡 注意: 不可在一次评测中评测 cin~sync cin~tie cin\~tie\~sync cin. 
因为 tie(NULL); sync_with_stdio(false); 执行后会一直有效

使用方法:

  1. 运行环境: Linux

  2. 第一次运行会生成 `data.txt`.

  3. 直接修改程序的 vector 选择要评测的方法. 再编译运行即可.

  4. 程序会对每种方法调用十次, 最后输出平均时间

## 文件结构:

- Makefile : make编译cpp文件
- data.txt : 每行一个整数, 一共 1000,0000个 (第一次生成)
- load_data : pascal 编译后的执行程序
- load_data.pas : pascal 源码
- seventh.cpp : 实验c++源码
- seventh.out : 编译后的执行程序

## 实验结果

| 方法         | 平均时间(s) |
|--------------|-------------|
| cin          | 3.93276     |
| scanf        | 1.57656     |
| fread        | 0.45389     |
| read         | 0.42889     |
| mmap         | 0.41451     |
| pascal       | 1.41711     |
| ifstream     | 0.99613     |
| cin~sync     | 1.06971     |
| cin~tie      | 3.87770     |
| cin~tie~sync | 1.02246     |

## 结论

1. 由结果来看 sync_with_stdio() 是阻碍 cin 速度的关键.
2. cin.tie() 可以稍微再让 cin 的速度快一点点
3. sync_with_stdio() 关闭后, cin 比 scanf 还要快
4. pascal 和 scanf 相当
5. ifsteram 的速度比 cin 从 stdin 要快
6. 先读进内存再处理的有 fread, read, mmap. 其中 read 和 mmap 相当, fread 稍慢一点点.
7. 先整个读进内存再处理比从文件读取快很多, 大概只要0.5s, 但是这是牺牲了内存空间换来的.
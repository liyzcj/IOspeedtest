#include<iostream>
#include<fstream>
#include<random>
#include<iomanip>  // io manipulator
#include <fcntl.h>  // O_RDONLY for read()
#include<unistd.h>  // linux read()
#include <sys/mman.h> // for mmap()
#include <cstdlib> // for pascal()
#include<sys/time.h> // for gettimeofday()
#include<map>
#include<vector>

using namespace std;


/*
Create by liyanzhe 2019.4.21
摘要: 本实验主要是为了测试不同方法的读取速度.
所有方法如下十种:
- "cin"         : freopen()到stdin, 再用cin读取读取;
- "scanf"       : freopen()到stdin, 再用scanf读取;
- "ifstream"    : 直接使用ifstream读取文件;
- "cin~sync"    : 使用cin,并关闭 sync_with_stdio(); 关闭同步
- "cin~tie"     : 使用cin, 并关闭 cin.tie(); 解除绑定
- "cin~tie~sync": 等价于 cin~sync + cin~tie
- "fread"       : 使用fread() 读取整个字符串再处理
- "read"        : 使用read() 读取整个字符串再处理
- "mmap"        : 使用mmap()映射进内存再处理
- "pascal"      : 使用pascal读取

! 注意: 不可在一次评测中评测 cin~sync cin~tie cin~tie~sync cin. 
因为 tie(NULL); sync_with_stdio(false); 执行后会一直有效

使用方法:
  运行环境: Linux
  直接修改下方的 vector 选择要评测的方法. 再编译运行即可.
*/

vector<string> evaluate_methods = {
  "cin",
  "scanf",
  "ifstream",
  "cin~sync",
  "cin~tie",
  "cin~tie~sync",
  "fread",
  "read",
  "mmap",
  "pascal"
};


//读取函数
void cin_read();
void scanf_read();
void cin_read_nosync();
void ifstream_read();
void cin_read_notie();
void cin_read_nosync_notie();
void fread_parse();
void read_parse();
void mmap_parse();
void pascal();


// 功能函数
void generate();
double evaluate(void (*func)());
void parse(char *buf);
ostream& operator<< (ostream& sm, map<string, double>& m);

const int MAXN = 10000000; // data个数
const int ITER = 10; // 每次实验循环次数
const int MAXS = 120*1024*1024; //存储整个文件

typedef void (*method)();
map<string,method> methods = {
    {"cin", cin_read},
    {"scanf", scanf_read},
    {"cin~sync", cin_read_nosync},
    {"cin~tie", cin_read_notie},
    {"cin~tie~sync", cin_read_nosync_notie},
    {"ifstream", ifstream_read},
    {"fread", fread_parse},
    {"read", read_parse},
    {"mmap", mmap_parse},
    {"pascal", pascal}
};

char buf[MAXS];
int data[MAXN];
int i;




// Main
int main(){

  {
  ifstream fin("data.txt");
  if(!fin)
    {
      cout << "Generating 'data.txt' !! " << endl;
      generate();
    }
  } // RAII

  map<string, double> res; // save results

  for (auto m : evaluate_methods)
  {
    cout << "Start Evaluate: " << setw(25) << setfill('-') << m << endl;
    res[m] = evaluate(methods[m]);
  }
  cout << res << endl;
  // evaluate(methods["test1"]);
  return 0;
}


// cin 读取
void cin_read(){
  freopen("data.txt", "r", stdin);
  for (i=0; i<MAXN; i++){
    cin >> data[i];
  }
}

// 取消 ios::sync_with_stdio()
void cin_read_nosync(){
  freopen("data.txt", "r", stdin);
  ios::sync_with_stdio(false);
  for (i=0; i<MAXN; i++){
    cin >> data[i];
  }
}

// 取消 cin.tie()
void cin_read_notie(){
  freopen("data.txt", "r", stdin);
  cin.tie(NULL);
  for (i=0; i<MAXN; i++){
    cin >> data[i];
  }
}

// 同时取消sync 与 tie
void cin_read_nosync_notie(){
  freopen("data.txt", "r", stdin);
  ios::sync_with_stdio(false);
  cin.tie(NULL);
  for (i=0; i<MAXN; i++){
    cin >> data[i];
  }
}

// scanf 读取
void scanf_read(){
  freopen("data.txt", "r", stdin);
  for (i=0; i<MAXN; i++){
    scanf("%d",&data[i]);
  }
}

// ifstream 读取
void ifstream_read(){
  ifstream inf("data.txt");
  for (i=0; i<MAXN; i++){
    inf >> data[i];
  }
}

//先fread读取字符串再parse
void fread_parse(){
  freopen("data.txt", "rb", stdin);
  int len = fread(buf, 1, MAXS, stdin);
  buf[len] = '\0';
  parse(buf);
}

//先read读取字符串再parse
void read_parse(){
  int fd = open("data.txt", O_RDONLY);
  int len = read(fd, buf, MAXS);
  buf[len] = '\0';
  parse(buf);
}

// mmap 读取 
void mmap_parse(){
  int fd = open("data.txt",O_RDONLY);
  int len = lseek(fd,0,SEEK_END);
  char *mbuf = (char *) mmap(NULL,len,PROT_READ,MAP_PRIVATE,fd,0);
  // buf = mbuf
  parse(mbuf);
}

//pascal
void pascal(){
  system("./load_data");
  i = MAXN;
}

//parse 函数
void parse(char *buf){
  i=0;
  data[i] = 0;
  for (char *p=buf; *p != '\0'; p++)
  {
    if (*p == '\n' && *(p+1) != '\0')
      data[++i] = 0;
    else
      data[i] = data[i] * 10 + *p - '0';
  }
  i++;
}

// 评估函数
double evaluate(void (*func)()){

  double time_use, totaltime = 0;
  struct timeval start;
  struct timeval end;
  /*
  struct  timeval{
  long  tv_sec;  // 秒
  long  tv_usec; // 微妙
  };
  */

  for (int it=0; it<ITER; it++)
  {
    gettimeofday(&start,NULL); // start time
    func();
    if (i == MAXN)
      cout << "Iter:" << setw(2) << setfill(' ') << it+1 << "\tState: success\t";
    else
    {
      cout << "Error";
      break;
    }
    gettimeofday(&end,NULL); // end time
    time_use = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec)/1000000.0;
    totaltime += time_use;
    cout << "Time: " << setprecision(5) << fixed << time_use << endl;
  }
  return totaltime/ITER;
}


// 生成随机数到 data.txt
void generate(){
  default_random_engine e;
  ofstream of("data.txt");
  for(int i=0; i<MAXN; ++i)
    of<<e()<<'\n';
}

// 重写 << 输出 result
ostream& operator<< (ostream& sm, map<string, double>& m) {
  sm << setw(45) << setfill('-') << "Result:" << endl;
  for (auto v : m){
    sm << "Method: " << setw(12) << left << setfill(' ') << v.first;
    sm << "Average Time: " << v.second << endl;
  }
  return sm;
}
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <thread>
#include <list>
// #include "./jemalloc-3.6.0/include/jemalloc/jemalloc.h"
using namespace std;

/*
  连续分配释放10000次内存(1024)性能测试, g++ 未优化编译
  单线程耗时: 
  标准库malloc耗时: 4230us free耗时: 667us
  使用dlmalloc耗时: 4102us, dlfree耗时: 853us
  使用jemalloc耗时: 1119us, jefree耗时: 525us
  4个线程平均耗时:
  标准库malloc, free耗时: 36641-72191us
  使用dlmalloc, dlfree耗时: 2232us-6131us
  使用jemalloc耗时: 
*/

#define BLOCK_SIZE (1024)

inline int64_t GetTickCountLinux()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000000 + ts.tv_nsec / (1000));
}

void test_func()
{
    list<char*> li;
    for (int i = 0; i < 10000; ++i)
    {
        char* p = (char*)malloc(BLOCK_SIZE);
        li.push_back(p);
        //free(p);
    }
    for (auto ptr : li)
    {
        free(ptr);
    }
    
}

int main(int argc, char** argv)
{
    time_t tCurTm = GetTickCountLinux();
    thread t1(bind(test_func));
    thread t2(bind(test_func));
    thread t3(bind(test_func));
    thread t4(bind(test_func));
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    
    cout << "cost tm: " << (GetTickCountLinux() - tCurTm) / 4 << "us" << endl;
    // cout << "free cost tm: " << (GetTickCountLinux() - tCurTm) << "us" << endl;
    return 0;
}
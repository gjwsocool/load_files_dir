#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
using namespace std;
/*8个线程,3个写,5个读*/
//共享数据
int nums = 100;
// 初始化锁
pthread_rwlock_t rw;
//线程函数
void* writeNum(void* arg){
    while(1){
        pthread_rwlock_wrlock(&rw);
        nums++;
        cout << pthread_self() << "is write,nums=" << nums << endl;
        pthread_rwlock_unlock(&rw);
        usleep(100);
    }
    return NULL;
}
void* readNum(void* arg){
    while(1){
        pthread_rwlock_rdlock(&rw);
        cout << pthread_self() << "is read,nums=" << nums << endl;
        pthread_rwlock_unlock(&rw);
        usleep(100);
    }
    return NULL;
}
int main(){
    //初始化锁
    pthread_rwlock_init(&rw,NULL);
    // 创建线程
    pthread_t wtid[3],rtid[5];
    for (int i = 0; i < 3;i++)
        pthread_create(&wtid[i], NULL, writeNum, NULL);
    for (int i = 0; i < 5;i++)
        pthread_create(&rtid[i], NULL, readNum, NULL);
    //设置线程分离
    for (int i = 0; i < 3;i++)
        pthread_detach(wtid[i]);
    for (int i = 0; i < 5;i++)
        pthread_detach(rtid[i]);
    pthread_exit(NULL);
    //释放锁
    pthread_rwlock_destroy(&rw);
    return 0;
}
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
using namespace std;
/* 三个窗口一起卖1000张票 */
int ticket = 1000;//全局
/*互斥量:全局变量*/
pthread_mutex_t mutex;
void *sellTicket(void *arg){
    while(1){
        /*加锁*/
        pthread_mutex_lock(&mutex);
        if (ticket > 0)
        {
            usleep(1000);
            cout << pthread_self() << "在卖第" << ticket << "张票" << endl;
            ticket--;
        }else{
            /*解锁*/
            pthread_mutex_unlock(&mutex);
            break;
        }
        //解锁
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
int main() {
    //初始化互斥量
    pthread_mutex_init(&mutex, NULL);
    // 创建3个子线程,主线程只进行线程资源回收
    pthread_t tid1, tid2, tid3;
    pthread_create(&tid1, NULL, sellTicket, NULL);
    pthread_create(&tid2, NULL, sellTicket, NULL);
    pthread_create(&tid3, NULL, sellTicket, NULL);
    //回收子线程资源
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    //退出主线程
    pthread_exit(NULL);
    //释放互斥量
    pthread_mutex_destroy(&mutex);
    return 0;
}
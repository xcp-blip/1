#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

// 信号量定义
sem_t rw_mutex;     // 读写互斥锁（写者独占，读者共享）
sem_t mutex;        // 保护读者计数
int read_count = 0; // 正在读的读者数量

// 读者线程
void *reader(void *arg) {
    int id = *(int *)arg;
    free(arg);

    while (1) {
        // 1. 申请读者计数锁
        sem_wait(&mutex);
        read_count++;
        // 第一个读者，阻塞写者
        if (read_count == 1)
            sem_wait(&rw_mutex);
        sem_post(&mutex);

        // ============= 开始读 =============
        printf("【读者 %d】正在读取数据...\n", id);
        sleep(1); // 模拟读取耗时
        printf("【读者 %d】读取完成\n", id);

        // 2. 释放读者计数锁
        sem_wait(&mutex);
        read_count--;
        // 最后一个读者，释放写者
        if (read_count == 0)
            sem_post(&rw_mutex);
        sem_post(&mutex);

        sleep(2); // 读间隔
    }
}

// 写者线程
void *writer(void *arg) {
    int id = *(int *)arg;
    free(arg);

    while (1) {
        // 申请独占访问
        sem_wait(&rw_mutex);

        // ============= 开始写 =============
        printf("【写者 %d】正在写入数据...\n", id);
        sleep(2); // 模拟写入耗时
        printf("【写者 %d】写入完成\n", id);

        // 释放独占访问
        sem_post(&rw_mutex);

        sleep(3); // 写间隔
    }
}

int main() {
    pthread_t tid_r1, tid_r2, tid_r3; // 3个读者
    pthread_t tid_w1, tid_w2;         // 2个写者

    // 初始化信号量
    sem_init(&rw_mutex, 0, 1);
    sem_init(&mutex, 0, 1);

    // 创建线程
    int *id1 = malloc(sizeof(int)); *id1 = 1;
    int *id2 = malloc(sizeof(int)); *id2 = 2;
    int *id3 = malloc(sizeof(int)); *id3 = 3;
    int *id4 = malloc(sizeof(int)); *id4 = 1;
    int *id5 = malloc(sizeof(int)); *id5 = 2;

    pthread_create(&tid_r1, NULL, reader, id1);
    pthread_create(&tid_r2, NULL, reader, id2);
    pthread_create(&tid_r3, NULL, reader, id3);
    pthread_create(&tid_w1, NULL, writer, id4);
    pthread_create(&tid_w2, NULL, writer, id5);

    // 等待线程结束
    pthread_join(tid_r1, NULL);
    pthread_join(tid_r2, NULL);
    pthread_join(tid_r3, NULL);
    pthread_join(tid_w1, NULL);
    pthread_join(tid_w2, NULL);

    // 销毁信号量
    sem_destroy(&rw_mutex);
    sem_destroy(&mutex);
    return 0;
}
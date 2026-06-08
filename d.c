#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUF_SIZE 5
int buf[BUF_SIZE];
int in = 0, out = 0;

sem_t empty, full;  // empty空缓冲区，full满缓冲区
pthread_mutex_t mutex;

// 生产者线程
void *producer(void *arg) {
    for(int i=1; i<=10; i++){
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        buf[in] = i;
        printf("生产者：放入数据 %d 位置%d\n", i, in);
        in = (in+1) % BUF_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        sleep(1);
    }
    return NULL;
}

// 消费者线程
void *consumer(void *arg) {
    for(int i=1; i<=10; i++){
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        int data = buf[out];
        printf("消费者：取出数据 %d 位置%d\n", data, out);
        out = (out+1) % BUF_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        sleep(2);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    sem_init(&empty, 0, BUF_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&tid1, NULL, producer, NULL);
    pthread_create(&tid2, NULL, consumer, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    return 0;
}
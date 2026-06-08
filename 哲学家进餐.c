#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define N 5 // 5个哲学家，5根筷子

pthread_mutex_t chopsticks[N]; // 每根筷子是一个互斥锁

// 哲学家线程
void *philosopher(void *num) {
    int id = *(int *)num;
    free(num);

    while (1) {
        // ========== 思考 ==========
        printf("哲学家 %d 正在思考...\n", id);
        sleep(1);

        // ========== 饥饿，拿筷子（无死锁策略） ==========
        int left = id;
        int right = (id + 1) % N;

        // 关键：总是先拿编号小的筷子，破坏循环等待条件
        if (left > right) {
            int temp = left;
            left = right;
            right = temp;
        }

        // 拿左筷子
        pthread_mutex_lock(&chopsticks[left]);
        // 拿右筷子
        pthread_mutex_lock(&chopsticks[right]);

        // ========== 吃饭 ==========
        printf("哲学家 %d 开始吃饭\n", id);
        sleep(2);
        printf("哲学家 %d 吃完饭，放下筷子\n", id);

        // 放筷子
        pthread_mutex_unlock(&chopsticks[right]);
        pthread_mutex_unlock(&chopsticks[left]);
    }
}

int main() {
    pthread_t tid[N];
    int i;

    // 初始化所有筷子
    for (i = 0; i < N; i++)
        pthread_mutex_init(&chopsticks[i], NULL);

    // 创建哲学家线程
    for (i = 0; i < N; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&tid[i], NULL, philosopher, id);
    }

    // 等待线程
    for (i = 0; i < N; i++)
        pthread_join(tid[i], NULL);

    // 销毁锁
    for (i = 0; i < N; i++)
        pthread_mutex_destroy(&chopsticks[i]);

    return 0;
}
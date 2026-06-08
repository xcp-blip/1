#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROC 20  // 最大进程数

// 进程结构体
typedef struct {
    char name[10];
    int arrive;     // 到达时间
    int service;    // 服务时间
    int priority;   // 优先级
    int start;      // 开始执行时间
    int finish;     // 完成时间
    int turnover;   // 周转时间
    int remain;     // 剩余时间（RR算法用）
} Process;

Process proc[MAX_PROC];
int n;  // 进程总数



// 输入进程信息
void input_proc() {
    printf("请输入进程个数：");
    scanf("%d", &n);
    for(int i = 0; i < n; i++){
        printf("请输入第%d个进程 名称 到达时间 服务时间 优先级：\n", i+1);
        scanf("%s %d %d %d", proc[i].name, &proc[i].arrive, &proc[i].service, &proc[i].priority);
        proc[i].remain = proc[i].service;
    }
}

// 输出结果表格
void print_result(Process p[], int cnt) {
    printf("进程名\t到达\t服务\t开始\t完成\t周转\n");
    float avg = 0;
    for(int i = 0; i < cnt; i++){
        p[i].turnover = p[i].finish - p[i].arrive;
        avg += p[i].turnover;
        printf("%s\t%d\t%d\t%d\t%d\t%d\n",p[i].name,p[i].arrive,p[i].service,p[i].start,p[i].finish,p[i].turnover);
    }
    printf("平均周转时间：%.2f\n\n", avg/cnt);
}


// 按到达时间升序排序
void sort_arrive(Process p[], int cnt) {
    for(int i=0; i<cnt-1; i++)
        for(int j=0; j<cnt-1-i; j++)
            if(p[j].arrive > p[j+1].arrive){
                Process t = p[j]; p[j] = p[j+1]; p[j+1] = t;
            }
}

void FCFS() {
    Process p[MAX_PROC];
    memcpy(p, proc, sizeof(proc));
    sort_arrive(p, n);
    int time = 0;
    for(int i=0; i<n; i++){
        if(time < p[i].arrive) time = p[i].arrive;
        p[i].start = time;
        time += p[i].service;
        p[i].finish = time;
    }
    printf("===== 先来先服务 FCFS =====\n");
    print_result(p, n);
}


void SJF() {
    Process p[MAX_PROC];
    memcpy(p, proc, sizeof(proc));
    int time = 0, finish_cnt = 0;
    int flag[MAX_PROC] = {0}; // 标记是否完成

    while(finish_cnt < n){
        int idx = -1;
        int min_service = 9999;
        // 遍历所有已到达且未完成的进程
        for(int i=0; i<n; i++){
            if(!flag[i] && p[i].arrive <= time && p[i].service < min_service){
                min_service = p[i].service;
                idx = i;
            }
        }
        if(idx == -1) { time++; continue; } // 无进程就绪，时间+1

        p[idx].start = time;
        time += p[idx].service;
        p[idx].finish = time;
        flag[idx] = 1;
        finish_cnt++;
    }
    printf("===== 短作业优先 SJF =====\n");
    print_result(p, n);
}

void RR(int slice) {
    Process p[MAX_PROC];
    memcpy(p, proc, sizeof(proc));
    int time = 0, finish_cnt = 0;
    int flag[MAX_PROC] = {0};

    while(finish_cnt < n){
        int has_run = 0;
        for(int i=0; i<n; i++){
            if(!flag[i] && p[i].arrive <= time){
                has_run = 1;
                if(p[i].remain == p[i].service) p[i].start = time;
                // 执行一个时间片
                if(p[i].remain > slice){
                    time += slice;
                    p[i].remain -= slice;
                } else {
                    time += p[i].remain;
                    p[i].remain = 0;
                    p[i].finish = time;
                    flag[i] = 1;
                    finish_cnt++;
                }
            }
        }
        if(!has_run) time++;
    }
    printf("===== 时间片轮转 RR(时间片=%d) =====\n", slice);
    print_result(p, n);
}


void Priority() {
    Process p[MAX_PROC];
    memcpy(p, proc, sizeof(proc));
    int time = 0, finish_cnt = 0;
    int flag[MAX_PROC] = {0};

    while(finish_cnt < n){
        int idx = -1;
        int min_prio = 9999;
        for(int i=0; i<n; i++){
            if(!flag[i] && p[i].arrive <= time && p[i].priority < min_prio){
                min_prio = p[i].priority;
                idx = i;
            }
        }
        if(idx == -1) { time++; continue; }

        p[idx].start = time;
        time += p[idx].service;
        p[idx].finish = time;
        flag[idx] = 1;
        finish_cnt++;
    }
    printf("===== 优先级调度 =====\n");
    print_result(p, n);
}


int main() {
    input_proc();
    FCFS();
    SJF();
    RR(2);  // 时间片设为2
    Priority();
    return 0;
}










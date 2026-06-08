#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROC 20
#define TIME_SLICE 2    // 基础时间片

// 进程结构体（扩展带权周转、就绪标记）
typedef struct {
    char name[10];
    int arrive;     // 到达时间
    int service;    // 总服务时间
    int priority;   // 优先级
    int remain;     // 剩余服务时间
    int start;      // 开始执行时间
    int finish;     // 完成时间
    int turnover;   // 周转时间
    float w_turn;   // 带权周转时间
    int done;       // 0未完成 1已完成
} Process;

Process proc[MAX_PROC];
int n;  // 进程总数

// 动态输入进程信息
void input_proc() {
    printf("===== 输入进程信息 =====\n");
    printf("进程名 到达时间 服务时间 优先级\n");
    scanf("%d", &n);
    for(int i = 0; i < n; i++){
        scanf("%s %d %d %d", proc[i].name, &proc[i].arrive, &proc[i].service, &proc[i].priority);
        proc[i].remain = proc[i].service;
        proc[i].done = 0;
        proc[i].start = -1;
        proc[i].finish = 0;
    }
}

// 计算周转、带权周转
void calc_stat(Process p[], int cnt) {
    for(int i = 0; i < cnt; i++){
        p[i].turnover = p[i].finish - p[i].arrive;
        p[i].w_turn = (float)p[i].turnover / p[i].service;
    }
}

// 单算法结果输出
void print_single(Process p[], int cnt, char *alg_name) {
    printf("\n========== %s ==========\n", alg_name);
    printf("进程\t到达\t服务\t完成\t周转\t带权周转\n");
    float avg_turn = 0, avg_wturn = 0;
    for(int i = 0; i < cnt; i++){
        avg_turn += p[i].turnover;
        avg_wturn += p[i].w_turn;
        printf("%s\t%d\t%d\t%d\t%d\t%.2f\n",
               p[i].name, p[i].arrive, p[i].service,
               p[i].finish, p[i].turnover, p[i].w_turn);
    }
    printf("平均周转时间: %.2f\n", avg_turn / cnt);
    printf("平均带权周转时间: %.2f\n", avg_wturn / cnt);
}

// 数组拷贝（每次算法独立运算，互不干扰）
void copy_proc(Process dest[], Process src[], int cnt) {
    memcpy(dest, src, sizeof(Process)*cnt);
}

// 按到达时间排序
void sort_arrive(Process p[], int cnt) {
    for(int i=0; i<cnt-1; i++)
        for(int j=0; j<cnt-1-i; j++)
            if(p[j].arrive > p[j+1].arrive){
                Process t = p[j];
                p[j] = p[j+1];
                p[j+1] = t;
            }
}

void FCFS() {
    Process p[MAX_PROC];
    copy_proc(p, proc, n);
    sort_arrive(p, n);
    int time = 0;
    for(int i=0; i<n; i++){
        if(time < p[i].arrive) time = p[i].arrive;
        p[i].start = time;
        time += p[i].service;
        p[i].finish = time;
        p[i].done = 1;
    }
    calc_stat(p, n);
    print_single(p, n, "FCFS 先来先服务"); 
}


void SJF() {
    Process p[MAX_PROC];
    copy_proc(p, proc, n);
    int time = 0, finish_cnt = 0;

    while(finish_cnt < n){
        int idx = -1;
        int min_s = 9999;
        // 选择已到达、未完成、服务时间最短
        for(int i=0; i<n; i++){
            if(!p[i].done && p[i].arrive <= time && p[i].service < min_s){
                min_s = p[i].service;
                idx = i;
            }
        }
        if(idx == -1) { time++; continue; }

        if(p[idx].start == -1) p[idx].start = time;
        time += p[idx].service;
        p[idx].finish = time;
        p[idx].done = 1;
        finish_cnt++;
    }
    calc_stat(p, n);
    print_single(p, n, "非抢占SJF 短作业优先");
}

void RR(int slice) {
    Process p[MAX_PROC];
    copy_proc(p, proc, n);
    int time = 0, finish_cnt = 0;

    while(finish_cnt < n){
        int has_run = 0;
        for(int i=0; i<n; i++){
            if(!p[i].done && p[i].arrive <= time){
                has_run = 1;
                if(p[i].start == -1) p[i].start = time;
                if(p[i].remain > slice){
                    time += slice;
                    p[i].remain -= slice;
                } else {
                    time += p[i].remain;
                    p[i].remain = 0;
                    p[i].finish = time;
                    p[i].done = 1;
                    finish_cnt++;
                }
            }
        }
        if(!has_run) time++;
    }
    calc_stat(p, n);
    char name[30];
    sprintf(name, "RR 时间片轮转(片长=%d)", slice);
    print_single(p, n, name);
}

// 抢占式短作业优先 SRTF（最短剩余时间）
void SRTF() {
    Process p[MAX_PROC];
    copy_proc(p, proc, n);
    int time = 0, finish_cnt = 0;
    int cur = -1;  // 当前运行进程

    while(finish_cnt < n){
        // 步骤1：寻找当前就绪队列中剩余时间最小的进程
        int min_remain = 9999;
        int next = -1;
        for(int i=0; i<n; i++){
            if(!p[i].done && p[i].arrive <= time && p[i].remain < min_remain){
                min_remain = p[i].remain;
                next = i;
            }
        }
        if(next == -1){ time++; continue; }

        // 步骤2：进程切换（抢占）
        if(cur != next){
            cur = next;
            if(p[cur].start == -1) p[cur].start = time;
        }

        // 步骤3：运行1个时间单位
        p[cur].remain--;
        time++;

        // 步骤4：进程完成
        if(p[cur].remain == 0){
            p[cur].finish = time;
            p[cur].done = 1;
            finish_cnt++;
            cur = -1;
        }
    }
    calc_stat(p, n);
    print_single(p, n, "SRTF 抢占式短作业优先");
}

#define QUEUE_NUM 3   // 3级反馈队列
int queue_slice[QUEUE_NUM] = {1, 2, 4}; // 各级时间片

// 多级反馈队列 MFQ
void MFQ() {
    Process p[MAX_PROC];
    copy_proc(p, proc, n);
    int time = 0, finish_cnt = 0;
    // 标记进程当前所在队列
    int proc_queue[MAX_PROC] = {0};

    while(finish_cnt < n){
        int run = 0;
        // 从高优先级队列到低优先级依次遍历
        for(int q = 0; q < QUEUE_NUM; q++){
            // 遍历当前队列所有进程
            for(int i = 0; i < n; i++){
                if(!p[i].done && p[i].arrive <= time && proc_queue[i] == q){
                    run = 1;
                    int slice = queue_slice[q];
                    // 记录开始时间
                    if(p[i].start == -1) p[i].start = time;

                    // 执行一个时间片
                    if(p[i].remain > slice){
                        time += slice;
                        p[i].remain -= slice;
                        // 时间片用完，降级到下一级队列
                        proc_queue[i] = q + 1;
                    } else {
                        // 进程执行完毕
                        time += p[i].remain;
                        p[i].remain = 0;
                        p[i].finish = time;
                        p[i].done = 1;
                        finish_cnt++;
                    }
                    break; // 同一队列内轮转，单次只跑一个进程
                }
            }
            if(run) break; // 高优先级队列有进程，不再遍历低队列
        }
        if(!run) time++; // 无就绪进程，时间推进
    }
    calc_stat(p, n);
    print_single(p, n, "MFQ 多级反馈队列");
}


int main() {
    // 示例输入格式：进程数 + 进程信息
    // 测试用例（可自行修改）：
    // 4
    // P1 0 7 3
    // P2 2 4 1
    // P3 4 2 2
    // P4 5 3 4
    input_proc();

    // 基础算法
    FCFS();
    SJF();
    RR(TIME_SLICE);

    // 扩展优化算法（扩展方向核心）
    SRTF();
    MFQ();

    return 0;
}















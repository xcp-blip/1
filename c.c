#include <stdio.h>
#define MAX_PAGE 100
#define MAX_FRAME 10

// FIFO页面置换
void FIFO(int page[], int n, int frame_num) {
    int frame[MAX_FRAME] = {0};
    int cnt = 0, fault = 0;
    for(int i=0; i<n; i++){
        int exist = 0;
        // 检查是否已在内存
        for(int j=0; j<frame_num; j++)
            if(frame[j] == page[i]) { exist=1; break; }
        if(!exist){
            fault++;
            frame[cnt % frame_num] = page[i];
            cnt++;
        }
    }
    printf("===== FIFO 页面置换 =====\n");
    printf("缺页次数：%d  缺页率：%.2f%%\n\n", fault, fault*100.0/n);
}

// LRU 最近最少使用
void LRU(int page[], int n, int frame_num) {
    int frame[MAX_FRAME];
    int time[MAX_FRAME]; // 记录使用时间
    int fault = 0, clock = 0;
    for(int i=0; i<frame_num; i++) frame[i] = -1;

    for(int i=0; i<n; i++){
        int exist = 0;
        int pos = 0;
        for(pos=0; pos<frame_num; pos++){
            if(frame[pos] == page[i]){
                exist = 1;
                time[pos] = clock++;
                break;
            }
        }
        if(exist) continue;

        fault++;
        // 找最久未使用
        int min_t = time[0], min_pos = 0;
        for(int j=1; j<frame_num; j++){
            if(time[j] < min_t){
                min_t = time[j];
                min_pos = j;
            }
        }
        frame[min_pos] = page[i];
        time[min_pos] = clock++;
    }
    printf("===== LRU 页面置换 =====\n");
    printf("缺页次数：%d  缺页率：%.2f%%\n\n", fault, fault*100.0/n);
}

int main() {
    int page_seq[MAX_PAGE];
    int n, frame;
    printf("输入页面访问序列长度：");
    scanf("%d", &n);
    printf("输入页面序列：");
    for(int i=0; i<n; i++) scanf("%d", &page_seq[i]);
    printf("输入内存块数量：");
    scanf("%d", &frame);

    FIFO(page_seq, n, frame);
    LRU(page_seq, n, frame);
    return 0;
}
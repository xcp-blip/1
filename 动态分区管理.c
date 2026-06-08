#include <stdio.h>
#include <stdlib.h>

#define MAX_BLOCK 20
// 分区结构体
typedef struct {
    int start;   // 起始地址
    int size;    // 大小
    int used;    // 0空闲 1已分配
} Block;

Block block[MAX_BLOCK];
int block_cnt;  // 分区数量

// 初始化内存分区
void init_block() {
    printf("请输入初始分区个数：");
    scanf("%d", &block_cnt);
    for(int i=0; i<block_cnt; i++){
        printf("分区%d 起始地址 大小：\n",i+1);
        scanf("%d %d", &block[i].start, &block[i].size);
        block[i].used = 0;
    }
}

// 打印分区状态
void print_block() {
    printf("【内存分区状态】\n");
    for(int i=0; i<block_cnt; i++){
        char *stat = block[i].used ? "已分配" : "空闲";
        printf("分区%d：起始=%d 大小=%d 状态=%s\n",
               i, block[i].start, block[i].size, stat);
    }
    printf("\n");
}

// 首次适应 FF
void FF_alloc(int req_size) {
    printf("===== 首次适应 FF 分配(请求大小：%d) =====\n", req_size);
    int ok = 0;
    for(int i=0; i<block_cnt; i++){
        if(block[i].used == 0 && block[i].size >= req_size){
            // 分割分区
            int old_start = block[i].start;
            int old_size = block[i].size;
            block[i].used = 1;
            block[i].size = req_size;
            // 新增空闲分区
            block[block_cnt].start = old_start + req_size;
            block[block_cnt].size = old_size - req_size;
            block[block_cnt].used = 0;
            block_cnt++;
            ok = 1;
            break;
        }
    }
    if(!ok) printf("分配失败，无足够空闲分区\n");
    print_block();
}

// 最佳适应 BF
void BF_alloc(int req_size) {
    printf("===== 最佳适应 BF 分配(请求大小：%d) =====\n", req_size);
    int idx = -1;
    int min_diff = 9999;
    // 寻找最接近请求大小的空闲分区
    for(int i=0; i<block_cnt; i++){
        if(block[i].used == 0 && block[i].size >= req_size){
            int diff = block[i].size - req_size;
            if(diff < min_diff){
                min_diff = diff;
                idx = i;
            }
        }
    }
    if(idx == -1){
        printf("分配失败，无足够空闲分区\n\n");
        return;
    }
    // 分割分区
    int old_start = block[idx].start;
    int old_size = block[idx].size;
    block[idx].used = 1;
    block[idx].size = req_size;
    block[block_cnt].start = old_start + req_size;
    block[block_cnt].size = old_size - req_size;
    block[block_cnt].used = 0;
    block_cnt++;
    print_block();
}

// 内存回收（按起始地址回收）
void free_block(int addr) {
    printf("===== 回收起始地址为 %d 的分区 =====\n", addr);
    for(int i=0; i<block_cnt; i++){
        if(block[i].start == addr && block[i].used == 1){
            block[i].used = 0;
            break;
        }
    }
    print_block();
}

int main() {
    init_block();
    print_block();
    FF_alloc(50);
    BF_alloc(30);
    free_block(0); // 回收指定分区
    return 0;
}






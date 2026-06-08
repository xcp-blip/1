#include <stdio.h>
#include <string.h>

#define MAX_FILE 10
#define BLOCK_NUM 32  // 磁盘块总数

// 位示图 管理空闲块
int bitmap[BLOCK_NUM] = {0}; // 0空闲 1占用

// 文件控制块FCB
typedef struct {
    char name[20];
    int start_block; // 起始块
    int size;        // 文件大小
    int exist;       // 0不存在 1存在
} FCB;

FCB file[MAX_FILE];

// 初始化
void init_fs() {
    memset(file, 0, sizeof(file));
    memset(bitmap, 0, sizeof(bitmap));
    printf("简易文件系统初始化完成\n");
}

// 分配空闲磁盘块
int alloc_block() {
    for(int i=0; i<BLOCK_NUM; i++){
        if(bitmap[i] == 0){
            bitmap[i] = 1;
            return i;
        }
    }
    return -1; // 无空闲块
}

// 创建文件
void create_file(char *name, int size) {
    int idx = -1;
    // 查找空FCB
    for(int i=0; i<MAX_FILE; i++){
        if(file[i].exist == 0) { idx = i; break; }
    }
    if(idx == -1) { printf("文件表已满\n"); return; }

    int block = alloc_block();
    if(block == -1) { printf("磁盘空间不足\n"); return; }

    strcpy(file[idx].name, name);
    file[idx].start_block = block;
    file[idx].size = size;
    file[idx].exist = 1;
    printf("文件 %s 创建成功，起始块：%d\n", name, block);
}

// 删除文件
void del_file(char *name) {
    for(int i=0; i<MAX_FILE; i++){
        if(file[i].exist && strcmp(file[i].name, name)==0){
            bitmap[file[i].start_block] = 0; // 回收块
            file[i].exist = 0;
            printf("文件 %s 删除成功\n", name);
            return;
        }
    }
    printf("文件不存在\n");
}

// 查看文件列表
void list_file() {
    printf("===== 文件列表 =====\n");
    for(int i=0; i<MAX_FILE; i++){
        if(file[i].exist)
            printf("文件名：%s 起始块：%d 大小：%d\n",
                   file[i].name, file[i].start_block, file[i].size);
    }
}

int main() {
    init_fs();
    create_file("test1.txt", 1024);
    create_file("data.bin", 2048);
    list_file();
    del_file("test1.txt");
    list_file();
    return 0;
}
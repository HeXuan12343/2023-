#include <iostream>
#include <time.h>
#include <iomanip>
using namespace std;

const int instruct_num = 320;//指令数量
int instruct_arr[instruct_num + 6];//指令集合
struct paper {//页面结构体，用来组成页面队列
    int instruct = 0;
    int time = 0;
    paper* next = NULL;
};
int rand(int min, int max)//产生指定范围[min,max]的随机数。
{
    return rand() % (max - min + 1) + min;
}
void make_instruct() {//产生指令序列
    int n = 0;
    while (n < instruct_num) {
        int M = rand(1, 30);
        instruct_arr[n++] = M;
        instruct_arr[n++] = M + 1;
        int M1 = rand(0, M - 1);
        instruct_arr[n++] = M1;
        instruct_arr[n++] = M1 + 1;
        int M2 = rand(M1 + 2, 30);
        instruct_arr[n++] = M2;
        instruct_arr[n++] = M2 + 1;
    }
}
double FIFO(int block_size) {
    int failure_times = 0;
    paper* Head = new paper;
    paper* Tail = Head;
    for (int i = 0; i < instruct_num; i++) {
        int cur_instruct = instruct_arr[i];
        bool hit = false;//是否命中

        for (paper* p = Head->next; p != NULL; p = p->next)
            if (p->instruct == cur_instruct)
                hit = true;

        if (!hit) {
            failure_times++;
            if (Head->instruct >= block_size)
                Head->next = Head->next->next;
            else
                Head->instruct++;

            Tail->next = new paper;
            Tail = Tail->next;
            Tail->instruct = cur_instruct;
            Tail->next = NULL;
        }
    }

    return 1.0 - (double)failure_times / instruct_num;
}
double LRU(int block_size) {
    int failure_times = 0;
    paper* Head = new paper;
    paper* Tail = Head;
    int clock = 999;

    for (int i = 0; i < instruct_num; i++) {
        clock--;
        int cur_instruct = instruct_arr[i];
        bool hit = false;
        for (paper* p = Head->next; p != NULL; p = p->next)
            if (p->instruct == cur_instruct) {
                hit = true;
                p->time = clock;//刷新时钟
            }

        if (!hit) {
            failure_times++;
            if (Head->instruct >= block_size) { //内存已满
                paper* t = new paper; t->time = -1;
                for (paper* p = Head->next; p != NULL; p = p->next)//遍历找到最久未使用的
                    if (p->time > t->time)
                        t = p;

                t->instruct = cur_instruct;
                t->time = clock;

            }
            else {
                Head->instruct++;
                Tail->next = new paper;
                Tail = Tail->next;
                Tail->instruct = cur_instruct;
                Tail->time = clock;
                Tail->next = NULL;
            }
        }
    }
    return 1.0 - (double)failure_times / instruct_num;
}
double Optimal(int block_size) {
    int failure_times = 0;
    paper* Head = new paper;
    paper* Tail = Head;
    for (int i = 0; i < instruct_num; i++) {
        int cur_instruct = instruct_arr[i];
        bool hit = false;

        for (paper* p = Head->next; p != NULL; p = p->next)
            if (p->instruct == cur_instruct)
                hit = true;

        if (!hit) {
            failure_times++;
            if (Head->instruct >= block_size) { //内存已满
                //找到每个请求还有多久会被用到
                for (paper* p = Head->next; p != NULL; p = p->next) {
                    p->time = 999;
                    for (int j = i + 1; j < instruct_num; j++)
                        if (p->instruct == instruct_arr[i]) {
                            p->time = j - i;
                            break;
                        }
                }
                //找到最久不被用到的淘汰
                paper* t = new paper; t->time = -1;
                for (paper* p = Head->next; p != NULL; p = p->next)//遍历找到最久未使用的
                    if (p->time > t->time)
                        t = p;

                t->instruct = cur_instruct;

            }
            else {
                Head->instruct++;
                Tail->next = new paper;
                Tail = Tail->next;
                Tail->instruct = cur_instruct;
                Tail->next = NULL;
            }
        }
    }
    return 1.0 - (double)failure_times / instruct_num;;
}
int main()
{
    srand((unsigned int)time(NULL));
    make_instruct();

    cout << "指令序列为：" << endl;
    int i = 0;
    while (i++ < instruct_num) {
        cout << instruct_arr[i] << "\t";
        if (i % 20 == 0)cout << endl;
    }cout << endl << endl;
    cout << "命中率如下：" << endl;
    cout << "SIZE\t\t" << "FIFO\t\t" << "LRU\t\t" << "Optimal" << endl;
    for (int disk_block_size = 8; disk_block_size <= 32; disk_block_size++) {

        cout << disk_block_size << "\t";
        cout << setw(8) << FIFO(disk_block_size) << "\t";
        cout << setw(8) << LRU(disk_block_size) << "\t";
        cout << setw(8) << Optimal(disk_block_size) << "\t";
        cout << endl;

    }
}

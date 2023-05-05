#include <iostream>
#include <fstream>
using namespace std;

enum Placement {//三种策略定义
    FIRST,//首次
    BEST,//最佳
    WOREST,//最坏
    EXIT//DEFALT
};
istream& operator >> (istream& is, Placement& choice)//重载输入运算符，方便用户选择
{
    int c = 0;
    cin >> c;
    switch (c)
    {
    case 0:
        choice = Placement::FIRST;
        break;
    case 1:
        choice = Placement::BEST;
        break;
    case 2:
        choice = Placement::WOREST;
        break;
    case 3:
        choice = Placement::EXIT;
    default:
        break;
    }
    return is;
}

struct rd {//分区描述器
    int flag;  //分配标志，空闲为0
    int size;   //分区大小
    rd* next;   //指向下一个空闲分区，对已分配区：此项为零。
    rd() { flag = -1; size = -1; next = NULL; }//构造函数
    rd(int _flag , int _size , rd *_next) : flag(_flag) , size(_size) , next(_next){}//带参构造
};

rd* Free , * Used;//分别用来存空闲队列和已占用队列

const int maxNum = 512;//需要申请的块数
const int maxsize = 512 * sizeof(rd);//实际需要申请的大小
rd* block;//申请到的资源

rd* Request(int need, Placement choice) {//申请地址的函数，返回申请到的地址
    int temp;
    rd* addr = NULL;//返回的地址值
    rd* p = NULL, * prep = new rd, * preAddr = new rd;

    if (choice == Placement::FIRST) {//首次算法:按物理地址的顺序排列，优先地址最小的
        temp = INT_MAX;
        for (p = Free->next, prep->next = p; p != NULL; p = p->next, prep = prep->next)
            if ((int)p < temp && p->size > need)
                temp = (int)p, addr = p, preAddr = prep;
        if (addr == NULL)
            return NULL;
    }
    else if (choice == Placement::BEST) {//最佳算法按:照空闲容量递增排列，优先最小的
        temp = maxNum;
        for (p = Free->next, prep->next = p; p != NULL; p = p->next, prep = prep->next)
            if (p->size < temp && p->size > need)
                temp = p->size, addr = p, preAddr = prep;
        if (addr == NULL)
            return NULL;
    }
    else  if (choice == Placement::WOREST) {//最坏算法：按照空闲容量递减排列，优先最大的
        temp = 0;
        for (p = Free->next, prep->next = p; p != NULL; p = p->next, prep = prep->next)
            if (p->size > temp)
                temp = p->size, addr = p, preAddr = prep;
        if (temp < need)
            return NULL;
    }

    //修改free队列
    if (addr->size == need)
        preAddr->next = addr->next;//删除表项
    else {
        addr->size -= need;
        //Ans =(Ans+Ans->size * sizeof(rd));//后一半划分出去

 //       cout<<"当前分配地址距离开始" << Ans - MainHead << endl;
        addr += addr->size;
    }

    addr->flag = 1;
    addr->size = need;

    //修改used队列
    if (Used->next == NULL) {
        Used->next = addr;
        addr->next = NULL;
    }
    else {
        addr->next = Used->next;
        Used->next = addr;
    }

    return addr;
}
int Release(rd* r) {//释放所给出的分区的地址，返回1表示正常释放
    if (r->flag == 0) {
        cout << "该空间未使用，无法释放" << endl;
        return 0;
    }

    rd* p = NULL;
    rd* f1 = NULL, * f2 = NULL;

    //先判断used
    for (p = Used->next; p != NULL; p = p->next) {
        if (p->next == r)
            f1 = p;
        if (r->next == p)
            f2 = p;
    }
    //再判断free
    for (p = Free->next; p != NULL; p = p->next) {
        if (p + p->size == r)
            f1 = p;
        if (r + r->size == p)
            f2 = p;
    }


    //修改used队列
    for (p = Used->next; p != NULL; p = p->next)
        if (p->next == r) {
            p->next = r->next;
            break;
        }

    bool f1_is_free = 1, f2_is_free = 1;
    if (f1->flag)
        f1_is_free = 0;
    if (f2 == NULL || f2->flag)
        f2_is_free = 0;
    //修改free队列
    if (f1_is_free && !f2_is_free) {     // A)f1空闲，f2已分配；     //f和f1合并，修改表项f1，数量不变
        f1->next = r->next;
        f1->size += r->size;
    }
    else if (!f1_is_free && f2_is_free) { // B)f1已分配，f2空闲；     //f和f2合并，修改表项f2，数量不变
        r->next = f2->next;
        r->flag = 0;
        r->size += f2->size;
        for (p = Free; p != NULL; p = p->next)//找到f2的前驱
            if (p->next == f2) {
                p->next = r;//后继修改为r
                break;
            }
    }
    else if (f1_is_free && f2_is_free) {  // C)f1空闲，f2空闲；       //f1 f f2合并，修改表项f1，删除f2
        for (p = Free; p != NULL; p = p->next)//找到f1的前驱
            if (p->next == f1)
                break;

        if (p != f2->next)//防止构成环路
            f1->next = f2->next;
        f1->size += (f2->size + r->size);
        for (p = Free; p != NULL; p = p->next)//找到f2的前驱
            if (p->next == f2) {
                p->next = f2->next;//后继修改为r
                break;
            }
    }
    else if (!f1_is_free && !f2_is_free) {// D)f1已分配，f2已分配；   //增加空闲表项f
        r->flag = 0;
        if (Free->next == NULL) {
            Free->next = r;
            r->next = NULL;
        }
        else {
            r->next = Free->next;
            Free->next = r;
        }
    }
    return 1;
}
void show_block() {//展示内存的使用情况
    rd* p = NULL;
    rd* Cur = block;

    //先判断used
    cout << "Used:--";
    for (p = Used->next; p != NULL; p = p->next) {
        cout << p - block << "_" << p->size << "--->";
    }
    cout << endl;
    //再判断free
    cout << "Free:--";
    for (p = Free->next; p != NULL; p = p->next) {
        cout << p - block << "_" << p->size << "--->";
    }
    cout << endl;

    do {//直到当前是最后一块为止

        for (p = Free->next; p != NULL; p = p->next)
            if (Cur == p) {
                break;
            }
        for (p = Used->next; p != NULL; p = p->next)
            if (Cur == p) {
                break;
            }

        //输出可视化
        string u = "[u]";
        string f = "[f]";

        if (Cur->flag == 0) {//为free
            //  cout << Cur->size;
            cout << Cur - block;
            for (int i = 0; i < (Cur->size / 20); i++)
                cout << f;

        }
        if (Cur->flag != 0) {//为free
            cout << Cur - block;
            for (int i = 0; i < (Cur->size / 20); i++)
                cout << u;
        }
        Cur += Cur->size;
    } while ((Cur - block) < maxNum - 1);

    cout << endl << endl << endl << endl;

}

int main()
{
    while (1) {
        cout << "================选择菜单选项================" << endl;
        cout << "| 首次：0     最佳：1    最坏：2    退出：3|" << endl;
        cout << "============================================" << endl;
        Placement method;
        cin >> method;
        if (method == Placement::EXIT) {
            system("cls");
            cout << "程序退出，感谢使用！！！" << endl;
            return 0;
        }
        if (method < 0 || method > 3) {
            cout << "您的输入有误，请重新输入:" << endl;
            continue;
        }


        block = new rd[maxsize];//申请到的资源
        Free = new rd, Used = new rd;//分别用来存空闲队列和已占用队列

        block->next = NULL;
        rd* fir = block; fir->next = NULL; fir->flag = 0;  fir->size = maxNum - 1;
        Free->next = fir;

        char operat;
        int id, need;
        fstream In;
        In.open("test.txt");
        if (!In.is_open())
            cout << "文件打开失败" << endl;
        cout << "初始状态：" << endl; show_block();//展示初始状态
        while (!In.eof()) {
            In >> operat;

            if (operat == '+') {
                In >> id >> need;
                rd* req = Request(need + 1, method);//req为申请到的地址
                if (req == NULL) {
                    cout << "     失败   " << id << "     " << need << " " << endl;
                    break;
                }
                cout << "     成功   " << id << "     " << need << " " << endl;
                req->flag = id;
                req->size = need + 1;
            }
            else
                if (operat == '-') {
                    In >> id;
                    rd* rel = Used->next;
                    for (; rel != NULL; rel = rel->next)
                        if (rel->flag == id)
                            break;
                    int rel_ans = 1;
                    int rel_size = rel->size;
                    if (rel == Used->next) {
                        cout << "进程" << id << "没有正在使用的空间" << endl;
                        break;
                    }
                    else
                        rel_ans = Release(rel);

                    if (rel_ans == 0) {
                        cout << "进程   " << id << "无法释放" << endl;
                    }
                    else
                        cout << "     成功   " << id << "     " << rel_size << "  " << endl;


                }
                else {
                    cout << operat << "读取失败" << endl;
                    break;
                }
            show_block();//展示每次修改后的结果
        }
        free(block);
    }
    return 0;
}


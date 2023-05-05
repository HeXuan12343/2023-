#include <iostream>
#include <fstream>
using namespace std;

enum Placement {//���ֲ��Զ���
    FIRST,//�״�
    BEST,//���
    WOREST,//�
    EXIT//DEFALT
};
istream& operator >> (istream& is, Placement& choice)//��������������������û�ѡ��
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

struct rd {//����������
    int flag;  //�����־������Ϊ0
    int size;   //������С
    rd* next;   //ָ����һ�����з��������ѷ�����������Ϊ�㡣
    rd() { flag = -1; size = -1; next = NULL; }//���캯��
    rd(int _flag , int _size , rd *_next) : flag(_flag) , size(_size) , next(_next){}//���ι���
};

rd* Free , * Used;//�ֱ���������ж��к���ռ�ö���

const int maxNum = 512;//��Ҫ����Ŀ���
const int maxsize = 512 * sizeof(rd);//ʵ����Ҫ����Ĵ�С
rd* block;//���뵽����Դ

rd* Request(int need, Placement choice) {//�����ַ�ĺ������������뵽�ĵ�ַ
    int temp;
    rd* addr = NULL;//���صĵ�ֵַ
    rd* p = NULL, * prep = new rd, * preAddr = new rd;

    if (choice == Placement::FIRST) {//�״��㷨:�������ַ��˳�����У����ȵ�ַ��С��
        temp = INT_MAX;
        for (p = Free->next, prep->next = p; p != NULL; p = p->next, prep = prep->next)
            if ((int)p < temp && p->size > need)
                temp = (int)p, addr = p, preAddr = prep;
        if (addr == NULL)
            return NULL;
    }
    else if (choice == Placement::BEST) {//����㷨��:�տ��������������У�������С��
        temp = maxNum;
        for (p = Free->next, prep->next = p; p != NULL; p = p->next, prep = prep->next)
            if (p->size < temp && p->size > need)
                temp = p->size, addr = p, preAddr = prep;
        if (addr == NULL)
            return NULL;
    }
    else  if (choice == Placement::WOREST) {//��㷨�����տ��������ݼ����У���������
        temp = 0;
        for (p = Free->next, prep->next = p; p != NULL; p = p->next, prep = prep->next)
            if (p->size > temp)
                temp = p->size, addr = p, preAddr = prep;
        if (temp < need)
            return NULL;
    }

    //�޸�free����
    if (addr->size == need)
        preAddr->next = addr->next;//ɾ������
    else {
        addr->size -= need;
        //Ans =(Ans+Ans->size * sizeof(rd));//��һ�뻮�ֳ�ȥ

 //       cout<<"��ǰ�����ַ���뿪ʼ" << Ans - MainHead << endl;
        addr += addr->size;
    }

    addr->flag = 1;
    addr->size = need;

    //�޸�used����
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
int Release(rd* r) {//�ͷ��������ķ����ĵ�ַ������1��ʾ�����ͷ�
    if (r->flag == 0) {
        cout << "�ÿռ�δʹ�ã��޷��ͷ�" << endl;
        return 0;
    }

    rd* p = NULL;
    rd* f1 = NULL, * f2 = NULL;

    //���ж�used
    for (p = Used->next; p != NULL; p = p->next) {
        if (p->next == r)
            f1 = p;
        if (r->next == p)
            f2 = p;
    }
    //���ж�free
    for (p = Free->next; p != NULL; p = p->next) {
        if (p + p->size == r)
            f1 = p;
        if (r + r->size == p)
            f2 = p;
    }


    //�޸�used����
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
    //�޸�free����
    if (f1_is_free && !f2_is_free) {     // A)f1���У�f2�ѷ��䣻     //f��f1�ϲ����޸ı���f1����������
        f1->next = r->next;
        f1->size += r->size;
    }
    else if (!f1_is_free && f2_is_free) { // B)f1�ѷ��䣬f2���У�     //f��f2�ϲ����޸ı���f2����������
        r->next = f2->next;
        r->flag = 0;
        r->size += f2->size;
        for (p = Free; p != NULL; p = p->next)//�ҵ�f2��ǰ��
            if (p->next == f2) {
                p->next = r;//����޸�Ϊr
                break;
            }
    }
    else if (f1_is_free && f2_is_free) {  // C)f1���У�f2���У�       //f1 f f2�ϲ����޸ı���f1��ɾ��f2
        for (p = Free; p != NULL; p = p->next)//�ҵ�f1��ǰ��
            if (p->next == f1)
                break;

        if (p != f2->next)//��ֹ���ɻ�·
            f1->next = f2->next;
        f1->size += (f2->size + r->size);
        for (p = Free; p != NULL; p = p->next)//�ҵ�f2��ǰ��
            if (p->next == f2) {
                p->next = f2->next;//����޸�Ϊr
                break;
            }
    }
    else if (!f1_is_free && !f2_is_free) {// D)f1�ѷ��䣬f2�ѷ��䣻   //���ӿ��б���f
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
void show_block() {//չʾ�ڴ��ʹ�����
    rd* p = NULL;
    rd* Cur = block;

    //���ж�used
    cout << "Used:--";
    for (p = Used->next; p != NULL; p = p->next) {
        cout << p - block << "_" << p->size << "--->";
    }
    cout << endl;
    //���ж�free
    cout << "Free:--";
    for (p = Free->next; p != NULL; p = p->next) {
        cout << p - block << "_" << p->size << "--->";
    }
    cout << endl;

    do {//ֱ����ǰ�����һ��Ϊֹ

        for (p = Free->next; p != NULL; p = p->next)
            if (Cur == p) {
                break;
            }
        for (p = Used->next; p != NULL; p = p->next)
            if (Cur == p) {
                break;
            }

        //������ӻ�
        string u = "[u]";
        string f = "[f]";

        if (Cur->flag == 0) {//Ϊfree
            //  cout << Cur->size;
            cout << Cur - block;
            for (int i = 0; i < (Cur->size / 20); i++)
                cout << f;

        }
        if (Cur->flag != 0) {//Ϊfree
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
        cout << "================ѡ��˵�ѡ��================" << endl;
        cout << "| �״Σ�0     ��ѣ�1    ���2    �˳���3|" << endl;
        cout << "============================================" << endl;
        Placement method;
        cin >> method;
        if (method == Placement::EXIT) {
            system("cls");
            cout << "�����˳�����лʹ�ã�����" << endl;
            return 0;
        }
        if (method < 0 || method > 3) {
            cout << "����������������������:" << endl;
            continue;
        }


        block = new rd[maxsize];//���뵽����Դ
        Free = new rd, Used = new rd;//�ֱ���������ж��к���ռ�ö���

        block->next = NULL;
        rd* fir = block; fir->next = NULL; fir->flag = 0;  fir->size = maxNum - 1;
        Free->next = fir;

        char operat;
        int id, need;
        fstream In;
        In.open("test.txt");
        if (!In.is_open())
            cout << "�ļ���ʧ��" << endl;
        cout << "��ʼ״̬��" << endl; show_block();//չʾ��ʼ״̬
        while (!In.eof()) {
            In >> operat;

            if (operat == '+') {
                In >> id >> need;
                rd* req = Request(need + 1, method);//reqΪ���뵽�ĵ�ַ
                if (req == NULL) {
                    cout << "     ʧ��   " << id << "     " << need << " " << endl;
                    break;
                }
                cout << "     �ɹ�   " << id << "     " << need << " " << endl;
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
                        cout << "����" << id << "û������ʹ�õĿռ�" << endl;
                        break;
                    }
                    else
                        rel_ans = Release(rel);

                    if (rel_ans == 0) {
                        cout << "����   " << id << "�޷��ͷ�" << endl;
                    }
                    else
                        cout << "     �ɹ�   " << id << "     " << rel_size << "  " << endl;


                }
                else {
                    cout << operat << "��ȡʧ��" << endl;
                    break;
                }
            show_block();//չʾÿ���޸ĺ�Ľ��
        }
        free(block);
    }
    return 0;
}


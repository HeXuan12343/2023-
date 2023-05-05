#include <iostream>
#include <Windows.h>
#include <list>

struct PCB {
	std::string name = "";
	int need_time = 0;//������������ʱ��
	int start_time = 0;//���̿�ʼ���е�ʱ��
	int end_time = 0;//���̽������е�ʱ�� 
	int first_start_time = 0;//��һ�ο�ʼ���е�ʱ��
	int run_time = 0;//�Ѿ����е�ʱ��
	int wait_time = 0;//�ѵȴ�ʱ��
	int count = 0;//���д���
};

const int Finish_time = 7;
int Clock = 0;
int turn_time_sum = 0;
std::string p_name[10] = { "P1" , "P2" , "P3" , "P4" , "P5" , "P6" , "P7" , "P8" , "P9" , "P10" };
int p_run_time[10] = { 16,11,	14,13,15,21,18,10,7,14 };
int p_wait_time[10] = { 6,5,4,3,2,1,2,3,4,5 };

std::list<PCB> RQ1, RQ2;

void initRQ1()
{
	std::cout << "��ʼ��RQ1" << std::endl;
	for (int i = 0; i < 5; i++) {
		PCB p;
		p.name = p_name[i];
		p.need_time = p_run_time[i];
		p.wait_time = p_wait_time[i];
		p.start_time = 0;
		p.run_time = 0;
		p.end_time = 0;
		p.first_start_time = 0;
		p.count = 0;
		RQ1.push_back(p);
	}
	std::cout << "����������......"<<std::endl;
	Sleep(1000);
	std::cout << "�������̳ɹ���" << std::endl;
	std::cout << std::endl;
	std::cout << "��������"<<"\t\t"<<"��������ʱ��" << std::endl;
	std::list<PCB>::iterator it = RQ1.begin();
	while (it != RQ1.end()) {
		std::cout<< (*it).name << "\t\t\t" << (*it).need_time << std::endl;
		it++;
	}
}

void initRQ2()
{
	std::cout << "��ʼ��RQ2" << std::endl;
	for (int i = 5; i < 10; i++) {
		PCB p;
		p.name = p_name[i];
		p.need_time = p_run_time[i];
		p.wait_time = p_wait_time[i];
		p.start_time = 0;
		p.run_time = 0;
		p.end_time = 0;
		p.first_start_time = 0;
		p.count = 0;
		RQ2.push_back(p);
	}
	std::cout << "����������......" << std::endl;
	Sleep(1000);
	std::cout << "�������̳ɹ���" << std::endl;
	std::cout << std::endl;
	std::cout << "��������" << "\t\t" << "��������ʱ��" << std::endl;
	std::list<PCB>::iterator it = RQ2.begin();
	while (it != RQ2.end()) {
		std::cout << (*it).name << "\t\t\t" << (*it).need_time << std::endl;
		it++;
	}
}

void Rotation()
{
	initRQ1();
	Clock = 0;
	std::cout << std::endl;
	std::cout<<"����\t"<<"��������ʱ��\t"<<"���̿�ʼ���е�ʱ��\t" << "ִ�д���\t" << "��ִ��ʱ��\t"\
		<< "����ʱ�䣺\t" << "��תʱ�䣺\n";
	while (!RQ1.empty()) {
		Sleep(500);
		PCB* p = &RQ1.front();
		p->start_time = Clock;
		std::cout << p->name << "\t\t" << p->need_time << "\t\t" << p->start_time << "\t\t";
		if (p->need_time > Finish_time) {
			Clock += Finish_time;
			p->need_time -= Finish_time;
			(p->count) ++;
			p->run_time += Finish_time;
			if (p->count == 1)
				p->first_start_time = p->start_time;
			std::cout << p->count << "\t\t" << p->run_time << "\t\t\t\t" << std::endl;
			RQ1.push_back(*p);
			RQ1.pop_front();
		}
		else {
			Clock += p->need_time;
			(p->count)++;
			p->run_time += p->need_time;
			p->need_time = 0;
			p->end_time = Clock;
			if (p->count == 1)
				p->first_start_time = p->start_time;
			turn_time_sum += (p->end_time - p->first_start_time);
			std::cout << p->count << "\t\t" << p->run_time << "\t\t" << p->end_time << "\t\t" << p->end_time - p->first_start_time << "\t\t"\
				<< "ִ�����";
			RQ1.pop_front();
		}
	}
}

void Short_precedence()
{
	initRQ2();
	std::cout << std::endl;
	Clock = 0;
	std::cout << "���̣�\t" << "ִ������ʱ�䣺\t" << "��ʼִ��ʱ�䣺\t" << "����ʱ�䣺\t" << "��תʱ��\n";
	while (!RQ2.empty()) {
		Sleep(500);
		std::list<PCB>::iterator p = RQ2.begin();
		for (std::list<PCB>::iterator it = RQ2.begin(); it != RQ2.end(); it++) {
			//�ҵ����Ԥ��ִ�е�ʱ��
			if (it->need_time < p->need_time)
				p = it;
		}
		p->start_time = Clock;
		p->end_time = p->start_time + p->need_time;
		Clock = p->end_time;
		turn_time_sum += (p->end_time - p->first_start_time);
		std::cout << p->name << "\t\t" << p->need_time << "\t\t" << p->start_time << "\t\t" \
			<< p->end_time << "\t\t" << p->end_time - p->start_time << "\tִ�����\n";
		RQ2.erase(p);
	}

}

int main()
{
	std::cout << "ִ��RQ1***********************************************" << std::endl;
	Rotation();
	std::cout << std::endl << std::endl << std::endl;
	std::cout << "ִ��RQ2***********************************************" << std::endl;
	Short_precedence();
	std::cout << std::endl;
	std::cout << "ȫ�����̵�ƽ����תʱ��Ϊ��" << turn_time_sum / 10 << std::endl;
	std::cin.get();
}
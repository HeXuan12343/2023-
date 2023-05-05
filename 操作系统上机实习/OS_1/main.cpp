#include <iostream>
#include <Windows.h>
#include <list>

struct PCB {
	std::string name = "";
	int need_time = 0;//进程运行所需时间
	int start_time = 0;//进程开始运行的时间
	int end_time = 0;//进程结束运行的时间 
	int first_start_time = 0;//第一次开始运行的时间
	int run_time = 0;//已经运行的时间
	int wait_time = 0;//已等待时间
	int count = 0;//运行次数
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
	std::cout << "初始化RQ1" << std::endl;
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
	std::cout << "创建进程中......"<<std::endl;
	Sleep(1000);
	std::cout << "创建进程成功！" << std::endl;
	std::cout << std::endl;
	std::cout << "进程名称"<<"\t\t"<<"所需运行时间" << std::endl;
	std::list<PCB>::iterator it = RQ1.begin();
	while (it != RQ1.end()) {
		std::cout<< (*it).name << "\t\t\t" << (*it).need_time << std::endl;
		it++;
	}
}

void initRQ2()
{
	std::cout << "初始化RQ2" << std::endl;
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
	std::cout << "创建进程中......" << std::endl;
	Sleep(1000);
	std::cout << "创建进程成功！" << std::endl;
	std::cout << std::endl;
	std::cout << "进程名称" << "\t\t" << "所需运行时间" << std::endl;
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
	std::cout<<"进程\t"<<"还需运行时间\t"<<"进程开始运行的时间\t" << "执行次数\t" << "已执行时间\t"\
		<< "结束时间：\t" << "周转时间：\n";
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
				<< "执行完毕";
			RQ1.pop_front();
		}
	}
}

void Short_precedence()
{
	initRQ2();
	std::cout << std::endl;
	Clock = 0;
	std::cout << "进程：\t" << "执行所需时间：\t" << "开始执行时间：\t" << "结束时间：\t" << "周转时间\n";
	while (!RQ2.empty()) {
		Sleep(500);
		std::list<PCB>::iterator p = RQ2.begin();
		for (std::list<PCB>::iterator it = RQ2.begin(); it != RQ2.end(); it++) {
			//找到最短预计执行的时间
			if (it->need_time < p->need_time)
				p = it;
		}
		p->start_time = Clock;
		p->end_time = p->start_time + p->need_time;
		Clock = p->end_time;
		turn_time_sum += (p->end_time - p->first_start_time);
		std::cout << p->name << "\t\t" << p->need_time << "\t\t" << p->start_time << "\t\t" \
			<< p->end_time << "\t\t" << p->end_time - p->start_time << "\t执行完毕\n";
		RQ2.erase(p);
	}

}

int main()
{
	std::cout << "执行RQ1***********************************************" << std::endl;
	Rotation();
	std::cout << std::endl << std::endl << std::endl;
	std::cout << "执行RQ2***********************************************" << std::endl;
	Short_precedence();
	std::cout << std::endl;
	std::cout << "全部进程的平均周转时间为：" << turn_time_sum / 10 << std::endl;
	std::cin.get();
}
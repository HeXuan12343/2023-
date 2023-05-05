#include<iostream>
#include<vector>

#define n 5
#define m 3

int Available[m + 1]{};//资源可使用数组
int Alloc[n + 1][m + 1]{ {} };//资源已分配数组
int Need[n + 1][m + 1]{ {} };//资源需求数组
std::vector<int> path;//可执行序列


struct Node
{
	int name;//进程ID
	int val[m + 1];//进程请求资源序列
};

bool isBigger(int a[], int b[])
{
	if ((a[1] - b[1]) < 0 && (a[2] - b[2]) < 0 && (a[3] - b[3]) < 0)
		return false;
	return true;
}

bool isSafe(int Work[])
{
	bool Finish[n + 1];
	for (int i = 1; i <= n; i++)
	{
		Finish[i] = false;
	}
	for (int j = 1; j <= n; j++)
	{
		for (int k = 1; k <= m; k++)
		{
			if ((!Finish[j]) && Need[j][k] <= Work[k])
			{
				Finish[j] = true;
				Work[k] += Alloc[j][k];
				path.push_back(j);
				j = 0;
				break;
			}
		}
	}
	for (int i = 1; i <= n; i++)
	{
		if (!Finish[i])
			return false;
	}
	return true;
}

int main()
{
	//初始化已分配数组
	Alloc[1][1] = 2; Alloc[1][2] = 1; Alloc[1][3] = 2;
	Alloc[2][1] = 4; Alloc[2][2] = 0; Alloc[3][3] = 2;
	Alloc[3][1] = 3; Alloc[3][2] = 0; Alloc[3][3] = 5;
	Alloc[4][1] = 2; Alloc[4][2] = 0; Alloc[4][3] = 4;
	Alloc[5][1] = 3; Alloc[5][2] = 1; Alloc[5][3] = 4;
	//初始化需求数组
	Need[1][1] = 3; Need[1][2] = 4; Need[1][3] = 7;
	Need[2][1] = 1; Need[2][2] = 3; Need[2][3] = 4;
	Need[3][1] = 0; Need[3][2] = 0; Need[3][3] = 3;
	Need[4][1] = 2; Need[4][2] = 2; Need[4][3] = 1;
	Need[5][1] = 1; Need[5][2] = 1; Need[5][3] = 0;
	//初始化可用数组
	Available[1] = 2; Available[2] = 3; Available[3] = 3;
	//初始化需求数组
	Node request[4];
	request[0].name = 2; request[0].val[1] = 0; request[0].val[2] = 3; request[0].val[3] = 4;
	request[1].name = 4; request[1].val[1] = 1; request[1].val[2] = 0; request[1].val[3] = 1;
	request[2].name = 1; request[2].val[1] = 2; request[2].val[2] = 0; request[2].val[3] = 1;
	request[3].name = 3; request[3].val[1] = 0; request[3].val[2] = 0; request[3].val[3] = 2;

	std::cout << "银行家算法执行结果如下：" << std::endl;
	
	for (int i = 0; i < sizeof(request) / sizeof(request[0]); i++)
	{
		path.clear();
		bool ilegal = false;
		bool block = false;
		for (int j = 1; j <= m; j++)
		{
			if (request[i].val[j] > Need[request[i].name][j])
				ilegal = true;
			if (request[i].val[j] > Available[j])
				block = true;
		}
		if (ilegal)
		{
			std::cout << "进程P" << request[i].name << "非法请求！"<<std::endl;
			continue;
		}
		if(block)
		{
			std::cout << "进程P" << request[i].name << "阻塞！"<<std::endl;
			continue;
		}
		
		for (int k = 1; k <= m; k++)
		{
			Available[k] -= request[i].val[k];
			Alloc[request[i].name][k] += request[i].val[k];
			Need[request[i].name][k] -= request[i].val[k];
		}
		int Work[m + 1];
		for (int k = 1; k <= m; k++)
		{
			Work[k] = Available[k];
		}
		if (!isSafe(Work))
		{
			for (int k = 1; k <= m; k++)
			{
				Available[k] += request[i].val[k];
				Alloc[request[i].name][k] -= request[i].val[k];
				Need[request[i].name][k] += request[i].val[k];
			}
			std::cout << "进程P" << request[i].name << "请求资源序列"<<"(";
			for (int h = 1; h <= m; h++)
			{
				if(h == m)
					std::cout << request[i].val[h];
				else
					std::cout << request[i].val[h] << ",";
			}
			std::cout << ")" << "失败！"<<std::endl;
		}
		else
		{
			std::cout << "进程P" << request[i].name << "请求成功！可行序列为：";
			std::vector<int>::iterator it = path.begin();
			while (it != path.end())
			{
				std::cout << *it << "-->";
				it++;
			}
			std::cout <<  std::endl;
			//for (int k = 1; k <= m; k++)//重置
			//{
			//	Available[k] += request[i].val[k];
			//	Alloc[request[i].name][k] -= request[i].val[k];
			//	Need[request[i].name][k] += request[i].val[k];
			//}
		}
	}

	
	std::cin.get();
}
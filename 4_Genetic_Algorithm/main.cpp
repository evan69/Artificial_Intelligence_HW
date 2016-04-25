/*
说明：
个体编码方案：一个数组，里面为0到num-1的排列，表示旅行商的路径
交配方法：采用ppt中讲到的“基于部分映射的交叉法”
变异方法：采用ppt中讲到的“基于次序的变异”
新种群的构成方法：由老种群出发，采用“确定性法”选出N个基因，经过交配和变异得到新种群
算法结束的条件：种群繁殖代数达到某个确定值后结束
*/
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstdio>

using namespace std;

struct point
{
	double x,y;
	char name;
	point(){}
	point(char _name,double _x,double _y)
	{
		name = _name;
		x = _x;
		y = _y;
	}
};

double C_MAX = 10000;
const double EPS = 1e-6;
double dist[100][100];
point arr[100];
int N = 500;
int num;
double pm = 0.95;//交配率
double pa = 0.15;//变异率
int indiv[1000][1000];
int ans_path[1000];
double ans_dist = 0;
double poss[1000];
double e[1000];
char* out;

bool cmp(int a,int b)
{
	return e[a] > e[b];
}

void input(char* filename)
{
	ifstream fin(filename);
	fin >> num;
	char c;
	double x,y;
	double max_len = 0.0;
	for(int i = 0;i < num;++i)
	{
		fin >> c >> x >> y;
		arr[i] = point(c,x,y);
	}
	for(int i = 0;i < num;i++)
	{
		for(int j = 0;j < i;j++)
		{
			dist[i][j] = sqrt((arr[i].x - arr[j].x) * (arr[i].x - arr[j].x) +
								(arr[i].y - arr[j].y) * (arr[i].y - arr[j].y));
			if(dist[i][j] > max_len) max_len = dist[i][j];
		}
	}
	
	for(int i = 0;i < num;i++)
	{
		for(int j = i;j < num;j++)
		{
			dist[i][j] = dist[j][i];
		}
	}
	C_MAX = num * max_len;
}

double cal_dist(int* array)
{
	double ret = 0.0;
	for(int i = 0;i < num;++i)
	{
		ret += dist[array[i]][array[(i+1) % num]];
	}
	return C_MAX - ret;
}

void cp_indiv(int* from,int* to)
{
	for(int i = 0;i < num;i++)
	{
		to[i] = from[i];
	}
}

void mate(int* father,int* mother)
{
	int lson[num];
	int rson[num];
	int l_pos = rand() % num;
	int h_pos = rand() % num;
	if(l_pos > h_pos) swap(l_pos,h_pos);
	for(int i = 0;i < num;++i)
	{
		lson[i] = father[i];
		rson[i] = mother[i];
		int cnt = l_pos;
		while(cnt < h_pos)
		{
			if(lson[i] == father[cnt])
			{
				lson[i] = mother[cnt];
			}
			else if(lson[i] == mother[cnt])
			{
				lson[i] = father[cnt];
			}
			
			if(rson[i] == father[cnt])
			{
				rson[i] = mother[cnt];
			}
			else if(rson[i] == mother[cnt])
			{
				rson[i] = father[cnt];
			}
			cnt++;
		}
	}
	cp_indiv(lson,father);
	cp_indiv(rson,mother);
}

void variation(int* gene)
{
	int l_pos = rand() % num;
	int h_pos = rand() % num;
	while(l_pos == h_pos)
	{
		h_pos = rand() % num;
	}
	swap(gene[l_pos],gene[h_pos]);
}

double geneticAlgorithm()
{
	int ans_no_change = 0;
	double average = 0.0;
	double cur_max = 0.0;
	double const_M = 50;
	for(int i = 0;i < N;++i)
	{
		for(int j = 0;j < num;j++)
		{
			indiv[i][j] = j;
		}
		random_shuffle(indiv[i],indiv[i] + num);
		//随机产生N个染色体作为初始值，第0代
		double dis = cal_dist(indiv[i]);
		average += dis;
		if(dis > cur_max) cur_max = dis;
	}
	average /= N;
	int generation = 0;
	while(generation < 5000)
	{
		double a = (const_M - 1) * average / (cur_max - average + EPS);
		double b = average * (cur_max - const_M * average) / (cur_max - average + EPS);
		double tot_poss = 0.0;
		for(int i = 0;i < N;++i)
		{
			poss[i] = max(a * cal_dist(indiv[i]) + b,EPS);//采用线性加速的适应函数
			tot_poss += poss[i];//根据F计算
		}
		for(int i = 0;i < N;++i)
		{
			poss[i] /= tot_poss;//每种染色体的概率
			e[i] = N * poss[i];//计算每种染色体的期望
		}
		int cnt = 0,size = 0;
		int tmp_indiv[N][num];
		for(int i = 0;i < N;++i)
		{
			while(size < (int)e[i])
			{
				cp_indiv(indiv[i],tmp_indiv[cnt]);
				cnt++;
				size++;
			}
			size = 0;
		}
		int index[N];//e数组的索引
		for(int i = 0;i < N;++i) index[i] = i;
		sort(index,index+N,cmp);//对染色体的e[i] - [e[i]]排序，index存储e的索引
		for(int i = 0;i < N;++i)
		{
			if(cnt >= N) break;
			cp_indiv(indiv[index[i]],tmp_indiv[cnt]);
			cnt++;
		}
		for(int i = 0;i < N;++i)
		{
			cp_indiv(tmp_indiv[i],indiv[i]);
		}
		//至此，indiv数组得到了N个染色体
		//开始交配
		for(int i = 0;i < N;i += 2)
		{
			double tmp_p = (double)rand() / RAND_MAX;
			if(tmp_p < pm)
			{
				mate(indiv[i],indiv[i+1]);
			}
		}
		//交配完毕
		//开始变异
		for(int i = 0;i < N;i++)
		{
			double tmp_p = (double)rand() / RAND_MAX;
			if(tmp_p < pa)
			{
				variation(indiv[i]);
			}
		}
		//变异结束
		double new_average = 0.0;
		double new_cur_max = 0.0;
		for(int i = 0;i < N;++i)
		{
			//随机产生N个染色体作为初始值，第0代
			double dis = cal_dist(indiv[i]);
			new_average += dis;
			if(dis > new_cur_max) new_cur_max = dis;
		}
		new_average /= N;
		
		average = new_average;
		cur_max = new_cur_max;
		
		double last_ans_dist = ans_dist;
		for(int i = 0;i < N;++i)
		{
			double dis = cal_dist(indiv[i]);
			if(dis > ans_dist)
			{
				ans_dist = dis;
				cp_indiv(indiv[i],ans_path);
			}
		}
		generation++;
		if (fabs(last_ans_dist - ans_dist) < 1e-6)
		{
			ans_no_change++;
		}//统计答案不发生变化的代数
		if (ans_no_change > 100)
		{
			pa += 0.05;
			pa = min(pa, 0.8);
			ans_no_change = 0;
		}
		//若长期局限在一个答案处，可能仅仅找到局部最优解，因此需要增加变异率
	}
	ofstream fout(out);
	for(int i = 0;i < num;++i)
	{
		fout << arr[ans_path[i]].name << " ";
	}
	fout << C_MAX - ans_dist << endl;
	fout.close();
	fout.flush();
	return C_MAX - ans_dist;
}

int main(int argc,char** argv)
{
	if(argc < 3)
	{
		cout << "Wrong input!" << endl;
		cout << "Usage:main.exe [input file] [output file]" << endl;
		return 0;
	}
	out = argv[2];
	input(argv[1]);
	srand(time(NULL));
	geneticAlgorithm();
	return 0;
}
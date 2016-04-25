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

double eps = 1e-6;//正温度
double dist[100][100];
point arr[100];
int n;
char* out;
int anspath[100];

void input(char* filename)
{
	ifstream fin(filename);
	fin >> n;
	char c;
	double x,y;
	for(int i = 0;i < n;++i)
	{
		fin >> c >> x >> y;
		arr[i] = point(c,x,y);
	}
	for(int i = 0;i < n;i++)
	{
		for(int j = 0;j < i;j++)
		{
			dist[i][j] = sqrt((arr[i].x - arr[j].x) * (arr[i].x - arr[j].x) +
								(arr[i].y - arr[j].y) * (arr[i].y - arr[j].y));
		}
	}
	
	for(int i = 0;i < n;i++)
	{
		for(int j = i;j < n;j++)
		{
			dist[i][j] = dist[j][i];
		}
	}
}

double cal_dist(int* array)
{
	double ret = 0.0;
	for(int i = 0;i < n;++i)
	{
		ret += dist[array[i]][array[(i+1) % n]];
	}
	return ret;
}

double cal_df()//确定delta f，这里随机出sample个f的差值取平均计算
{
	double df = 0.0;
	double current = 0.0,last = 0.0;
	int sample = 150;
	int state[2*n];
	for (int i = 0; i < n; i++)
		state[i] = i;
	random_shuffle(state, state + n);
	last = cal_dist(state);
	for(int i = 0;i < sample;i++)
	{
		random_shuffle(state,state + n);
		current = cal_dist(state);
		df += fabs(current - last);
		last = current;
	}
	df /= sample;
	return df;
}

void output(int* state,double cur_f,ofstream& fout)
{
	for(int i = 0;i < n;i++)
	{
		fout << arr[state[i]].name << " ";
	}
	fout << cur_f << endl;
	fout.flush();
}

double tsp()
{
	ofstream fout(out);
	double p0 = 0.9;//初始接受概率
	double df = cal_df();//利用接受概率确定初始温度
	double t0 = df / log(1 / p0);//初始温度
	double down_alpha = 0.95;//下降系数
	
	int Lk = 10 * n;
	int cnt = 0;//终止条件，外层循环结果不变积累的次数
	
	int state[2*n];
	for (int i = 0; i < n; i++)
	{
		state[i] = i;
		anspath[i] = i;
	}
	random_shuffle(state + 1, state + n);//随机产生一个初始解
	double cur_f = cal_dist(state);
	double T = t0;//温度为初始温度
	while(T > eps)//算法终止的条件为温度降到eps这一很小的值以下，即采用0度法
	{
		int iter_cnt = 1;
		output(state,cur_f,fout);
		if(cal_dist(state) < cal_dist(anspath))
		{
			for(int i = 0;i < n;++i)
			{
				anspath[i] = state[i];
			}
		}
		int ac_cnt = 0;
		while(iter_cnt < Lk)//每一个温度下，迭代次数达到Lk就终止
		{
			int u = 0,v = 0;
			while(u >= v)
			{
				u = rand() % n;
				v = rand() % n;
			}
			int newState[2*n];
			int index = 0;
			while(index < u + 1)
			{
				newState[index] = state[index];
				index++;
			}
			while(index < v + 1)
			{
				newState[index] = state[u+v+1-index];
				index++;
			}
			while(index < n)
			{
				newState[index] = state[index];
				index++;
			}
			
			double new_f = cal_dist(newState);
			cur_f = cal_dist(state);
			double P = exp(-(new_f - cur_f) / T);
			double tP = (double)rand() / (double)RAND_MAX;
			if(new_f < cur_f || tP < P)//状态被接受的条件：要么f减小，要么以某一概率接受
			{
				for(int i = 0;i < n;++i)
				{
					state[i] = newState[i];
				}
				cur_f = new_f;
				ac_cnt++;
			}
			iter_cnt++;
		}
		T *= down_alpha;//温度下降采用指数下降的办法
	}
	if(cal_dist(anspath) > cal_dist(state)) output(state,cur_f,fout);
	else
	{
		for(int i = 0;i < 10;++i)
		{
			output(anspath,cal_dist(anspath),fout);
		}
	}
	fout.close();
	return cur_f;
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
	srand(time(NULL));
	input(argv[1]);
	tsp();
	for(int i = 0;i < 100000000;++i)
	{
		
	}
	return 0;
}
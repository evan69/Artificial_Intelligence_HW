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

double cal_df()
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

void output(int* state,double cur_f)
{
	for(int i = 0;i < n;i++)
	{
		cout << arr[state[i]].name;
	}
	cout << " " << cur_f << endl;
}

double tsp()
{
	double p0 = 0.9;//初始接受概率
	double df = cal_df();
	double t0 = df / log(1 / p0);//初始温度
	double down_alpha = 0.95;//下降系数
	
	int Lk = 10 * n;
	int cnt = 0;//终止条件，外层循环结果不变积累的次数
	
	int state[2*n];
	for (int i = 0; i < n; i++)
		state[i] = i;
	random_shuffle(state + 1, state + n);//随机产生一个初始解
	double cur_f = cal_dist(state);
	double T = t0;//温度为初始温度
	while(T > eps)//采用0度法
	{
		int iter_cnt = 1;
		output(state,cur_f);
		int ac_cnt = 0;
		while(iter_cnt < Lk)
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
			if(new_f < cur_f || tP < P)
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
		T *= down_alpha;
	}
	output(state,cur_f);
	return cur_f;
}

int main(int argc,char** argv)
{
	if(argc < 2)
	{
		cout << "Wrong input!" << endl;
		cout << "Usage:main.exe [input file] " << endl;
		return 0;
	}
	srand(time(NULL));
	input(argv[1]);
	cout << tsp() << endl;
	return 0;
}
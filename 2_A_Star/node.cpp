#include "node.h"
#include <cmath>
#include <algorithm>
#include <cassert>
using namespace std;
node::node()
{
	for(int i = 0;i < 3;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			data[i][j] = 0;
		}
	}
	last = 0;
	g = 0;
}

node::node(int** a)
{
	for(int i = 0;i < 3;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			data[i][j] = a[i][j];
		}
	}
	last = 0;
	g = 0;
}

node::node(const node& a)
{
	for(int i = 0;i < 3;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			data[i][j] = a.data[i][j];
		}
	}
	this->last = a.last;
	if(last != 0 && (*this->last == *this))
	{
		assert(0);
	}
	this->g = a.g;
}

void node::operator=(const node& a)
{
	for(int i = 0;i < 3;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			data[i][j] = a.data[i][j];
		}
	}
	this->last = a.last;
	this->g = a.g;
}

vector<node> node::expand() const
{
	vector<node> ret;
	int ii = 0,jj = 0;
	for(int i = 0;i < 3;++i)
		for(int j = 0;j < 3;++j)
			if(data[i][j] == 0)
			{
				ii = i;
				jj = j;
			}
	for(int i = 0;i < 3;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			if(abs(i-ii)+abs(j-jj) == 1)
			{
				int** tmp = new int*[3];
				for(int r = 0;r < 3;r++)
				{
					tmp[r] = new int[3];
				}
				for(int r = 0;r < 3;r++)
				{
					for(int s = 0;s < 3;s++)
					{
						tmp[r][s] = this->data[r][s];
					}
				}
				swap(tmp[i][j],tmp[ii][jj]);
				ret.push_back(node(tmp));
				for(int i = 0;i < 3;i++)
				{
					delete[] tmp[i];
				}
				delete[] tmp;
			}
		}
	}
	return ret;
}

int node::cal() const
/*
计算某个状态的h函数
意义为：该状态下不在位的将牌数
*/
{
	int cnt = 0;
	int target[3][3] = {
		{1,2,3},
		{8,0,4},
		{7,6,5}
	};
	for(int i = 0;i < 3;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			if(data[i][j] == 0) continue;
			if(data[i][j] != target[i][j])
				cnt++;
		}
	}
	return cnt;
}

bool operator<(const node& a,const node& b)
{
	return (a.g + a.cal() < b.g + b.cal());
}
/*
bool operator<(const node& a,const node& b)
{
	for(int i = 0;i < 3;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			if(a.data[i][j] < b.data[i][j])
				return true;
		}
	}
	return false;
}
*/

bool operator==(const node& a,const node& b)
{
	for(int i = 0;i < 3;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			if(a.data[i][j] != b.data[i][j])
				return false;
		}
	}
	return true;
}

ostream& operator<<(ostream& o,const node& a)
{
	for(int i = 0;i < 3;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			o << a.data[i][j] << " ";
		}
		o << endl;
	}
	return o;
}


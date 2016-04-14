//回溯法求解数独
#include <iostream>//输入输出 
#include <fstream>//输入输出
using namespace std;
char p[20][20];//以字符形式储存数 
bool check_row(int x,int y)
//检查这一行已经填的数有没有重复 
{
	for(int i = 0;i < 9;i++)
	{
		if(i != y && p[x][i] == p[x][y])
		{
			return false;
		}
	}
	return true;
}

bool check_col(int x,int y)
//检查这一列已经填的数有没有重复 
{
	for(int i = 0;i < 9;i++)
	{
		if(i != x && p[i][y] == p[x][y])
		{
			return false;
		}
	}
	return true;
}

bool check_sqr(int x,int y)
//检查同一个正方形中有没有重复的数 
{
	int r = x / 3;
	int s = y / 3;
	for(int i = 3*r;i < 3*(r+1);i++)
	{
		for(int j = 3*s;j < 3*(s+1);j++)
		{
			if(i != x && j != y && p[i][j] == p[x][y])
			{
				return false;
			}
		}
	}
	return true;
}

bool check(int x,int y)
//检查某一个位置是否可以 
{
	if(check_row(x,y) && check_col(x,y) && check_sqr(x,y))
	{
		return true;
	}
	return false;
}

bool f(int x,int y)
{
//	cout << x << " " << y <<endl;
	if(y == 9)
	{
		return f(x+1,0);//到达界外，转下一行 
	}
	if(x == 9) return true;//已经搜索完毕 
	if(p[x][y] != '0')
	{
		return f(x,y+1);
		//不是0的点无需处理 
	}
	for(int i = 0;i < 9;i++)
	{
		p[x][y] = char('1' + i); 
		if(check(x,y))
		//取第一条可应用的规则 
		//生成了数组p的一个新状态
		{
			bool tmp = f(x,y+1);
			//递归调用 
			if(tmp) return true;
		}
	}
	p[x][y] = '0';
	return false;
	//失败 必须回溯 改回p数组在x，y处的值 
}

int main(int argc,char** argv)
{
	ifstream fin;
	fin.open(argv[1]);
	ofstream fout;
	fout.open(argv[2]);
	for(int i = 0;i < 9;i++)
	{
		for(int j = 0;j < 9;j++)
		{
			fin >> p[i][j];
		}
	}
	fin.close();
	f(0,0);//从第一个点开始 
	for(int i = 0;i < 9;i++)
	{
		for(int j = 0;j < 9;j++)
		{
			fout << p[i][j];
		}
		fout << '\n';
	}
	fout.close();
	return 0;
}



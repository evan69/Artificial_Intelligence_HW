#include "node.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <stack>
using namespace std;
int target[3][3] = {
	{1,2,3},
	{8,0,4},
	{7,6,5}
};
int target2[9];
int input[9];
vector<node> open;
vector<node> close;
bool myfind(vector<node>& arr,node& x)
{
	for(vector<node>::iterator it = arr.begin();it != arr.end();++it)
	{
		if(*it == x) return true;
	}
	return false;
}
int caloe(int* arr)
{
	int cnt = 0;
	for(int i = 0;i < 9;i++)
	{
		int j = 0;
		while(j < i)
		{
			if(arr[j] > 0 && arr[j] < arr[i])
			{
				cnt++;
			}
			j++;
		}
	}
	return cnt % 2;
}
int main(int argc,char** argv)
{
	if(argc < 3)
	{
		cout << "Wrong input\n" << endl;
		return 0;
	}
	int** tmp = new int*[3];
	int** tar = new int*[3];
	for(int i = 0;i < 3;i++)
	{
		tmp[i] = new int[3];
		tar[i] = new int[3];
	}
	ifstream fin(argv[1]);
	for(int i = 0;i < 3;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			fin >> tmp[i][j];
			input[3*i+j] = tmp[i][j];
			tar[i][j] = target[i][j];
			target2[3*i+j] = target[i][j];
		}
	}
	fin.close();
	ofstream fout(argv[2]);
	if(caloe(target2) != caloe(input))
	{
		fout << "no solution" << endl;
		fout.close();
		return 0;
	}
	
	node s(tmp);
	node t(tar);
	open.push_back(s);
	while(1)
	{
		if(open.size() == 0)
		{
			cout << "fail" << endl;
			break;
		}
		sort(open.begin(),open.end());
		node* n = new node();
		*n = open[0];
		open.erase(open.begin());
		close.push_back(*n);
		if(*n == t)
		{
			stack<node> st;
			st.push(*n);
			int cnt = 0;
			while(n->last != 0)
			{
				n = n->last;
				st.push(*n);
				cnt++;
			}
			fout << cnt << endl;
			fout << endl;
			fout.flush();
			st.pop();
			while(!st.empty())
			{
				fout << st.top() << endl;
				st.pop();
			}
			fout.close();
			break;
		}
		vector<node> exp = n->expand();
		for(vector<node>::iterator it = exp.begin();it != exp.end();++it)
		{
			if(myfind(open,*it)) continue;
			if(myfind(close,*it)) continue;
			it->g = n->g + 1;
			it->last = n;
			open.push_back(*it);
		}	
	}
	return 0;
}
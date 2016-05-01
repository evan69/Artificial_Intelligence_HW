#include "myai.h"
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <fstream>

#include <conio.h>
#include <atlstr.h>

using namespace std;

const double C = 0.8;
const double MAX_NUM = 100000.0;
int M, N, noX, noY;
int root = 0;
int** root_board;
int* root_top;
int** cur_board;
int* cur_top;


struct node
{
	enum node_kind{UN_INIT,OPPO_ACTION,MY_ACTION};
	node_kind kind;//节点种类：自己落子或对方落子
	int win;
	//胜利场数
	int visit;
	//访问次数
	int parent;
	//父亲节点
	int col;
	//落子位置
	vector<int> kid;
	//儿子
	double ucb;
	//信心上限值
	node()
	{
		kind = UN_INIT;
		win = 0;
		visit = 0;
		parent = 0;
		col = -1;
		ucb = 0;
	}
};

const int MAXSIZE = 1000000;
node tree[MAXSIZE];
int treesize = 0;

void cal_ucb(int index)
{
	_cprintf("cal_ucb:%d\n",index);
	node a = tree[index];
	if(a.visit == 0)
	{
		tree[index].ucb = MAX_NUM;
		return;
	}
	if(tree[a.parent].visit == 0 && index != root) _cprintf("bug\n");
	tree[index].ucb = (double)a.win / a.visit + C * sqrt(log(tree[a.parent].visit) / a.visit);
	//修改
}

int simple_search()
{
	node simtree[1000];
	int flag[1000];
	simtree[0].kind = node::MY_ACTION;
	int index = 0;
	for(int i = 0;i < 3;++i)
	{
		for(int j = 0;j < N;++j)
		{

		}
	}
	int simsize = 1;
}

int selection()
{
	/*
	int* cur_top = new int[N];
	for(int i = 0;i < N;++i)
		cur_top[i] = root_top[i];
	*/
	int cur = root;
	_cprintf("size:%d\n",tree[cur].kid.size());
	tree[cur].visit++;
	while(tree[cur].kid.size() > 0)
	{
		_cprintf("select cur:%d\n",cur);
		int max_index = -1;
		double max_ucb = -9999999.9;
		for(int i = 0;i < tree[cur].kid.size();++i)
		{
			if(tree[tree[cur].kid[i]].ucb > max_ucb)
			{
				max_ucb = tree[tree[cur].kid[i]].ucb;
				max_index = tree[cur].kid[i];
			}
		}
		_cprintf("select0\n");
		cur_top[tree[max_index].col]--;
		_cprintf("select1\n");
		if(cur_top[tree[max_index].col] < 0) _cprintf("bug!!\n");
		if(tree[max_index].col < 0) _cprintf("bug2!!\n");
		_cprintf("%d %d\n",tree[max_index].col,cur_top[tree[max_index].col]);
		//cur_board[tree[max_index].col][cur_top[tree[max_index].col]] = tree[cur].kind;//bug here
		cur_board[cur_top[tree[max_index].col]][cur_top[tree[max_index].col]] = tree[cur].kind;
		_cprintf("select2\n");
		if(tree[max_index].col == noY && cur_top[tree[max_index].col] == noX + 1)
			cur_top[tree[max_index].col]--;
		_cprintf("select3\n");
		cur = max_index;
		_cprintf("select4\n");
		tree[cur].visit++;
		_cprintf("select5\n");
	}
	return cur;
}

int expansion(int pos)
{
	_cprintf("expansion:%d\n",pos);
	for(int i = 0;i < N;++i)
	{
		if(cur_top[i] > 0)
		{
			_cprintf("son:%d\n",treesize);
			//if(treesize == 2) system("pause");
			tree[treesize].parent = pos;
			tree[pos].kid.push_back(treesize);
			tree[treesize].col = i;
			if(tree[pos].kind == node::MY_ACTION)
				tree[treesize].kind = node::OPPO_ACTION;
			if(tree[pos].kind == node::OPPO_ACTION)
				tree[treesize].kind = node::MY_ACTION;
			tree[treesize].visit = 0;
			cal_ucb(treesize);
			_cprintf("sonucb %llf\n",tree[treesize].ucb);
			//system("pause");
			treesize++;
		}
	}
	int ret = tree[pos].kid[rand() % tree[pos].kid.size()];
	cur_top[tree[ret].col]--;
	if(tree[ret].col == noY && cur_top[tree[ret].col] == noX + 1)
		cur_top[tree[ret].col]--;
	tree[ret].visit = 1;
	cal_ucb(ret);
	return ret;
}

int simulation(int pos,int** para_board)
{
	_cprintf("simulation:%d\n",pos);
	int result;
	int** local_board = new int*[M];
	for(int i = 0;i < M;++i)
		local_board[i] = new int[N];
	copy_board(para_board,local_board);
	if(tree[pos].kind == node::OPPO_ACTION)
	{
		int userturn = rand() % N;
		while(cur_top[userturn] <= 0)
			userturn = rand() % N;
		cur_top[userturn]--;
		local_board[cur_top[userturn]][userturn] = 1;
		if(tree[userturn].col == noY && cur_top[tree[userturn].col] == noX + 1)
			cur_top[tree[userturn].col]--;
		if(userWin(cur_top[userturn],userturn,M,N,local_board))
		{
			result = 0;
			return result;
		}
		if(isTie(N,cur_top))
		{
			result = 0;
			return result;
		}
	}
	while(1)
	{
		int myturn = rand() % N;
		while(cur_top[myturn] <= 0)
			myturn = rand() % N;
		cur_top[myturn]--;
		local_board[cur_top[myturn]][myturn] = 2;
		if(tree[myturn].col == noY && cur_top[tree[myturn].col] == noX + 1)
			cur_top[tree[myturn].col]--;
		if(machineWin(cur_top[myturn],myturn,M,N,local_board))
		{
			result = 1;
			break;
		}
		if(isTie(N,cur_top))
		{
			result = 0;
			break;
		}

		int userturn = rand() % N;
		while(cur_top[userturn] <= 0)
			userturn = rand() % N;
		cur_top[userturn]--;
		para_board[cur_top[userturn]][userturn] = 1;
		if(tree[userturn].col == noY && cur_top[tree[userturn].col] == noX + 1)
			cur_top[tree[userturn].col]--;
		if(userWin(cur_top[userturn],userturn,M,N,para_board))
		{
			result = 0;
			break;
		}
		if(isTie(N,cur_top))
		{
			result = 0;
			break;
		}
	}
	return result;
}

void backPropagation(int pos,int delta)
{
	int cur = pos;
	int flag = 1;
	//表示最大最小节点的种类
	while(cur != root)
	{
		_cprintf("back:%d\n",cur);
		cur = tree[cur].parent;
		//tree[pos].visit++;
		tree[cur].win += flag * delta;
		flag = flag;
		cal_ucb(cur);
	}
}

void copy_board(int** from,int** to)
{
	for(int i = 0;i < M;++i)
	{
		for(int j = 0;j < N;++j)
		{
			to[i][j] = from[i][j];
		}
	}
}

void hyf_getPoint(const int para_M, const int para_N, const int* top, int** board, 
	const int lastX, const int lastY, const int para_noX, const int para_noY,int& ans_x,int& ans_y)
{
	//ofstream fout("debug.txt");
	clock_t begin = clock();
	AllocConsole();
	_cprintf("%d %d\n",para_M,para_N);
	
	_cprintf("1\n");
	//srand(time(0));
	_cprintf("1\n");
	//初始化各个全局变量
	M = para_M;
	N = para_N;
	noX = para_noX;
	noY = para_noY;
	_cprintf("1\n");
	root_board = new int*[M];
	for(int i = 0;i < M;++i)
		root_board[i] = new int[N];
	_cprintf("1\n");
	copy_board(board,root_board);
	_cprintf("1\n");
	cur_board = new int*[M];
	for(int i = 0;i < M;++i)
		cur_board[i] = new int[N];
	_cprintf("1\n");
	root_top = new int[N];
	cur_top = new int[N];
	_cprintf("1\n");
	for(int i = 0;i < N;++i)
	{
		root_top[i] = top[i];
	}
	
	//初始化根节点
	if(root == 0)
	{
		root = 1;
		treesize = 2;
		tree[1] = node();//1号为根节点
		tree[1].kind = node::MY_ACTION;
		tree[1].col = -1;
	}
	else
	{
		for(int i = 0;i < tree[root].kid.size();++i)
		{
			if(tree[tree[root].kid[i]].col == lastY)
			{
				root = tree[root].kid[i];
				break;
			}
		}
	}


	_cprintf("init\n");
	
	while(1)
	{
		//计时
		clock_t end = clock();
		double totaltime = (double)(end - begin)/ CLOCKS_PER_SEC;
		_cprintf("%llf\n",totaltime);
		if(totaltime > 4) break;
		//以下重新产生cur_board和cur_top
		copy_board(board,cur_board);
		for(int i = 0;i < N;++i)
			cur_top[i] = root_top[i];
		//选取节点
		_cprintf("2\n");
		int to_expand = selection();
		_cprintf("3\n");
		int pos = expansion(to_expand);
		_cprintf("4\n");
		int result = simulation(pos,cur_board);
		_cprintf("5\n");
		tree[pos].visit++;
		tree[pos].win += result;
		cal_ucb(pos);
		_cprintf("6\n");
		backPropagation(pos,result);
		_cprintf("7\n");
	}
	int max_index = -1;
	double max_ratio = -999999.9;
	for(int i = 0;i < tree[root].kid.size();++i)
	{
		_cprintf("ucb:%llf ",tree[tree[root].kid[i]].ucb);
		_cprintf("sonwin:%d sonvisit:%d\n",tree[tree[root].kid[i]].win,tree[tree[root].kid[i]].visit);
		double cur_ratio = (double)tree[tree[root].kid[i]].win / (tree[tree[root].kid[i]].visit);
		_cprintf("ratio:%llf\n",cur_ratio);
		if(cur_ratio > max_ratio)
		{
			max_ratio = cur_ratio;
			max_index = tree[root].kid[i];
		}
	}
	ans_y = tree[max_index].col;
	ans_x = root_top[ans_y] - 1;
	root = max_index;
	_cprintf("ans:%d %d\n",ans_x,ans_y);
}
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <fstream>
#include <vector>
#include <conio.h>
#include <atlstr.h>
#include <cassert>
#include "Judge.h"
#include "myai.h"
using namespace std;

const double C = 0.8;
const double MAX_NUM = 100000.0;
int M, N, noX, noY;
int root = 1;
int** root_board;
int* root_top;
int** cur_board;
int* cur_top;
//int cannot[30];//第一步不能走的列，1表示不可走

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
	bool gameover;
	void clearall()
	{
		kind = UN_INIT;
		win = 0;
		visit = 0;
		parent = 0;
		col = -1;
		kid.clear();
		ucb = 0;
		gameover = false;//是否是已经分出胜负的节点
	}
	node()
	{
		kind = UN_INIT;
		win = 0;
		visit = 0;
		parent = 0;
		col = -1;
		ucb = 0;
		gameover = false;//是否是已经分出胜负的节点
	}
};

const int MAXSIZE = 20000000;
node tree[MAXSIZE];
int treesize = 0;
bool choose_gameover_node = false;
void copy_board(int** from,int** to);

int choose(int** para_board,int* para_top,int col,int gamer)//gamer:1-user 2-machine return choose row
{
	int ret = -1;
	para_top[col]--;
	para_board[para_top[col]][col] = gamer;
	ret = para_top[col];
	if(col == noY && para_top[col] == noX + 1)
		para_top[col]--;
	return ret;
}

void backchoose(int** para_board,int* para_top,int col)
{
	if(col == noY && para_top[col] == noX)
		para_top[col]++;
	para_board[para_top[col]][col] = 0;
	para_top[col]++;
}
/*
void print_board(int** para_board)
{
	for(int i = 0;i < M;++i)
	{	
		for(int j = 0;j < N;++j)
		{
			_cprintf("%d ",para_board[i][j]);
		}
		_cprintf("\n");
	}
}
*/
void cal_ucb(int index)
{
	node& a = tree[index];
	if(a.visit == 0)
	{
		tree[index].ucb = MAX_NUM;
		return;
	}
	assert(tree[a.parent].visit > 0);
	tree[index].ucb = (double)a.win / a.visit + C * sqrt(2.0 * log(tree[a.parent].visit) / a.visit);
	//修改
}

int selection()
{
	int cur = root;
	while(tree[cur].kid.size() > 0)
	{
		int max_index = -1;
		double max_ucb = -99999.9;
		for(int i = 0;i < tree[cur].kid.size();++i)
		{
			cal_ucb(tree[cur].kid[i]);
			if(tree[tree[cur].kid[i]].ucb > max_ucb)// && cannot[tree[tree[cur].kid[i]].col] == 0)//add special judge
			{
				max_ucb = tree[tree[cur].kid[i]].ucb;
				max_index = tree[cur].kid[i];
			}
		}
		if(max_index == -1)
			max_index = tree[cur].kid[rand() % tree[cur].kid.size()];
		choose(cur_board,cur_top,tree[max_index].col,tree[cur].kind);
		cur = max_index;
		//have added code here
		if(tree[cur].gameover == true)
		{
			choose_gameover_node = true;
			return cur;
		}
		//--------------------
	}
	return cur;
}

int expansion(int pos)
{
	if(treesize + 20 > MAXSIZE)
	{
		choose_gameover_node = true;
		return pos;
	}
	for(int i = 0;i < N;++i)
	{
		if(cur_top[i] > 0)
		{
			tree[treesize].parent = pos;
			tree[pos].kid.push_back(treesize);
			tree[treesize].col = i;
			if(tree[pos].kind == node::MY_ACTION)
				tree[treesize].kind = node::OPPO_ACTION;
			if(tree[pos].kind == node::OPPO_ACTION)
				tree[treesize].kind = node::MY_ACTION;
			tree[treesize].visit = 0;
			//have added code here
			int row = choose(cur_board,cur_top,i,tree[pos].kind);
			if(tree[pos].kind == node::OPPO_ACTION)
			{
				if(userWin(row,i,M,N,cur_board) || isTie(N,cur_top))
				{
					//print_board(cur_board);
					tree[treesize].gameover = true;
				}
			}
			if(tree[pos].kind == node::MY_ACTION)
			{
				if(machineWin(row,i,M,N,cur_board) || isTie(N,cur_top))
				{
					//print_board(cur_board);
					tree[treesize].gameover = true;
				}
			}
			backchoose(cur_board,cur_top,i);
			//--------------------
			treesize++;
		}
	}
	if(tree[pos].kid.size() == 0)
	{
		return pos;
	}
	int ret = tree[pos].kid[rand() % tree[pos].kid.size()];
	choose(cur_board,cur_top,tree[ret].col,tree[pos].kind);
	//have added code here
	if(tree[ret].gameover == true)
		choose_gameover_node = true;
	//--------------------
	return ret;
}

int simulation(int pos,int print = 0)
{
	int result;
	int row = -1;
	int** local_board = new int*[M];
	for(int i = 0;i < M;++i)
		local_board[i] = new int[N];
	int* local_top = new int[N];
	copy_board(cur_board,local_board);
	for(int i = 0;i < N;++i)
		local_top[i] = cur_top[i];
	if(tree[pos].kind == node::OPPO_ACTION)
	{
		int userturn = rand() % N;
		while(local_top[userturn] <= 0)
			userturn = rand() % N;
		row = choose(local_board,local_top,userturn,node::OPPO_ACTION);
		if(userWin(row,userturn,M,N,local_board))
		{
			result = 0;
			return result;
		}
		if(isTie(N,local_top))
		{
			result = 0;
			return result;
		}
	}
	while(1)
	{
		int myturn = rand() % N;
		while(local_top[myturn] <= 0)
			myturn = rand() % N;
		row = choose(local_board,local_top,myturn,node::MY_ACTION);
		if(machineWin(row,myturn,M,N,local_board))
		{
			result = 1;
			break;
		}
		if(isTie(N,local_top))
		{
			result = 0;
			break;
		}

		int userturn = rand() % N;
		while(local_top[userturn] <= 0)
			userturn = rand() % N;
		row = choose(local_board,local_top,userturn,node::OPPO_ACTION);
		if(userWin(row,userturn,M,N,local_board))
		{
			result = 0;
			break;
		}
		if(isTie(N,local_top))
		{
			result = 0;
			break;
		}
	}
	
	for(int i = 0;i < M;++i)
		delete local_board[i];
	delete[] local_board;
	delete local_top;
	return result;
}

void backPropagation(int pos,int delta)
{
	int cur = pos;
	int flag = -1;
	if(tree[pos].kind == node::OPPO_ACTION)
		flag = 1;
	else
		flag = -1;
	//表示最大最小节点的种类
	while(cur != tree[root].parent)
	{
		tree[cur].visit++;
		tree[cur].win += flag * (2*delta - 1);
		flag = -flag;
		cur = tree[cur].parent;
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

bool used = false;
void hyf_getPoint(const int para_M, const int para_N, const int* top, int** board, 
	const int lastX, const int lastY, const int para_noX, const int para_noY,int& ans_x,int& ans_y)
{
	clock_t begin = clock();
	for(int i = 0;i < MAXSIZE;++i)
	{
		//tree[i] = node();
		tree[i].clearall();
	}
	if(!used)
	{
		srand(time(NULL));
		M = para_M;
		N = para_N;
		noX = para_noX;
		noY = para_noY;
		root_board = new int*[M];
		for(int i = 0;i < M;++i)
			root_board[i] = new int[N];
		cur_board = new int*[M];
		for(int i = 0;i < M;++i)
			cur_board[i] = new int[N];
		root_top = new int[N];
		cur_top = new int[N];
		root = 1;
		treesize = 2;
		tree[root] = node();//1号为根节点
		tree[root].kind = node::MY_ACTION;
		tree[root].col = -1;
		used = true;
	}
	else
	{
		root = 1;
		treesize = 2;
		tree[root] = node();//1号为根节点
		tree[root].kind = node::MY_ACTION;
		tree[root].col = -1;
	}
	
	//初始化各个全局变量
	copy_board(board,root_board);
	copy_board(board,cur_board);
	for(int i = 0;i < N;++i)
	{
		root_top[i] = top[i];
		cur_top[i] = top[i];
	}
	//初始化根节点
	double totaltime = 0;
	while(totaltime < 4.5)
	{
		clock_t end = clock();
		totaltime = (double)(end - begin)/ CLOCKS_PER_SEC;
		if(totaltime > 4.5) break;
		copy_board(root_board,cur_board);
		for(int i = 0;i < N;++i)
			cur_top[i] = root_top[i];
		//选取节点
		choose_gameover_node = false;//add
		int to_expand = selection();
		if(choose_gameover_node == true)
		{
			if(tree[to_expand].kind == node::MY_ACTION) backPropagation(to_expand,0);
			if(tree[to_expand].kind == node::OPPO_ACTION) backPropagation(to_expand,1);
			continue;
		}//add
		int pos = expansion(to_expand);
		if(choose_gameover_node == true)
		{
			if(tree[pos].kind == node::MY_ACTION) backPropagation(pos,0);
			if(tree[pos].kind == node::OPPO_ACTION) backPropagation(pos,1);
			continue;
		}//add
		int result = simulation(pos);
		backPropagation(pos,result);
	}
	int max_col = -1;
	double max_ratio = -9999.9;
	int max_index = -1;
	for(int i = 0;i < tree[root].kid.size();++i)
	{
		int index = tree[root].kid[i];
		double cur_ratio = (double)tree[index].win / (tree[index].visit);
		if(cur_ratio > max_ratio)
		{
			max_ratio = cur_ratio;
			max_col = tree[index].col;
			max_index = i;
		}
	}
	if(max_col == -1)
	{
		max_col = tree[tree[root].kid[rand() % tree[root].kid.size()]].col;
	}
	ans_y = max_col;
	ans_x = root_top[ans_y] - 1;
}
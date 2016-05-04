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

//const double C = 0.8;
const double C = 0.8;
const double MAX_NUM = 100000.0;
int M, N, noX, noY;
 int root = 1;
int** root_board;
int* root_top;
int** cur_board;
int* cur_top;
int cannot[30];//第一步不能走的列，1表示不可走

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
//node* tree;
int treesize = 0;
bool choose_gameover_node = false;
void copy_board(int** from,int** to);

int choose(int** para_board,int* para_top,int col,int gamer)//gamer:1-user 2-machine return choose row
{
	int ret = -1;
	para_top[col]--;
	assert(para_board[para_top[col]][col] == 0);
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
	//tree[cur].visit++;
	//add special judge
	int ceng = 0;
	//-----------------
	while(tree[cur].kid.size() > 0)
	{
		ceng++;
		int max_index = -1;
		double max_ucb = -99999.9;
		for(int i = 0;i < tree[cur].kid.size();++i)
		{
			cal_ucb(tree[cur].kid[i]);
			//if(ceng == 1)
			//	_cprintf("%llf ",tree[tree[cur].kid[i]].ucb);
			if(tree[tree[cur].kid[i]].ucb > max_ucb && cannot[tree[tree[cur].kid[i]].col] == 0)//add special judge
			{
				max_ucb = tree[tree[cur].kid[i]].ucb;
				max_index = tree[cur].kid[i];
			}
		}
		//_cprintf("\n");
		/*
		cur_top[tree[max_index].col]--;
		cur_board[cur_top[tree[max_index].col]][cur_top[tree[max_index].col]] = tree[cur].kind;
		if(tree[max_index].col == noY && cur_top[tree[max_index].col] == noX + 1)
			cur_top[tree[max_index].col]--;
		*/
		//add special judge
		if(max_index == -1)
			max_index = tree[cur].kid[rand() % tree[cur].kid.size()];
		assert(max_index > -1);
		//-----------------
		choose(cur_board,cur_top,tree[max_index].col,tree[cur].kind);
		cur = max_index;
		//_cprintf("choose col:%d\n",tree[cur].col);
		//modifying
		//tree[cur].visit++;
		//have added code here
		if(tree[cur].gameover == true)
		{
			choose_gameover_node = true;
			//add special judge
			if(ceng == 2)
			{
				cannot[tree[tree[cur].parent].col] = 1;//该列不可走，对方必胜
			}
			//-----------------
			return cur;
		}
		//--------------------
	}
	return cur;
}

int expansion(int pos)
{
	//_cprintf("expansion:%d\n",pos);
	assert(pos > 0);
	for(int i = 0;i < N;++i)
	{
		if(cur_top[i] > 0)
		{
			//if(pos == root && cannot[i] == 1) continue;//bug:cannot 全部为1
			//if(cannot[i] == 1) continue;
			if(treesize >= MAXSIZE - 100)
				_cprintf(">maxsize");
			tree[treesize].parent = pos;
			tree[pos].kid.push_back(treesize);
			tree[treesize].col = i;
			if(tree[pos].kind == node::MY_ACTION)
				tree[treesize].kind = node::OPPO_ACTION;
			if(tree[pos].kind == node::OPPO_ACTION)
				tree[treesize].kind = node::MY_ACTION;
			tree[treesize].visit = 0;
			cal_ucb(treesize);
			//have added code here
			int row = choose(cur_board,cur_top,i,tree[pos].kind);
			if(tree[pos].kind == node::OPPO_ACTION)
			{
				if(userWin(row,i,M,N,cur_board))
					tree[treesize].gameover = true;
			}
			if(tree[pos].kind == node::MY_ACTION)
			{
				if(machineWin(row,i,M,N,cur_board))
					tree[treesize].gameover = true;
			}
			backchoose(cur_board,cur_top,i);
			//--------------------
			treesize++;
		}
	}
	assert(tree[pos].kid.size() > 0);
	int ret = tree[pos].kid[rand() % tree[pos].kid.size()];
	choose(cur_board,cur_top,tree[ret].col,tree[pos].kind);
	assert(tree[ret].visit == 0);
	//tree[ret].visit = 1;
	//cal_ucb(ret);
	//have added code here
	if(tree[ret].gameover == true)
		choose_gameover_node = true;
	//--------------------
	return ret;
}

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
	//system("pause");
}

int simulation(int pos,int print = 0)
{
	//_cprintf("simulation:%d\n",pos);
	//return 1;//for DEBUG
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
			if(print)
			{
				_cprintf("lose\n");
				_cprintf("%d %d\n",row,userturn);
				print_board(local_board);
			}
			return result;
		}
		if(isTie(N,local_top))
		{
			result = 0;
			if(print)
			{
				_cprintf("tie\n");
				_cprintf("%d %d\n",row,userturn);
				print_board(local_board);
			}
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
			if(print)
			{
				_cprintf("win\n");
				_cprintf("%d %d\n",row,myturn);
				print_board(local_board);
			}
			break;
		}
		if(isTie(N,local_top))
		{
			result = 0;
			if(print)
			{
				_cprintf("tie\n");
				_cprintf("%d %d\n",row,myturn);
				print_board(local_board);
			}
			break;
		}

		int userturn = rand() % N;
		while(local_top[userturn] <= 0)
			userturn = rand() % N;
		row = choose(local_board,local_top,userturn,node::OPPO_ACTION);
		if(userWin(row,userturn,M,N,local_board))
		{
			result = 0;
			if(print)
			{
				_cprintf("lose\n");
				_cprintf("%d %d\n",row,userturn);
				print_board(local_board);
			}
			break;
		}
		if(isTie(N,local_top))
		{
			result = 0;
			if(print)
			{
				_cprintf("tie\n");
				_cprintf("%d %d\n",row,userturn);
				print_board(local_board);
			}
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
	assert(pos > 0);
	int cur = pos;
	int flag = 1;
	//cal_ucb(cur);
	//表示最大最小节点的种类
	while(cur != tree[root].parent)
	{
		//_cprintf("back:%d\n",cur);
		assert(cur > 0);
		tree[cur].visit++;
		tree[cur].win += flag * (2*delta - 1);
		flag = -flag;
		
		//added
		//tree[cur].win += 2*delta - 1;
		/*
		if(flag == 1) tree[cur].win += delta;
		else tree[cur].win += (1 - delta);
		flag = 1 - flag;
		*/
		//-----
		//cal_ucb(cur);
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

int simple_search()//搜索三层，返回必胜，如果无就返回-1，同时给出不可走的列
{
	for(int i = 0;i < 20;++i) cannot[i] = 0;
	int** local_board = new int*[M];
	for(int i = 0;i < M;++i)
		local_board[i] = new int[N];
	copy_board(root_board,local_board);
	//复制得到board
	int* local_top = new int[N];
	for(int i = 0;i < N;++i)
		local_top[i] = root_top[i];
	//复制得到top
	int* type = new int[2100];
	for(int i = 0;i < 2100;++i)
		type[i] = 0;//0-uninit 1-必胜 2-必败 3-未知
	//必胜、必败标记
	node* simtree = new node[2100];
	//根节点为1号
	int index = 2;
	for(int i = 0;i < N;++i)
	{
		if(local_top[i] <= 0)
		{
			cannot[i] = 1;
			continue;
		}
		int index_i = index;//记录该点的索引
		simtree[index].col = i;//列号
		simtree[index].parent = 1;//父节点为1号
		type[index] = 1;//初始状态设为必胜节点
		local_board[local_top[i] - 1][i] = 2;//第i列落子
		if(machineWin(local_top[i] - 1,i,M,N,local_board))
		{
			return i;//如果走到这里已经胜利，直接返回，【正确】
		}
		local_board[local_top[i] - 1][i] = 0;//恢复
		index++;
		choose(local_board,local_top,i,2);//设置board和top
		for(int j = 0;j < N;++j)
		{
			if(local_top[j] <= 0) continue;
			int index_j = index;//记录该点的索引
			simtree[index].col = j;//列号为j
			simtree[index].parent = index_i;//父亲为之前记录的索引
			type[index] = 0;//初始状态为不胜利
			local_board[local_top[j] - 1][j] = 1;//修改board放置棋子
			if(userWin(local_top[j] - 1,j,M,N,local_board))//对方是否可以获胜，如果胜
			{
				cannot[simtree[simtree[index].parent].col] = 1;
				//type[simtree[index].parent] = 2;//则它的父亲节点必败，不可走
				/*
				index++;
				local_board[local_top[j] - 1][j] = 0;//恢复棋盘
				break;//后面无需搜索
				*/
			}
			//对方暂时不获胜
			local_board[local_top[j] - 1][j] = 0;
			choose(local_board,local_top,j,1);
			//恢复棋盘后统一修改board和top
			index++;
			for(int k = 0;k < N;++k)
			{
				if(local_top[k] <= 0) continue;
				simtree[index].col = k;//记录列号为k
				simtree[index].parent = index_j;//父亲为之前记录的索引
				local_board[local_top[k] - 1][k] = 2;//修改board放置棋子
				if(machineWin(local_top[k] - 1,k,M,N,local_board))//如果我获胜了
				{
					type[simtree[index].parent] = 1;//说明其父亲节点对我来说是必胜
					/*
					index++;
					local_board[local_top[k] - 1][k] = 0;//恢复棋盘
					break;//后面无需搜索
					*/
				}
				index++;
				local_board[local_top[k] - 1][k] = 0;//恢复棋盘
			}
			backchoose(local_board,local_top,j);//恢复棋盘
			if(type[index_j] == 0) type[simtree[index_j].parent] = 0;
			//如果该点仍然为0，说明它不是必胜点，那么他父亲也不必胜
			//否则，若所有的type[index_j]为1，说明无论对方怎么走我都可以胜，那么父亲也必胜
		}
		backchoose(local_board,local_top,i);//恢复棋盘
		if(type[index_i] == 1) return simtree[index_i].col;
	}
	//_cprintf("index:%d\n",index);
	//system("pause");
	for(int i = 2;i < 2000;++i)
	{
		if(simtree[i].parent != 1) continue;
		//_cprintf("%d\n",i);
		//_cprintf("%d\n",type[i]);
		if(type[i] == 1) return simtree[i].col;
		if(type[i] == 2) cannot[simtree[i].col] = 1;
	}
	
	for(int i = 0;i < M;++i)
		delete local_board[i];
	delete[] local_board;
	delete[] local_top;
	return -1;
}

bool used = false;
void hyf_getPoint(const int para_M, const int para_N, const int* top, int** board, 
	const int lastX, const int lastY, const int para_noX, const int para_noY,int& ans_x,int& ans_y)
{
	clock_t begin = clock();
	AllocConsole();
	_cprintf("cao");
	
	for(int i = 0;i < MAXSIZE;++i)
	{
		tree[i] = node();
	}
	for(int i = 0;i < 30;++i)
		cannot[i] = 0;
	_cprintf("cao");
	if(!used)
	{
		//AllocConsole();
		_cprintf("first");
		//srand(time(NULL));
		M = para_M;
		N = para_N;
		noX = para_noX;
		noY = para_noY;
		_cprintf("first");
		root_board = new int*[M];
		for(int i = 0;i < M;++i)
			root_board[i] = new int[N];
		cur_board = new int*[M];
		for(int i = 0;i < M;++i)
			cur_board[i] = new int[N];
		_cprintf("first");
		root_top = new int[N];
		cur_top = new int[N];
		//root = 1;
		treesize = 2;
		tree[root] = node();//1号为根节点
		tree[root].kind = node::MY_ACTION;
		tree[root].col = -1;
		used = true;
	}
	else
	{
		treesize = 2;
		tree[root] = node();//1号为根节点
		tree[root].kind = node::MY_ACTION;
		tree[root].col = -1;
		/*
		for(int i = 0;i < tree[root].kid.size();++i)
			if(tree[tree[root].kid[i]].col == lastY)
				root = tree[root].kid[i];
		*/
	}
	_cprintf("first\n");
	
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
	/*
	int cnt = 0;
	//int simple_search_result = simple_search();
	bool allone = true;
	for(int i = 0;i < N;++i)
	{
		if(cannot[i] == 0) allone = false;
		_cprintf("%d ",cannot[i]);
	}
	
	if(allone)
	{
		ans_y = rand() % N;
		while(root_top[ans_y] <= 0) ans_y = rand() % N;
		ans_x = root_top[ans_y] - 1;
		return;
	}
	*/
	while(totaltime < 4)
	{
		clock_t end = clock();
		totaltime = (double)(end - begin)/ CLOCKS_PER_SEC;
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
		//tree[pos].visit++;
		//tree[pos].win += result;
		cal_ucb(pos);
		backPropagation(pos,result);
		/*
		_cprintf("treesize:%d\n",treesize);
		_cprintf("visit:%d\n",tree[root].visit);
		for(int i = 0;i < tree[root].kid.size();++i)
		{
			int index = tree[root].kid[i];
			_cprintf("visit:%d win:%d ucb:%llf\n",tree[index].visit,tree[index].win,tree[index].ucb);
		}
		_cprintf("---------------------------------\n");
		*/
	}
	for(int i = 0;i < tree[root].kid.size();++i)
	{
		for(int j = 0;j < tree[tree[root].kid[i]].kid.size();++j)
		{
			if(tree[tree[tree[root].kid[i]].kid[j]].gameover == true)
			{
				cannot[tree[tree[root].kid[i]].col] = 1;
			}
		}
	}
	int max_col = -1;
	double max_ratio = -9999.9;
	for(int i = 0;i < N;++i)
		_cprintf("%d ",cannot[i]);
	_cprintf("\ntreesize:%d\n",treesize);
	_cprintf("visit:%d win:%d\n",tree[root].visit,tree[root].win);
	int max_index = -1;
	for(int i = 0;i < tree[root].kid.size();++i)
	{
		int index = tree[root].kid[i];
		double cur_ratio = (double)tree[index].win / (tree[index].visit);
		_cprintf("visit:%d win:%d ratio:%llf ucb:%llf\n",tree[index].visit,tree[index].win,cur_ratio,tree[index].ucb);
		if(cannot[tree[index].col] == 0 && cur_ratio > max_ratio)
		{
			max_ratio = cur_ratio;
			max_col = tree[index].col;
			max_index = i;
			//change root
			//root = tree[root].kid[i];
		}
	}
	//_cprintf("%d\n",tree[selection()].col);
	copy_board(root_board,cur_board);
	for(int i = 0;i < N;++i)
		cur_top[i] = root_top[i];
	choose(cur_board,cur_top,max_col,node::MY_ACTION);
	for(int i = 0;i < 20;++i)
		simulation(max_index,1);
	if(max_col == -1)
	{
		max_col = tree[tree[root].kid[rand() % tree[root].kid.size()]].col;
	}
	ans_y = max_col;
	ans_x = root_top[ans_y] - 1;
}
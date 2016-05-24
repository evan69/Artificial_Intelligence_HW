#ifndef MYAI_H
#define MYAI_H
#include "Judge.h"
#include "Point.h"
#include <vector>

int selection();
int expansion(int);
int simulation(int pos,int** para_board);
void backPropagation(int pos,int delta);
void hyf_getPoint(const int para_M, const int para_N, const int* top, int** board, 
	const int lastX, const int lastY, const int para_noX, const int para_noY,int& ans_x,int& ans_y);
void copy_board(int**,int**);

struct node;
/*
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
				
				//index++;
				//local_board[local_top[j] - 1][j] = 0;//恢复棋盘
				//break;//后面无需搜索
				
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
					
					//index++;
					//local_board[local_top[k] - 1][k] = 0;//恢复棋盘
					//break;//后面无需搜索
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
*/
#endif
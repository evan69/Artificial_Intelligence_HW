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

#endif
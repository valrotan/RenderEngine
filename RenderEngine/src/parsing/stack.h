#pragma once

#include "../math/renderMath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DUMMY_TRAILER '\177'
// octal ASCII code of the
// last character in the ASCII table

typedef struct snode StackNode;
struct snode {
	int p1;
	int p2;
	int p3;
	int curMat;
	StackNode *next;
};

// typedef struct snode StackNode;
// struct snode {
//	Triangle3D* data;
//	StackNode* next;
//};

StackNode *push(StackNode *stack, int p1, int p2, int p3, int curMat);
StackNode *pop(StackNode **stack);
void destroyStack(StackNode *stack);

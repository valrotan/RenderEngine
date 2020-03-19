#pragma once

#include <math.h>
#include "../math/renderMath.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DUMMY_TRAILER '\177'
// octal ASCII code of the
// last character in the ASCII table


typedef struct {
	int p1;
	int p2;
	int p3;
} STACK_DATA;

typedef struct snode StackNode;
struct snode {
	STACK_DATA pt1;
	STACK_DATA pt2;
	STACK_DATA pt3;
	StackNode* next;
};

//typedef struct snode StackNode;
//struct snode {
//	Triangle3D* data;
//	StackNode* next;
//};


StackNode* push(StackNode* stack, int p1, int p2, int p3);
Triangle3D* pop(StackNode** stack);
void destroyStack(StackNode* stack);
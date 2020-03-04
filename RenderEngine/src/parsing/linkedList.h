#pragma once

#include <math.h>
#include "../math/renderMath.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DUMMY_TRAILER '\177'
// octal ASCII code of the
// last character in the ASCII table

typedef struct snode StackNode;
struct snode {
	Triangle3D* data;
	StackNode* next;
};


StackNode* push(StackNode* stack, Triangle3D* data);
Triangle3D* pop(StackNode** stack);
void destroyStack(StackNode* stack);
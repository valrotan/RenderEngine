#include "stack.h"

/***************************************************
	Destroys dynamically allocated stack
*/
void destroyStack(StackNode* stack) {
	while (stack) {
		free(pop(&stack));
	}
}


/***************************************************
	Stack Insert: insert in the beginning
*/
StackNode* push(StackNode* stack, Triangle3D* data)
{
	StackNode* pnew;

	pnew = (StackNode*) malloc(sizeof(StackNode));
	if (!pnew)
	{
		printf("... error in push!\n");
		exit(1);
	}
	pnew->data = data;
	pnew->next = stack;
	stack = pnew;

	return stack;
}


/***************************************************
	Stack Delete: delete the first node
*/
Triangle3D* pop(StackNode** stack)
{
	StackNode* first;
	if (*stack == NULL) return NULL;
	first = *stack;
	*stack = (*stack)->next;
	first->next = NULL;

	return first->data;
}
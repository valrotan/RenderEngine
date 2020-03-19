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
StackNode* push(StackNode* stack, int p1, int p2, int p3)
{
	StackNode* pnew;

	pnew = (StackNode*) malloc(sizeof(StackNode));
	if (!pnew)
	{
		printf("... error in push!\n");
		exit(1);
	}
	pnew->p1 = p1;
	pnew->p2 = p2;
	pnew->p3 = p3;
	pnew->next = stack;
	stack = pnew;
	return stack;
}


/***************************************************
	Stack Delete: delete the first node
*/
StackNode* pop(StackNode** stack)
{
	StackNode* first;
	if (*stack == NULL) return NULL;
	first = *stack;
	*stack = (*stack)->next;
	first->next = NULL;

	return first;
}
#include "Node.h"

Node * createNode(const char * data, Node * prev, Node * next)
{
	Node * new_node = (Node *)malloc(sizeof(Node));

	new_node->init = initNode;
	new_node->init(new_node, data, prev, next);

	return new_node;
}

void initNode(Node * node, const char * data, Node * prev, Node * next)
{
	//int 저장
	node->data = (char *)malloc(sizeof(char) * 4);
	if(node->data == NULL)
		error_handling("Node.c node->data malloc() error");

	*((int *)node->data) = *((int *)data);
	node->prev = prev;
	node->next = next;
}

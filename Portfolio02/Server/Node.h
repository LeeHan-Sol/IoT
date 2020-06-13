#ifndef NODE_H
#define NODE_H

#include "IoT.h"

typedef struct Node
{
	char * data;
	struct Node * prev;
	struct Node * next;

	void (*init)(struct Node *, const char *, struct Node *, struct Node *);
}Node;

Node * createNode(const char *, Node *, Node *);
void initNode(Node *, const char *, Node *, Node *);

#endif

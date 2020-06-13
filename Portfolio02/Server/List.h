#ifndef LIST_H
#define LIST_H

#include "IoT.h"
#include "Node.h"

typedef struct List
{
	int size;
	Node * head;
	Node * tail;

	void (*init)(struct List *);
	int (*isEmpty)(const struct List *);
	void (*display)(const struct List *);
	void (*pushRear)(struct List *, const int);
	void (*popItem)(struct List *, Node *);
	Node * (*peekRear)(const struct List *);
}List;

List * createList();
void initList(List *);
int isEmptyList(const List *);
void displayList(const List *);
void pushRear(List *, const int);
void popItem(List *, Node *);
Node * peekRear(const List *);

#endif

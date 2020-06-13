#include "List.h"

List * createList()
{
	List * new_list = (List *)malloc(sizeof(List));
	
	new_list->init = initList;
	new_list->init(new_list);

	return new_list;
}

void initList(List * list)
{
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;

	list->isEmpty = isEmptyList;
	list->display = displayList;
	list->pushRear = pushRear;
	list->popItem = popItem;
	list->peekRear = peekRear;
}

int isEmptyList(const List * list)
{
	return list->size == 0 ? 1 : 0;
}

void displayList(const List * list)
{
	if(list->isEmpty(list))
	{
		return ;
	}

	fputs("List 항목\n", stdout);
	for(Node * temp = list->head; temp != NULL; temp = temp->next)
	{
		fprintf(stdout, "%4d", *((int *)temp->data));
	}
	fputc('\n', stdout);

	return ;
}

void pushRear(List * list, const int data)
{
	if(list->isEmpty(list))
	{
		list->head = createNode((char *)&data, NULL, NULL);
		list->tail = list->head;
		list->size++;

		return ;
	}

	list->tail->next = createNode((char *)&data, list->tail, NULL);
	list->tail = list->tail->next;
	list->size++;

	return ;
}

void popItem(List * list, Node * node)
{
	if(list->isEmpty(list))
	{
		return ;
	}

	if(node->prev != NULL)
	{
		node->prev->next = node->next;
	}
	
	if(node->next != NULL)
	{
		node->next->prev = node->prev;
	}

	free(node);
	list->size--;
	return ;
}

Node * peekRear(const List * list)
{
	if(list->isEmpty(list))
	{
		return (Node *)NULL;
	}

	return list->tail;
}

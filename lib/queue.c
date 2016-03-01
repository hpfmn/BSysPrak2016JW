#include "queue.h"

q_elem* getLastElem(q_elem *elem)
{
	if(elem==0)
		return 0; while(elem->next != 0)
		elem = elem->next;
	return elem;
}


int insertElem(q_elem *new, q_elem **list)
{
	if(new==0)
		return -1;
	if((*list)==0)
	{
		(*list)=new;
		new->prev=0;
		new->next=0;
		return 0;
	}

	(*list)->prev = new;
	new->next = *list;
	new->prev = 0;
	(*list)=new;
	return 0;
}

int appendElem(q_elem *new, q_elem **list)
{
	if(new==0)
		return -1;
	if((*list)==0)
	{
		(*list)=new;
		new->prev=0;
		new->next=0;
		return 0;
	}

	q_elem *endOfList = getLastElem(*list);

	if(endOfList == 0)
		return -2;

	endOfList->next = new;
	new->prev = endOfList;
	new->next = 0;

	return 0;
}

int insertAtElem(q_elem *new, q_elem *at)
{
	if(new==0 || at==0)
		return -1;

	q_elem* tmp=at->next;
	at->next = new;
	new->next = tmp;
	new->prev = at;
	if(tmp)
		tmp->prev = new;
	return 0;
}

int queueOut(q_elem *new, q_elem **list)
{
	if(new == (*list))
		(*list)=new->next;
	if(new->prev)
		new->prev->next=new->next;
	if(new->next)
		new->next->prev=new->prev;
	return 0;
}

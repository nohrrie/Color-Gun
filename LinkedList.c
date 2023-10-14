#include "LinkedList.h"
#include "DMLib.h"


#include <stdlib.h>


ObjNode* create_node(void) {
	ObjNode* temp;
	temp = malloc(sizeof(struct ObjectList));
	temp->next = NULL;
	temp->last = NULL;
	
	return temp;
}


ObjNode* add_node(ObjNode*head, GameObject obj) {
	ObjNode* temp, *p, *last;
	temp = create_node();
	temp->data.object = obj;

	if (head == NULL) {
		head = temp;
	}
	else {
		p = head;
		last = head;
		while (p->next != NULL) {
			p = p->next;
			p->last = last;
			last = p;
		}
		p->next = temp;
		
	}
	return *&temp;
}

int ListSize(ObjNode* head) {
	int size = 0;

	ObjNode* temp = head;
	if (temp != NULL) size++;
	else return size;
	while (temp->next != NULL) {
		size++;
		temp = temp->next;
	}

	return size;
}

void FreeList(ObjNode* head)
{
	ObjNode* tmp;

	while (head != NULL)
	{
		tmp = head;
		head = head->next;
		free(tmp);
	}

}


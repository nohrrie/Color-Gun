#pragma once
#include "DMLib.h"


typedef struct ObjectList {

	union {
		GameObject object;
		CustomText text;
		CustomButton button;
	} data;
	enum DataType type;
	struct ObjectList* next;
	struct ObjectList* last;
} ObjNode;


ObjNode* create_node(void);


ObjNode* add_node(ObjNode* head, GameObject obj);


int ListSize(ObjNode* head);

void FreeList(ObjNode* head);
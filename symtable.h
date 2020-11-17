/***************************
 * PROJECT: 
 *  IFJ20 - Compiler for imperative programming language IFJ20
 * 
 * UNIVERSITY: 
 *  Faculty of Information Technology, Brno University of Technology
 * 
 * FILE:
 *  symtable.h
 * 
 * DESCRIPTION:
 *  Header file for symtable.c
 * 
 * AUTHOR:
 *  Žovinec Martin      <xzovin00@stud.fit.vutbr.cz>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "str.h"
#include "error.h"

#define KW_FUNC       1
#define T_INT        30
#define T_FLOAT      31
#define T_STRING     32 

#define PointerError 	-1
#define AllGood 		0
#define BadType 		1
#define BadOrder 		2
#define BadTypeAndOrder 3

#define MAX_LIST_LENGHT 100

#ifndef SYMTABLE_H
#define SYMTABLE_H

/* Binary tree node structure */

typedef struct varNode{
    int type;
	string name;

	struct varNode*LPtr;
	struct varNode*RPtr;
} *varNode;

typedef struct funNode{
	string name;

	bool isDeclared;
	bool isCalled;

	struct funList *parameters;
	struct funList *returnCodes;

	struct funNode *LPtr;
	struct funNode *RPtr;
} *funNode;

typedef struct funListElement{
	int type;
	int order;

	struct funListElement *NextPtr;
} *funListElement;

typedef struct funList{
    funListElement First;
	int elementCount;
} funList;

/* Prototypes of variable operations */

void BSTInit   	(varNode*);
varNode BSTSearch(varNode, string);
void BSTInsert 	(varNode*, string, int);
void BSTDispose	(varNode*);
bool isDeclared (varNode, string);
int  getType	(varNode, string);

/* Prototypes of function operations */

void funInit (funNode *RootPtr);
funNode funSearch (funNode RootPtr, string Key);
void addFunToTree(funNode *RootPtr, string Key, bool Declaration, bool Call);
void funDisposeTree (funNode *RootPtr);
void addFunCall(funNode *RootPtr, string Key);
void addFunDec(funNode *RootPtr, string Key);
int addParam(funNode RootPtr, string Key, int parameterType, int parameterOrder);
int addReturn(funNode RootPtr, string Key, int returnType, int returnOrder);
int isFunCallDec(funNode RootPtr);

/*Prototypes of function list operations*/

void funListInit (funList *L);
void funListAdd (funList *L, int val, int order);
funListElement funListSearch (funList *L, int order);
void funListDelete(funList *L);
int processListElement(funNode *RootPtr, string Key, int type, int order);
int funListElementCheck (funListElement ListElement, int Type, int Order);

/* Prototypes of datastructure printing functions */

void printVarTree(varNode);
void printVarTree2(varNode, char* sufix, char fromdir);
void printFunList(funList TL);
void printFunTree(funNode TempTree);
void printFunTree2(funNode TempTree, char* sufix, char fromdir);

#endif /*SYMTABLE_H*/

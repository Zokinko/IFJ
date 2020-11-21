/***************************
 * PROJECT: 
 *  IFJ20 - Compiler for imperative programming language IFJ20
 * 
 * UNIVERSITY: 
 *  Faculty of Information Technology, Brno University of Technology
 * 
 * FILE:
 *  symtable.c
 * 
 * DESCRIPTION:
 *  Implementation of symbol table using binary tree
 * 
 * AUTHOR:
 *  Žovinec Martin      <xzovin00@stud.fit.vutbr.cz>
 */

#include "symtable.h"
#include "error.h"

// TODO returnTypes in functions

/********************************************************/
/*************** Variable tree operations ***************/

/**
 * @brief Initializes the variable BST by setting its pointer to NULL.
 * 
 * @param RootPtr pointer to the variable BST
 */
void BSTInit (varNode *RootPtr) {
	*RootPtr = NULL;
}

/**
 * @brief Finds if given variable was declared.
 * 
 * @param RootPtr pointer to the variable BST
 * @param Key name of the searched variable
 * 
 * @return Bool value true if variable is in the BST, false otherwise
 */
bool isDeclared (varNode RootPtr, string Key) {
	if(BSTSearch(RootPtr, Key) == NULL)
		return false;
	return true;
}

/**
 * @brief Finds the variable and returns its type.
 * 
 * @param RootPtr pointer to the variable BST
 * @param Key name of the searched variable
 * 
 * @return int value of the variables type
 */
int getType (varNode RootPtr, string Key) {
	if(!RootPtr)
		return 666;

	else if ( strCmpString(&Key, &(RootPtr->name)) < 0)
		return getType (RootPtr->LPtr, Key);

	else if ( strCmpString(&Key, &(RootPtr->name)) > 0)
		return getType (RootPtr->RPtr, Key);

	return RootPtr->varStack->type;
}

/**
 * @brief Finds the variable in the BST.
 * 
 * @param RootPtr pointer to the variable binary BST
 * @param Key name of the searched variable
 * 
 * @return structure varNode of the searched variable
 */
varNode BSTSearch (varNode RootPtr, string Key)	{

	if(!RootPtr)
		return NULL;

	else if ( strCmpString(&Key, &(RootPtr->name)) < 0)
		return BSTSearch (RootPtr->LPtr, Key);

	else if ( strCmpString(&Key, &(RootPtr->name)) > 0)
		return BSTSearch (RootPtr->RPtr, Key);

	return RootPtr;
}

/**
 * @brief Adds a variable to the variable BST, prints error if the variable is already declared in the same scope.
 * 
 * @param RootPtr pointer to the variable BST
 * @param Key name of the new variable
 * @param Type type of the new variable
 * @param Scope current scope in the parser
 */
void BSTInsert (varNode *RootPtr, string Key, int Type, int scope)	{

	if( !*RootPtr ) {
		(*RootPtr) = (varNode)malloc(sizeof(struct varNode));
		if(RootPtr == NULL)
			return;

		(*RootPtr)->varStack = NULL;
		stackPush(RootPtr,Type,scope);
		strInit(&(*RootPtr)->name);
		
		strCopyString(&((*RootPtr)->name),&Key);

		(*RootPtr)->LPtr = (*RootPtr)->RPtr = NULL;
		return;
	}

	if ( strCmpString(&Key, &((*RootPtr)->name)) < 0) {
		BSTInsert ( &((*RootPtr)->LPtr), Key, Type, scope);
		return;
	}

	if ( strCmpString(&Key, &((*RootPtr)->name)) > 0) {
		BSTInsert ( &((*RootPtr)->RPtr), Key, Type, scope);
		return;
	}

	// Overwriting of the values, if the var exists
	if(scope > (*RootPtr)->varStack->scope)
		stackPush(RootPtr,Type,scope);
	else{
		fprintf(stderr,"ERROR 3: Redefinition of a variable in the same scope [%s]\n", Key.str);
		exit(3);
	}
}

/**
 * @brief Frees the whole variable BST
 * 
 * @param RootPtr pointer to the variable BST
 */
void BSTDispose (varNode *RootPtr) {

	if( *RootPtr != NULL){
        BSTDispose(&((*RootPtr)->LPtr));
        BSTDispose(&((*RootPtr)->RPtr));

		stackDelete(RootPtr);
		strFree(&((*RootPtr)->name));
        free(*RootPtr);
        *RootPtr = NULL;
    }
}

/**
 * @brief Copies and frees the rightmost node from the left branch, then with the copied content overwrites the contents of the node we want to delete.
 * 
 * @param PtrReplaced node we want to replace
 * @param leftBranch left branch node where we will search for the rightmost node to replace PtrReplaced with
 */
void ReplaceByRightmost (varNode PtrReplaced, varNode *leftBranch){

	if(!*leftBranch)
		return;

	if(( *leftBranch)->RPtr)
		ReplaceByRightmost(PtrReplaced, &((*leftBranch)->RPtr ));
	else{
		varNode delete_me = (*leftBranch);
		
		strClear(&(PtrReplaced->name));
		stackDelete(&PtrReplaced);

		strCopyString(&(PtrReplaced->name),&(delete_me->name));
		PtrReplaced->varStack = delete_me ->varStack;
		
		(*leftBranch) = (*leftBranch)->LPtr;
		
		strFree(&(delete_me->name));
		free(delete_me);
	}
}

/**
 * @brief Deletes the variable Key from the BSTD.
 * 
 * @param RootPtr pointer to the variable BST
 * @param Key name of the variable we want to delete
 */
void BSTDelete (varNode *RootPtr, string Key){
	if(!*RootPtr){
		return;
	}
	if (strCmpString(&Key, &((*RootPtr)->name)) < 0){
			BSTDelete( &((*RootPtr)->LPtr), Key);

	}else if (strCmpString(&Key, &((*RootPtr)->name)) > 0){
		BSTDelete( &((*RootPtr)->RPtr), Key);

	}else if ((*RootPtr)->LPtr && (*RootPtr)->RPtr)
		ReplaceByRightmost((*RootPtr), &((*RootPtr )->LPtr));

	else{
		varNode delete_me = (*RootPtr);

		if((*RootPtr)->LPtr)
			 *RootPtr = (*RootPtr)->LPtr;
		else
			 *RootPtr = (*RootPtr)->RPtr;
			 
		stackDelete(&delete_me);
		strFree(&(delete_me->name));
		free(delete_me);
	}
}

/**
 * @brief Searches the whole tree, pops any node Stacks which have scope value higher than newScope, if there is no value left, then it deletes the node.
 * 
 * @param RootPtr pointer to the variable BST
 * @param newScope new lower scope which we compare with the old scope
 */
void BSTScopeDelete(varNode *RootPtr, int newScope){
	if(!*RootPtr)
		return;

	BSTScopeDelete(&((*RootPtr)->LPtr), newScope);
	BSTScopeDelete(&((*RootPtr)->RPtr), newScope);

	while ((*RootPtr)->varStack != NULL && (*RootPtr)->varStack->scope > newScope ){
		stackPop(RootPtr);
	}
	
	if ((*RootPtr)->varStack == NULL){
		BSTDelete(RootPtr, (*RootPtr)->name);
	}
}

/*********************************************************/
/*************** Var tree stack operations ***************/

/**
 * @brief Pops the top values on the stack, so it now has the previous values on top.
 * 
 * @param varTree pointer to the variable BST
 */
void stackPop(varNode* varTree){
	if ((*varTree)->varStack != NULL){
		varStackElement temp = (*varTree)->varStack;
		(*varTree)->varStack = (*varTree)->varStack->previousElement;
		free(temp);
	}
}

/**
 * @brief Pushes the variable stack so the top now has new values and points to the previous element.
 * 
 * @param varTree pointer to the variable BST
 * @param type type of the variable
 * @param scope scope of the variable
 */
void stackPush(varNode* varTree, int type, int scope){
	varStackElement newElement = (varStackElement) malloc(sizeof(struct varStackElement));
	newElement->type = type;
	newElement->scope = scope;

	newElement->previousElement = (*varTree)->varStack;

	(*varTree)->varStack = newElement;
}

/**
 * @brief Frees the whole stack.
 * 
 * @param varTree pointer to the variable BST
 */
void stackDelete(varNode *varTree){
	while ((*varTree)->varStack != NULL){
		stackPop(varTree);
	}
}

/********************************************************/
/*************** Function tree operations ***************/

/**
 * @brief Initializes the function BST by setting its pointer to NULL.
 * 
 * @param RootPtr pointer to the function BST
 */
void funInit (funNode *RootPtr) {
	*RootPtr = NULL;
}

/**
 * @brief Searches the function BST for function for key.
 * 
 * @param RootPtr pointer to the function BST
 * @param name name of the function we are looking for
 * 
 * @return funNode with searched name or NULL if it is not in the BST
 */
funNode *funSearch (funNode *RootPtr, string Key)	{

	if(!*RootPtr)
		return NULL;

	else if (strCmpString(&Key, &((*RootPtr)->name)) < 0)
		return funSearch(&((*RootPtr)->LPtr), Key);

	else if (strCmpString(&Key, &((*RootPtr)->name)) > 0)
		return funSearch(&((*RootPtr)->RPtr), Key);

	return RootPtr;
}

/**
 * @brief Adds the function to the function BST and inicializes all of its elements.
 * 
 * @param RootPtr pointer to the function BST
 * @param name name of the function we are looking for
 */
void addFunToTree(funNode *RootPtr, string Key){
	// function will be inserted into the tree
	if(!*RootPtr){
		(*RootPtr) = (funNode)malloc(sizeof(struct funNode));
		if(RootPtr == NULL)
			return;

		(*RootPtr)->parameters = malloc(sizeof(struct funList));
		(*RootPtr)->parameters->First = NULL;
		(*RootPtr)->parameters->elementCount = 0;

		(*RootPtr)->returnCodes = malloc(sizeof(struct funList));
		(*RootPtr)->returnCodes->First = NULL;
		(*RootPtr)->returnCodes->elementCount = 0;
		
		strInit(&((*RootPtr))->name);
		strCopyString(&((*RootPtr)->name),&Key);

		(*RootPtr)->LPtr = (*RootPtr)->RPtr = NULL;
		return;
	}

	if (strCmpString(&Key, &((*RootPtr)->name)) < 0) {
		addFunToTree ( &((*RootPtr)->LPtr), Key);
		return;
	}else if (strCmpString(&Key, &((*RootPtr)->name)) > 0) {
		addFunToTree ( &((*RootPtr)->RPtr), Key);
		return;
	}
}

/**
 * @brief Finds the function with given name in the function BST then checks for errors and changes its contents. 
 * 
 * @param RootPtr pointer to the function BST
 * @param name name of the function we are looking for
 * @param Declaration true if the function was declared, used for checking if the function isn't declared twice
 * @param Call true if the fuction was called, used in isFunCallDec() to check if the function wasn't called without being declared
 * @param paramCount number of function parameters, used for checking if the function isn't called or declared with different amount of parameters in other instances
 * @param returnCount number of function return types, used for checking if the function isn't called or declared with different amount of return types in other instances
 */
void funActualize (funNode *RootPtr, string Key, bool Declaration, bool Call, int paramCount, int returnCount){
	if(!*RootPtr){
		//printf("Function %s was not found!\n",Key.str);
		return;
	}

	if (strCmpString(&Key, &((*RootPtr)->name)) < 0) {
		funActualize( &((*RootPtr)->LPtr), Key, Declaration, Call, paramCount, returnCount);
		return;
	}else if (strCmpString(&Key, &((*RootPtr)->name)) > 0) {
		funActualize( &((*RootPtr)->RPtr), Key, Declaration, Call, paramCount, returnCount);
		return;
	}

	// function was found

	if((*RootPtr)->parameters->elementCount != paramCount){
		fprintf(stderr,"ERROR 6: Function has wrong amount of parameters [%s]\n", Key.str);
		//exit(6);	// TODO check type
	}

	if((*RootPtr)->returnCodes->elementCount != returnCount){
		fprintf(stderr,"ERROR 6: Function has wrong amount of return types [%s]\n", Key.str);
		//exit(3);	// TODO check type
	}

	if(Declaration == true && ((*RootPtr)->isDeclared == true)){
		fprintf(stderr,"ERROR 3: Redefinition of function [%s]\n", Key.str);
		exit(3);
    }

	if(Declaration == true && !((*RootPtr)->isDeclared == true)){
		(*RootPtr)->isDeclared = true;
	}

	if (Call == true && !((*RootPtr)->isCalled ==true)) {
		(*RootPtr)->isCalled = true;
	}
}

/**
 * @brief Frees the whole function BST and the elements of each node.
 * 
 * @param RootPtr pointer to the function BST
 */
void funDisposeTree (funNode *RootPtr) {

	if( *RootPtr != NULL){
        funDisposeTree(&((*RootPtr)->LPtr));
        funDisposeTree(&((*RootPtr)->RPtr));

		strFree(&((*RootPtr)->name));
		funListDelete((*RootPtr)->parameters);
		funListDelete((*RootPtr)->returnCodes);

        free(*RootPtr);

        *RootPtr = NULL;
    }
}

/**
 * @brief Changes the bool value Call of the function to true, checks the variable tree if there is no variable with the same name as the functions.
 * 
 * @param RootPtr pointer to the function BST
 * @param Key name of the searched function
 * @param varTree variable tree for error checking
 * @param paramCount number of function parameters, used by funActualize() for error chcecking
 * @param returnCount number of function return types, used by funActualize() for error chcecking
 */
void addFunCall(funNode *RootPtr, string Key, varNode varTree, int paramCount, int returnCount){
	if(BSTSearch (varTree, Key)){
		fprintf(stderr,"Error 3: Function is also a variable in the same scope! [%s]\n", Key.str);
		exit(3);
	}
	funActualize(RootPtr, Key, false, true, paramCount, returnCount);
}

/**
 * @brief Changes the bool value Declaration of the function to true.
 * 
 * @param RootPtr pointer to the function BST
 * @param Key name of the searched function
 * @param paramCount number of function parameters, used by funActualize() for error chcecking
 * @param returnCount number of function return types, used by funActualize() for error chcecking
 */
void addFunDec(funNode *RootPtr, string Key, int paramCount, int returnCount){
	funActualize(RootPtr, Key, true, false, paramCount, returnCount);
}

/**
 * @brief Adds a parameter to the function Key in the BST. If the function was already declared or called, checks the parameter for errors instead.
 * 
 * @param RootPtr pointer to the function BST
 * @param Key name of the searched function
 * @param parameterType type of the parameter
 * @param parameterOrder order of the parameter, used for error checking
 */
void addParam(funNode *RootPtr, string Key, int parameterType, int parameterOrder){
	funNode *tempTree;
	tempTree = funSearch(RootPtr, Key);
	if((*tempTree)->isCalled == false && (*tempTree)->isDeclared == false ){
		funListAdd((*tempTree)->parameters, parameterType, parameterOrder);
	}else{
		checkListElement((*tempTree)->parameters,parameterType,parameterOrder);
	}
}

/**
 * @brief Adds a return type to the function Key in the BST. If the function was already declared or called, checks the return type for errors instead.
 * 
 * @param RootPtr pointer to the function BST
 * @param Key name of the searched function
 * @param returnType type of the return
 * @param returnOrder order of the return, used for error checking
 */
void addReturn(funNode *RootPtr, string Key, int returnType, int returnOrder){
	funNode *tempTree;
	tempTree = funSearch(RootPtr, Key);
	if((*tempTree)->isCalled == false && (*tempTree)->isDeclared == false ){
		funListAdd((*tempTree)->parameters, returnType, returnOrder);
	}else{
		checkListElement((*tempTree)->parameters, returnType, returnOrder);
	}
}

/**
 * @brief Recursively goes through the whole function BST and checks if each function was not Called without declaration.
 * 
 * @param RootPtr pointer to the function BST
 */
int isFunCallDec(funNode RootPtr){
	if(RootPtr != NULL){
		isFunCallDec(RootPtr->LPtr);
		isFunCallDec(RootPtr->RPtr);

		if (RootPtr->isCalled && !RootPtr->isDeclared ){
			fprintf(stderr,"Error - the function %s is called but not declared!\n", RootPtr->name.str);
			exit(3); 
		}
		return 0;
	}
	return 0;
}

// int parCount(funNode RootPtr,string name){
// 	funNode *temp;
// 	temp = funSearch ( &RootPtr,  name);	
// 	return (*temp)->parameters->elementCount;
// }

// int retCount(funNode RootPtr, string name){
// 	funNode *temp;
// 	temp = funSearch ( &RootPtr,  name);	
// 	return (*temp)->returnCodes->elementCount;
// }

/********************************************************/
/*************** Function list operations ***************/


/**
 * @brief Initializes the parameter/return list.
 * 
 * @param list pointer to the function list
 */
void funListInit (funList *list) {
	list->First = NULL;
	list->elementCount = 0;
}

/**
 * @brief Appends a new funListElement at the end of the list.
 * 
 * @param list pointer to the function list
 * @param type type of the element
 * @param order order of the element
 */
void funListAdd (funList *list, int type, int order){	
	funListElement temp = list->First;

	if (list->First != NULL){
		while (temp->NextPtr != NULL){
			temp = temp->NextPtr;
		}
	}
	
	funListElement listElement = (funListElement) malloc(sizeof(struct funListElement) );
	if( listElement == NULL ){
		return;
	}

	listElement->type = type;	
	listElement->order = order;
	listElement->NextPtr = NULL;

	list->elementCount++;

	if (list->First == NULL){
		list->First = listElement;
	} else{
		temp->NextPtr = listElement;
	}
}

/**
 * @brief Searches the list structure until it finds the element at the position order.
 * 
 * @param list pointer to the function list
 * @param order order of the element
 * 
 * @return found element of the list or NULL if it is not found
 */
funListElement funListSearch (funList *list, int order){
	
	if( list->First == NULL){
		return NULL;
	}
	
	funListElement temp = list->First;

	for (int elementNum = 1; temp != NULL; elementNum++){
		
		if (elementNum == order){
			return temp;
		}
		temp = temp->NextPtr;
	}

	return NULL;
}

/**
 * @brief Frees the whole function list structure.
 * 
 * @param list pointer to the function list
 */
void funListDelete(funList *list){
	funListElement temp = list->First;
	while (temp != NULL){
		temp = temp->NextPtr;
		free(list->First);
		list->First = temp;
	}
}

/**
 * @brief Checks if the given element has a given type.
 * 
 * @param list pointer to the function list
 * @param type type to compare with elements type
 * @param order position of the element we want to check in the list
 */
void checkListElement(funList *list, int type, int order){
	funListElement tempListElement;
	tempListElement = funListSearch (list, order);
	
	if (tempListElement == NULL){
		return;
	}
	
	if (tempListElement->type != type){
		fprintf(stderr,"Error 6: Wrong return/parameter type of a function\n");	
		// TODO add exit(6); for testing purposes is not here now
	}
}

/*********************************************************************/
/*************** Functions for printing datastructures ***************/


/**
 * @brief Debugging function which transfers type constants to strings for printing.
 * 
 * @param typeNum type to convert
 * 
 * @return converted type as a string
 */
char* printType(int typeNum){
	if (typeNum == T_INT){
		return "int";
	}else if (typeNum == T_FLOAT){
		return "float";
	}else if (typeNum == T_STRING){
		return "string";
	}else{
		return "Unknown type";
	}
}

/**
 * @brief Magical function for printing binary trees. Taken from IAL testing file c401-test.c . Used only for debugging purposes. Slightly modified.
 * 
 */
void printVarTree2(varNode TempTree, char* sufix, char fromdir){
    if (TempTree != NULL){
		char* suf2 = (char*) malloc(strlen(sufix) + 4);
		strcpy(suf2, sufix);

        if (fromdir == 'L'){
	   		suf2 = strcat(suf2, "  |");
	   		printf("%s\n", suf2);
		}else
	   		suf2 = strcat(suf2, "   ");

		printVarTree2(TempTree->RPtr, suf2, 'R');
        printf("%s  +-[%s,%s,S%d]\n", sufix,  TempTree->name.str, printType(TempTree->varStack->type),TempTree->varStack->scope);
		strcpy(suf2, sufix);

        if (fromdir == 'R')
	   		suf2 = strcat(suf2, "  |");	
		else
	   		suf2 = strcat(suf2, "   ");
		printVarTree2(TempTree->LPtr, suf2, 'L');

		if (fromdir == 'R') printf("%s\n", suf2);
			free(suf2);
    }
}

/**
 * @brief Magical function for printing binary trees. Taken from IAL testing file c401-test.c . Used only for debugging purposes. Slightly modified.
 * 
 * @param TempTree variable BST which is to be printed
 */
void printVarTree(varNode TempTree){
  	printf("Struktura binarniho stromu:\n");
  	printf("\n");

  	if (TempTree != NULL)
     	printVarTree2(TempTree, "", 'X');
  	else
     printf("strom je prazdny\n");

  	printf("\n");
  	printf("=================================================\n");
} 

/**
 * @brief Magical function for printing lists. Taken from IAL testing file c206-test.c . Used only for debugging purposes. Slightly modified.
 * 
 * @param TL list which is to be printed
 */
void printFunList(funList TL){
	
	funList TempList=TL;
	int CurrListLength = 0;
	printf("\t -----------------\n");
	if(TempList.First == NULL ){
		printf("\t  list je prazdny\n");
	}
	while ((TempList.First!=NULL) && (CurrListLength<MAX_LIST_LENGHT))	{
		
		printf("\t |%s, order %d|\n", printType(TempList.First->type),TempList.First->order);
		
		TempList.First=TempList.First->NextPtr;
		CurrListLength++;
	}
    if (CurrListLength>=MAX_LIST_LENGHT){
        printf("List exceeded maximum length!\n");
	}
	printf("\t -----------------\n");     
}

/**
 * @brief Magical function for printing function BST. Taken from IAL testing file c401-test.c . Used only for debugging purposes. Slightly modified.
 * 
 * @param TempTree function BST which is to be printed
 */
void printFunTree(funNode TempTree){
  	printf("Struktura binarniho stromu:\n");
  	printf("\n");

  	if (TempTree != NULL)
     	printFunTree2(TempTree, "", 'X');
  	else
     printf("strom je prazdny\n");

  	printf("\n");
  	printf("=================================================\n");
} 

/**
 * @brief Magical function for printing function BST. Taken from IAL testing file c401-test.c . Used only for debugging purposes. Lightly modified.
 */
void printFunTree2(funNode TempTree, char* sufix, char fromdir){
	funListElement tempElement;
    if (TempTree != NULL){
		char* suf2 = (char*) malloc(strlen(sufix) + 4);
		strcpy(suf2, sufix);

        if (fromdir == 'L'){
	   		suf2 = strcat(suf2, "  |");
	   		printf("%s\n", suf2);
		}else
	   		suf2 = strcat(suf2, "   ");

		printFunTree2(TempTree->RPtr, suf2, 'R');
        printf("%s  +-[%s,D%d,C%d", sufix,  TempTree->name.str,TempTree->isDeclared,TempTree->isCalled);
		for ( int i = 1; (tempElement = funListSearch(TempTree->parameters, i)) != NULL; i++){
			printf(",P %s",printType(tempElement->type));
		}
		for ( int i = 1; (tempElement = funListSearch(TempTree->returnCodes, i)) != NULL; i++){
			printf(",R %s",printType(tempElement->type));
		}
		printf("]\n");

		strcpy(suf2, sufix);

        if (fromdir == 'R')
	   		suf2 = strcat(suf2, "  |");	
		else
	   		suf2 = strcat(suf2, "   ");
		printFunTree2(TempTree->LPtr, suf2, 'L');

		if (fromdir == 'R') printf("%s\n", suf2);
			free(suf2);
    }
}
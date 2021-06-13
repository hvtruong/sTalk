#include "list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static List arrOfLists[LIST_MAX_NUM_HEADS];
static Node arrOfNodes[LIST_MAX_NUM_NODES];

static int nodesUsedCount = 0;
static int listsUsedCount = 0;
static Node* freeNodesPtr; // points to start of free nodes list, if NULL, then no free nodes
static List* freeListsPtr; // points to start of free nodes list, if NULL, then no free nodes
static bool listCreated = false;


//helper funcitons used to pass variables (used only during testing)
/*int returnNodesUsedCount(){
	return nodesUsedCount;
}

int returnListsUsedCount(){
	return listsUsedCount;
}

Node* returnFreeNodesPtr(){
	return freeNodesPtr;
}

List* returnFreeListsPtr(){
	return freeListsPtr;
}

List* returnFreshLists(){
	return arrOfLists;
}

Node* returnFreshNodes(){
	return arrOfNodes;
}*/

// private helper function 
// checks if there are avaialbe free nodes, takes a free node from front of list, 
// updates the start of the free nodes list, updates the usednodecount, update inUse
// return pointer to node to use on success, returns null on fail
static Node* Take_node(){ // TESTED 

	if(nodesUsedCount >= LIST_MAX_NUM_NODES){
		return NULL;
	}

	Node* returnNode = freeNodesPtr;

	freeNodesPtr = freeNodesPtr->next;
	returnNode->next = NULL;
	returnNode->prev = NULL;
	returnNode->data = NULL;
	returnNode->inUse = true;

	nodesUsedCount += 1;

	return returnNode;
}

// private helper function 
// replaces a used node to front of list, 
// updates the start of the free nodes list, update the usedNodeCount, update inUse, free data
// node should be already not useful to the list, unlinked to everything
// return 0 on success, -1 on fail
static void Replace_node(Node* replaceThisNode){

	//assert(replaceThisNode != NULL);

	//clean up the node
	replaceThisNode->data = NULL;
	replaceThisNode->prev = NULL;
	replaceThisNode->next = NULL;
	replaceThisNode->inUse = false;

	// update the node count
	nodesUsedCount -= 1;

	//add node back to top of stack, connect the nodes,update the ptr,
	replaceThisNode->next = freeNodesPtr;
	freeNodesPtr = replaceThisNode;

	return;
}

// private helper function 
// replaces a used node to front of list, 
// updates the start of the free nodes list, update the usedNodeCount, update inUse, free data
// node should be already not useful to the list, unlinked to everything
// return 0 on success, -1 on fail
static void Replace_list(List* replaceThisList){
	//assert(replaceThisList != NULL);

	//clean up the list
	replaceThisList->head = NULL;
	replaceThisList->tail = NULL;
	replaceThisList->current = NULL;
	replaceThisList->oob = LIST_OOB_END;
	replaceThisList->count = 0;
	replaceThisList->inUse = false;
	replaceThisList->nextList = NULL;

	//update list count
	listsUsedCount -= 1;

	// add list back to stack 
	replaceThisList->nextList = freeListsPtr;
	freeListsPtr = replaceThisList;

	return;
}















// Makes a new, empty list, and returns its reference on success. 
// Returns a NULL pointer on failure.
List* List_create(){  // TESTED 

	//initilize nodes and lists for first time calling list_create()
	if(listCreated == false){

		// initilize array of nodes
		for(int i = 0; i < LIST_MAX_NUM_NODES; i++){
			arrOfNodes[i].data = NULL;
			arrOfNodes[i].next = NULL;
			arrOfNodes[i].prev = NULL;
			arrOfNodes[i].inUse = false;
			arrOfNodes[i].nodeName = i;
		}

		// connect free nodes in a stack
		for(int i = 0; i < LIST_MAX_NUM_NODES - 1; i++){
			arrOfNodes[i].next = &arrOfNodes[i+1];
		}

		freeNodesPtr = &arrOfNodes[0]; // set freenodes pointer to start of free nodes


		// initilize array of lists 
		for(int i = 0; i < LIST_MAX_NUM_HEADS; i++){
			arrOfLists[i].head = NULL;
			arrOfLists[i].tail = NULL;
			arrOfLists[i].current = NULL;
			arrOfLists[i].oob = LIST_OOB_END;
			arrOfLists[i].count = 0;
			arrOfLists[i].inUse = false;
			arrOfLists[i].nextList = NULL;	
			arrOfLists[i].listName = i;			
		}

		// connect free lists in a stack 
		for(int i = 0; i < LIST_MAX_NUM_HEADS - 1; i++){
			arrOfLists[i].nextList = &arrOfLists[i+1];
		}

		freeListsPtr = &arrOfLists[0]; // set free list pointer to start of free lists
		
		listCreated = true; // ensure loop is not called again 
	}

	// eunsure user has not reached limit of the heads, eles return NULL
	if(listsUsedCount < LIST_MAX_NUM_HEADS){
		
		List* returnList = freeListsPtr;
		freeListsPtr = freeListsPtr->nextList;

		// reset just in case
		returnList->head = NULL;
		returnList->tail = NULL;
		returnList->current = NULL;
		returnList->oob = LIST_OOB_END;
		returnList->count = 0;


		returnList->inUse = true;
		returnList->nextList = NULL;

		listsUsedCount += 1;

		return returnList;

	}


	return NULL;
	
}

// Returns the number of items in pList.
int List_count(List* pList){ // TESTED
	
	//assert(pList != NULL);

	return pList->count;
}


// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_first(List* pList){ // TESTED
	//assert(pList != NULL);

	if (pList->count != 0){
		void* returnItem = pList->head->data;
		pList->current = pList->head;
		pList->oob = LIST_OOB_NONE;
		return returnItem;
	}

	pList->current = NULL;

	return NULL;
}

// Returns a pointer to the last item in pList and makes the last item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_last(List* pList){ // TESTED

	//assert(pList != NULL);

	if (pList->count != 0){
		void* returnItem = pList->tail->data;
		pList->current = pList->tail;
		pList->oob = LIST_OOB_NONE;
		return returnItem;
	}

	pList->current = NULL;

	return NULL;
}

// Advances pList's current item by one, and returns a pointer to the new current item.
// If this operation advances the current item beyond the end of the pList, a NULL pointer 
// is returned and the current item is set to be beyond end of pList.
void* List_next(List* pList){

	//assert(pList != NULL);

	void* returnItem;

	if(pList->count != 0){
		if(pList->oob == LIST_OOB_NONE){
			if(pList->current->next == NULL){
				pList->current = NULL;
				pList->oob = LIST_OOB_END;
				return NULL; // aka return null
			}

			pList->current = pList->current->next;
			returnItem = pList->current->data;
			return returnItem;

		} else if (pList->oob == LIST_OOB_START){
			pList->current = pList->head;
			returnItem = pList->current->data;
			pList->oob = LIST_OOB_NONE;
			return returnItem;

		} else if (pList->oob == LIST_OOB_END){
			return NULL; // aka return null
		}

	}

	return NULL; // RETURN NULL
}

// Backs up pList's current item by one, and returns a pointer to the new current item. 
// If this operation backs up the current item beyond the start of the pList, a NULL pointer 
// is returned and the current item is set to be before the start of pList.
void* List_prev(List* pList){
	//assert(pList != NULL);

	void* returnItem;

	if(pList->count != 0){
		if(pList->oob == LIST_OOB_NONE){
			if(pList->current->prev == NULL){
				pList->current = NULL;
				pList->oob = LIST_OOB_START;
				return NULL; // aka return null
			}

			pList->current = pList->current->prev;
			returnItem = pList->current->data;
			return returnItem;

		} else if (pList->oob == LIST_OOB_START){
			return NULL; // aka return null

		} else if (pList->oob == LIST_OOB_END){
			pList->current = pList->tail;
			returnItem = pList->current->data;
			pList->oob = LIST_OOB_NONE;
			return returnItem;
		}

	}

	return NULL; // RETURN NULL
}

// Returns a pointer to the current item in pList.
void* List_curr(List* pList){
	//assert(pList != NULL);

	void* returnItem;

	if(pList->current != NULL){
		returnItem = pList->current->data;
		return returnItem;
	}

	return NULL;

}

// Adds the new item to pList directly after the current item, and makes item the current item. 
// If the current pointer is before the start of the pList, the item is added at the start. If 
// the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_add(List* pList, void* pItem){

	//assert(pList != NULL && pItem != NULL && pList->inUse != false);
	
	//check to see if there's free nodes available
	if(nodesUsedCount >= LIST_MAX_NUM_NODES){
		return -1;
	}

	// for brand new or empty lists
	if(pList->count == 0){

		Node* newNode = Take_node();
		
		if(newNode == NULL){
			return -1;
		}

		newNode->data = pItem; // insert data
		pList->head = newNode; // set head 
		pList->tail = newNode; // set tail

		pList->current = newNode; // set current
		pList->oob = LIST_OOB_NONE; // change oob
		pList->count += 1; // add to count
		return 0;
	}

	// for established lists, current not at OOB
	if(pList->count > 0 && pList->oob == LIST_OOB_NONE){

		Node* newNode = Take_node();

		if(newNode == NULL){
			return -1;
		}

		// inserting at the end of list
		if(pList->current->next == NULL){

			if(pList->current != pList->tail){
				printf("SOMETHING IS WRONG 1\n");
				return -1;
			}

			newNode->data = pItem; // inset data 

			Node* prevNode = pList->tail; // create temp pointer at old tail
			pList->tail = newNode; // set tail as newNode
			newNode->prev = prevNode; // connect new node back to previous node
			prevNode->next = newNode; // connect prev node to new node

			pList->current = newNode; // update current 
			pList->oob = LIST_OOB_NONE; // update oob
			pList->count +=1; // update count
			return 0;

		}


		// inserting in between 
		if(pList->current->next != NULL && pList->current != pList->tail){
			newNode->data = pItem;

			Node* nextNode = pList->current->next; // create ptr to the node on RHS of current

			//connect current to new node, new node to current 
			pList->current->next = newNode;
			newNode->prev = pList->current;


			// connect next node to new node, new node to next node
			nextNode->prev = newNode;
			newNode->next = nextNode;

			pList->current = newNode;
			pList->oob = LIST_OOB_NONE;
			pList->count += 1;
			return 0;
		}

	}

	// when current is oob at the start 
	if(pList->count > 0 && pList->oob == LIST_OOB_START){

		return List_prepend(pList, pItem);
		
	}


	// when current is oob at the end
	if(pList->count > 0 && pList->oob == LIST_OOB_END){
		
		return List_append(pList, pItem);
	
	}

	return -1;
}

// Adds item to pList directly before the current item, and makes the new item the current one. 
// If the current pointer is before the start of the pList, the item is added at the start. 
// If the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert(List* pList, void* pItem){

	//assert(pList != NULL && pItem != NULL && pList->inUse != false);
	
	//check to see if there's free nodes available
	if(nodesUsedCount >= LIST_MAX_NUM_NODES){
		return -1;
	}

	// for brand new or empty lists
	if(pList->count == 0){

		Node* newNode = Take_node();
		
		if(newNode == NULL){
			return -1;
		}

		newNode->data = pItem; // insert data
		pList->head = newNode; // set head 
		pList->tail = newNode; // set tail

		pList->current = newNode; // set current
		pList->oob = LIST_OOB_NONE; // change oob
		pList->count += 1; // add to count
		return 0;
	}

	// for established lists, current not at OOB
	if(pList->count > 0 && pList->oob == LIST_OOB_NONE){

		Node* newNode = Take_node();

		if(newNode == NULL){
			return -1;
		}

		// inserting at the beginning of list
		if(pList->current->prev == NULL){

			if(pList->current != pList->head){
				printf("SOMETHING IS WRONG 1\n");
				return -1;
			}

			newNode->data = pItem; // inset data 

			Node* nextNode = pList->head; // create temp pointer at old head
			pList->head = newNode; // set head as newNode
			newNode->next = nextNode; // connect new node to next node
			nextNode->prev = newNode; // connect next node back to new node

			pList->current = newNode; // update current 
			pList->oob = LIST_OOB_NONE; // update oob
			pList->count +=1; // update count
			return 0;

		}


		// inserting in between 
		if(pList->current->prev != NULL && pList->current != pList->head){
			newNode->data = pItem; // insert data

			Node* prevNode = pList->current->prev; // create ptr to the node on LHS of current

			//connect current to new node, new node to current 
			pList->current->prev = newNode;
			newNode->next = pList->current;


			// connect prev node to new node, new node to prev node
			prevNode->next = newNode;
			newNode->prev = prevNode;

			pList->current = newNode;
			pList->oob = LIST_OOB_NONE;
			pList->count += 1;
			return 0;
		}

	}

	// when current is oob at the start 
	if(pList->count > 0 && pList->oob == LIST_OOB_START){

		return List_prepend(pList, pItem);
		
	}


	// when current is oob at the end
	if(pList->count > 0 && pList->oob == LIST_OOB_END){
		
		return List_append(pList, pItem);
	
	}

	return -1;
}

// Adds item to the end of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_append(List* pList, void* pItem){

	//assert(pList != NULL && pItem != NULL);


	//check to see if there's free nodes available
	if(nodesUsedCount >= LIST_MAX_NUM_NODES){
		return -1;
	}

	// for brand new or empty lists
	if(pList->count == 0){

		Node* newNode = Take_node();
		
		if(newNode == NULL){
			return -1;
		}

		newNode->data = pItem; // insert data
		pList->head = newNode; // set head 
		pList->tail = newNode; // set tail

		pList->current = newNode; // set current
		pList->oob = LIST_OOB_NONE; // change oob
		pList->count += 1; // add to count
		return 0;
	}



	// FOR ESTABLISHED LISTS
	Node* newNode = Take_node();

	if(newNode == NULL){
		return -1;
	}

	newNode->data = pItem; // inset data 

	Node* prevNode = pList->tail; // create temp pointer at old tail
	pList->tail = newNode; // set tail as newNode
	newNode->prev = prevNode; // connect new node back to previous node
	prevNode->next = newNode; // connect prev node to new node

	pList->current = newNode; // update current 
	pList->oob = LIST_OOB_NONE; // update oob
	pList->count +=1; // update count
	return 0;
}

// Adds item to the front of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_prepend(List* pList, void* pItem){

	//assert(pList != NULL && pItem != NULL);


	//check to see if there's free nodes available
	if(nodesUsedCount >= LIST_MAX_NUM_NODES){
		return -1;
	}

	// for brand new or empty lists
	if(pList->count == 0){

		Node* newNode = Take_node();
		
		if(newNode == NULL){
			return -1;
		}

		newNode->data = pItem; // insert data
		pList->head = newNode; // set head 
		pList->tail = newNode; // set tail

		pList->current = newNode; // set current
		pList->oob = LIST_OOB_NONE; // change oob
		pList->count += 1; // add to count
		return 0;
	}
	
	Node* newNode = Take_node();

	if(newNode == NULL){
		return -1;
	}

	newNode->data = pItem; // inset data 

	Node* nextNode = pList->head; // create temp pointer at old head
	pList->head = newNode; // set head as newNode
	newNode->next = nextNode; // connect new node to nextnode
	nextNode->prev = newNode; // connect next node back to new nde

	pList->current = newNode; // update current 
	pList->oob = LIST_OOB_NONE; // update oob
	pList->count +=1; // update count
	return 0;
}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
// from piazza: if current node is the last node, current should point oob after removal
void* List_remove(List* pList){

	// 4 cases.
	// 1. if current is set to oob
	// 2. if list is empty
	// 3. if list has 1 item
	// 4. if list has 2 or more items
		// if current is last item
		// if current is first item
		// if current is in middle 


	//assert(pList != NULL);


	// case 1
	if(pList->oob != LIST_OOB_NONE){
		return NULL;
	} 

	//case 2
	if(pList->count == 0){
		return NULL;
	} 


	//case 3
	if(pList->oob == LIST_OOB_NONE && pList->count == 1){
		void* returnItem = pList->current->data;

		Replace_node(pList->current);

		pList->head = NULL;
		pList->tail = NULL;
		pList->current = NULL;
		pList->oob = LIST_OOB_END;
		pList->count -= 1;

		return returnItem;


	}


	//case 4
	if(pList->oob == LIST_OOB_NONE && pList->count > 1){

		void* returnItem = pList->current->data;

		// subcase 1. if node to remove is the last node 
		if(pList->current == pList->tail){

			Node* nodeToRemove = pList->current;

			pList->tail = pList->tail->prev;	
			pList->tail->next = NULL;
			pList->current = NULL;

			Replace_node(nodeToRemove);
			
			pList->oob = LIST_OOB_END;
			pList->count -=1;

			return returnItem;

		} 

		// subcase 2. if node to remove is first node
		if (pList->current == pList->head) { 

			Node* nodeToRemove = pList->current;

			pList->head = pList->head->next;
			pList->head->prev = NULL;
			pList->current = pList->head;

			Replace_node(nodeToRemove);
			
			pList->oob = LIST_OOB_NONE;
			pList->count -=1;

			return returnItem;

		}

		// subcase 3. if node to remove is in the middle 
		if(pList->current != pList->head && pList->current != pList->tail){
			Node* nodeToRemove = pList->current;

			Node* nextNode = pList->current->next;
			Node* prevNode = pList->current->prev;

			nextNode->prev = prevNode;
			prevNode->next = nextNode;
			pList->current = nextNode;

			Replace_node(nodeToRemove);

			pList->oob = LIST_OOB_NONE;
			pList->count -=1;

			return returnItem;

		}

	}


	return NULL;

	// remove node from list using remove node function 
		// add node to front of list
		// resets the node, data = null, next, prev = null, inuse = false
		// subtract from global used node count 
	// connect the disconnected nodes
	// subtract from list count
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1. 
// pList2 no longer exists after the operation; its head is available
// for future operations.
// question @53 for list 1 empty 
void List_concat(List* pList1, List* pList2){
	//cases
	// both lists are empty
	// list 1 is empty
	// list 2 is empty
	// both lists are not empty 

	//assert(pList1 != NULL && pList2 != NULL && pList1 != pList2);

	if(pList1->count == 0 && pList2->count == 0){
		Replace_list(pList2);
		return;
	}

	if (pList1->count == 0 && pList2->count > 0){
		pList1->head = pList2->head;
		pList1->tail = pList2->tail;
		pList1->count = pList2->count;

		Replace_list(pList2);
		return;

	} 

	if (pList2->count == 0 && pList1->count > 0){
		Replace_list(pList2);
		return;
	} 

	if (pList1->count > 0 && pList2->count > 0){
		pList1->tail->next = pList2->head;
		pList2->head->prev = pList1->tail;
		pList1->tail = pList2->tail;
		pList1->count = pList1->count + pList2->count;
		Replace_list(pList2);

		return;

	} else {

		printf("MAJOR ERROR\n");
		return;
	}

	return;

}

// Delete pList. pItemFreeFn is a pointer to a routine that frees an item. 
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are 
// available for future operations.
typedef void (*FREE_FN)(void* pItem);
void List_free(List* pList, FREE_FN pItemFreeFn){

	//assert(pList != NULL); 

	Node* removeNode = pList->head;

	for (int i = 0; i <  pList->count; i++){
		
		Node* nextNode = removeNode->next;

		(*pItemFreeFn)(removeNode->data);

		Replace_node(removeNode);

		removeNode = nextNode;
	}

	Replace_list(pList);
	return;
}

// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void* List_trim(List* pList){

	//4 cases
	//1. list is empty
	//2. list has 1 item
	//3. list has 2 or more items

	//assert(pList != NULL);


	// case 1
	if(pList->count == 0){
		return NULL;
	}


	// case 2
	if(pList->count == 1){

		void* returnItem = pList->tail->data;
		Node* nodeToRemove = pList->tail;

		Replace_node(nodeToRemove);

		pList->head = NULL;
		pList->tail = NULL;
		pList->current = NULL;
		pList->oob = LIST_OOB_END;
		pList->count -= 1;

		return returnItem;
	}


	// case 3
	if(pList->count >= 2){
		void* returnItem = pList->tail->data;
		Node* nodeToRemove = pList->tail;
		Node* prevNode = pList->tail->prev;

		Replace_node(nodeToRemove);

		pList->tail = prevNode;
		pList->current = pList->tail;
		pList->tail->next = NULL;
		pList->oob = LIST_OOB_NONE;		
		pList->count -= 1;
			
		return returnItem;
	}

	return NULL;
}

// Search pList, starting at the current item, until the end is reached or a match is found. 
// In this context, a match is determined by the comparator parameter. This parameter is a
// pointer to a routine that takes as its first argument an item pointer, and as its second 
// argument pComparisonArg. Comparator returns 0 if the item and comparisonArg don't match, 
// or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator. 
// 
// If a match is found, the current pointer is left at the matched item and the pointer to 
// that item is returned. If no match is found, the current pointer is left beyond the end of 
// the list and a NULL pointer is returned.
// 
// If the current pointer is before the start of the pList, then start searching from
// the first node in the list (if any).
typedef bool (*COMPARATOR_FN)(void* pItem, void* pComparisonArg);
void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
//                list to srch 	name of comparison fuc     thing to compare with
	// 3 cases 
	// 1. if there's no items in the list
	// (non empty list):
	//		2. current is at oob start
	// 		3. current is not at oob start

	//case 1
	if(pList->count == 0){
		return NULL;
	}

	// case 2, case 3
	if(pList->count > 0){
		Node* searchNode;

		if(pList->oob == LIST_OOB_START){
			searchNode = pList->head;
		} else if (pList->oob == LIST_OOB_END) {
			return NULL;	
		} else {
			searchNode = pList->current;
		}

		bool itemFound = false;


		// seach only when item not found, and not at end of lists
		while(itemFound == false && searchNode != NULL){
			itemFound = pComparator(searchNode->data, pComparisonArg);

			// if item is found, set current to seachnode, oob to none, and return the item
			if(itemFound == true){
				void* returnItem = searchNode->data;
				pList->current = searchNode;
				pList->oob = LIST_OOB_NONE;

				return returnItem;
			}

			searchNode = searchNode->next;
		}


		// item was not found, set current to null, ooob to oob end
		pList->current = NULL;
		pList->oob = LIST_OOB_END;
		return NULL;

	}

	return NULL;

}




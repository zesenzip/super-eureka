// Zesen Feng
// zefeng
// pa5
// CSE101
// List.c
// Source Code for functions with List ADT
#include <stdio.h>
#include <stdlib.h>
#include "List.h"

// structs --------------------------------------------------------------------


typedef struct NodeObj
{
	long data;
	struct NodeObj* next;
	struct NodeObj* prev;
} NodeObj;

typedef NodeObj* Node;

typedef struct ListObj
{
	int length;
	int index;
	Node front;
	Node back;
	Node cursor; 
} ListObj;

//typedef ListObj* List;

// Constructors-Destructors for ListObj---------------------------------------------------
List newList(void)		// Creates and returns a new empty List.
{ 
	List L = malloc(sizeof(ListObj));
	L->front = NULL;
	L->back = NULL;
	L->cursor = NULL;
	L->length = 0;
	L->index = -1;		//negative index being undefined.

	return L;
}

void freeList(List* pL)	// Frees all heap memory associated with *pL, and sets *pL to NULL.
{	
	if( pL!=NULL && (*pL)!=NULL){
		clear(*pL);
		free(*pL);
		*pL = NULL;
		return;
	}
}

// Constructors-Destructors for NodeObj---------------------------------------------------
Node newNode(long data)	// Creates and returns a new Node.
{
	Node N = malloc(sizeof(NodeObj));
	N->data = data;
	N->next = NULL;
	N->prev = NULL;

	return N;
}

void freeNode(Node* pN)	// Frees all heap memory associated with *pN, and sets *pN to NULL.
{
	if( pN!=NULL && (*pN)!=NULL){
		(*pN)->next = NULL;
		(*pN)->prev = NULL;
		free(*pN);
		*pN = NULL;
	}
	
}
long data(Node N)	// Returns the data of the Node *or useless ?*
{
	if( N==NULL ){
    	fprintf(stderr ,"Node Error: calling data() on NULL Node reference\n");
    	exit(EXIT_FAILURE);
   	}
	return (N->data);
}

// Access functions -----------------------------------------------------------

int length(List L)	// Returns the number of elements in L.
{
	if( L==NULL ){
    	fprintf(stderr ,"List Error: calling length() on NULL List reference\n");
    	exit(EXIT_FAILURE);
   }
	return (L->length);
}

int index(List L)	// Returns index of cursor element if defined, -1 otherwise.
{					
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling index() on NULL List reference\n");
		exit(EXIT_FAILURE);
   }
	if( L->cursor == NULL){	// check extra time just in case
		L->index = -1;
	} 
	return(L->index);
}

long front(List L)	// Returns front element of L. Pre: length()>0
{
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling front() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if( L->length <= 0 ){	
		fprintf(stderr ,"List Error: calling front() on empty List\n");
		exit(EXIT_FAILURE);
	}
	return ( L->front->data);
}

long back(List L)	// Returns back element of L. Pre: length()>0
{
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling back() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if( L->length <= 0 ){	
		fprintf(stderr ,"List Error: calling back() on empty List\n");
		exit(EXIT_FAILURE);
	}
	return ( L->back->data);
}

long get(List L) // Returns cursor element of L. Pre: length()>0, index()>=0
{				// Relies on the index and cursor being always updated correctly
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling get() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if( L->length <= 0 ){	
		fprintf(stderr ,"List Error: calling get() on empty List\n");
		exit(EXIT_FAILURE);
	}
	if( L->index < 0 ){
		fprintf(stderr, "List Error: calling get() on undefined index\n");
		exit(EXIT_FAILURE);
	}
	return ( L->cursor->data);
}

int equals(List A, List B)	// Returns true (1) iff Lists A and B are in same
{							// state, and returns false (0) otherwise.
	int eq = 0;				// acting boolean / return value
	Node N = NULL;			// temp Node
	Node M = NULL;

	if( A==NULL || B==NULL ){
      fprintf( stderr, "List Error: calling equals() on NULL List reference\n");
      exit(1);
   }

   eq = ( A->length == B->length );
   N = A->front;
   M = B->front;

   while( eq && N!=NULL && M!=NULL){
      eq = (N->data == M->data);
      N = N->next;
      M = M->next;
   }

   return eq;
}

// Manipulation procedures ----------------------------------------------------
// set()
// Overwrites the cursor element with x. Pre: length()>0, index()>=0
void set(List L, long x){ 
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling set() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	L->cursor->data = x;
	return;
}

void clear(List L){		// Resets L to its original empty state.

	if( L==NULL ){
		fprintf(stderr ,"List Error: calling clear() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	while( L->length > 0){
		deleteBack(L);
	}
	return;
}
void moveFront(List L)	// If L is non-empty, sets cursor under the front element,
{						// otherwise does nothing.
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling moveFront() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if( L->length > 0){
		L->index = 0;
		L->cursor = L->front;
	}
	return;
}

void moveBack(List L)	// If L is non-empty, sets cursor under the back element,
{						// otherwise does nothing.
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling moveBack() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if( L->length > 0){
		L->index = (L->length)-1;
		L->cursor = L->back;
	}
	return;	
}
// If cursor is defined and not at front, move cursor one
// step toward the front of L; if cursor is defined and at
// front, cursor becomes undefined; if cursor is undefined
// do nothing
void movePrev(List L)
{
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling movePrev() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if( L->index > 0 ){	//if list is ever empty the index will be undefined
		L->index--;
		L->cursor = L->cursor->prev; 
	}
	else{ 
		L->index = -1;
		L->cursor = NULL;
	}
	return;
}
// If cursor is defined and not at back, move cursor one
// step toward the back of L; if cursor is defined and at
// back, cursor becomes undefined; if cursor is undefined
// do nothing
void moveNext(List L){
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling moveNext() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if( L->index < (L->length)-1){	//last element has index length-1
		L->index++;
		L->cursor = L->cursor->next;
	}
	else{
		L->index = -1;
		L->cursor = NULL;
	}
	return;
}

void prepend(List L, long data) // Insert new element into L. If L is non-empty,
{								// insertion takes place before front element.
	Node N = NULL;

	if( L==NULL ){
		fprintf(stderr ,"List Error: calling prepend() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	N = newNode(data);

	if (L->length == 0){	//empty list
		L->front = N;
		L->back  = N;
	}
	else{
		L->front->prev = N;
		N->next = L->front;
		L->front = N;
	}
	if ( L->index != -1){
		L->index++;	// one more element in front of the index of cursor
	}				// also needs condition since index can be undefined	
	N = NULL;
	L->length++;
	return;
}

void append(List L, long data){	// Insert new element into L. If L is non-empty,
								// insertion takes place after back element.
	Node N = NULL;

	if( L==NULL ){
		fprintf(stderr ,"List Error: calling append() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}

	N = newNode(data);
	if (L->length == 0){	//empty list
		L->front = N;
		L->back  = N;
	}
	else{
		L->back->next = N;
		N->prev = L->back;
		L->back = N;
	}	// appending to the back does not influence the cursor index

	N = NULL;
	L->length++;
	return;
}
void insertBefore(List L, long data){	// Insert new element before cursor.
	Node N = NULL;						// Pre: length()>0, index()>=0

	if( L==NULL ){
		fprintf(stderr ,"List Error: calling insertBefore() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if(L->length == 0){
		fprintf(stderr ,"List Error: calling insertBefore() on a empty List\n");
		exit(EXIT_FAILURE);
	}
	if( L->index < 0){
		fprintf(stderr ,"List Error: calling insertBefore() on a undefined index\n");
		exit(EXIT_FAILURE);
	}

	if( L->cursor == L->front ){ // case: cursor is the first item
		prepend(L, data);
		return;
	}
	else{
		N = newNode(data);
		N->prev = L->cursor->prev;
		N->next = L->cursor;
		L->cursor->prev->next = N;
		L->cursor->prev = N;
		L->index++;	//pushes index up
	}
	N = NULL;
	L->length++;
	return;
}

void insertAfter(List L, long data){	// Insert new element before cursor. 
									// Pre: length()>0, index()>=0
	Node N = NULL;

	if( L==NULL ){
		fprintf(stderr ,"List Error: calling insertAfter() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if(L->length == 0){
		fprintf(stderr ,"List Error: calling insertAfter() on a empty List\n");
		exit(EXIT_FAILURE);
	}
	if( L->index < 0){
		fprintf(stderr ,"List Error: calling insertAfter() on a undefined index\n");
		exit(EXIT_FAILURE);
	}
	if( L->cursor == L->back ){ // case: cursor is the last item
		append(L, data);
		return;
	}
	else{
		N = newNode(data);
		N->prev = L->cursor;
		N->next = L->cursor->next;
		L->cursor->next->prev = N;
		L->cursor->next = N;
	}
	N = NULL;
	L->length++;
	return;
}

void deleteFront(List L){	// Delete the front element. Pre: length()>0
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling deleteFront() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if(L->length < 1 ){
		fprintf(stderr ,"List Error: calling deleteFront() on a empty List\n");
		exit(EXIT_FAILURE);
	}
	if( L->front == L->back){	//case: length is 1
		freeNode(&(L->front));
		L->back = NULL;
		L->cursor = NULL;
		L->index = -1;
	}
	else{
		if(L->cursor == L->front){	//case: cursor is at front
			L->cursor = NULL;
			L->index = -1;
		}
		else L->index--;
		L->front = L->front->next;
		freeNode(&(L->front->prev));
		L->front->prev = NULL;
	} 
	L->length--;
	return;
}

void deleteBack(List L){	// Delete the front element. Pre: length()>0
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling deleteBack() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if(L->length < 1 ){
		fprintf(stderr ,"List Error: calling deleteBack() on a empty List\n");
		exit(EXIT_FAILURE);
	}
	if( L->front == L->back){	//case: length is 1
		freeNode(&(L->back));
		L->front = NULL;
		L->cursor = NULL;
		L->index = -1;
	}
	else{
		if(L->cursor == L->back){	//case: cursor is on the last node
			L->cursor = NULL;
			L->index = -1;
		}
		L->back = L->back->prev;
		freeNode(&(L->back->next));
	}
	L->length--;
	return;
}

void delete(List L){	// Delete cursor element, making cursor undefined.
						// Pre: length()>0, index()>=0
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling delete() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if(L->length < 1 ){
		fprintf(stderr ,"List Error: calling delete() on a empty List\n");
		exit(EXIT_FAILURE);
	}
	if( L->index < 0){
		fprintf(stderr ,"List Error: calling delete() on a undefined index\n");
		exit(EXIT_FAILURE);
	}
	if(L->cursor == L->front){
		deleteFront(L);
	}
	else if(L->cursor == L->back){
		deleteBack(L);
	}
	// at this point the list must have at least 3 nodes
	else{
		L->cursor->prev->next = L->cursor->next;
		L->cursor->next->prev = L->cursor->prev;
		freeNode(&(L->cursor));
		L->length--;
	}
	L->cursor = NULL;
	L->index = -1;
	return;
}

void printList(FILE* out, List L){	// Prints to the file pointed to by out, a
									// string representation of L consisting
									// of a space separated sequence of integers,
									// with front on left.
	Node N = NULL;
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling printList() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	N = L->front;
	while( N!= NULL){
		fprintf(out, "%ld ", N->data);
		N = N->next;
	}
	return;
}

List copyList(List L){	// Returns a new List representing the same integer
						// sequence as L. The cursor in the new list is undefined,
						// regardless of the state of the cursor in L. The state
						// of L is unchanged.
	List B = newList();
	Node N = NULL;
	if( L==NULL ){
		fprintf(stderr ,"List Error: calling copyList() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	N = L->front;
	while( N!= NULL){
		append(B, N->data);
		N = N->next;
	}
	return B;
}

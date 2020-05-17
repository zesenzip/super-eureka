// Zesen Feng
// zefeng
// pa3
// BigInteger.c
// source code for functions for BigInteger ADT
#define BASE 1000000000
#define POWER 9
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BigInteger.h"
#include "List.h"

// structs --------------------------------------------------------------------

typedef struct BigIntegerObj{
	int sign;
	List list; 
} BigIntegerObj;
void norm(BigInteger A, long swap);
// Constructors-Destructors for BigIntegerObj---------------------------------------------------

// newBigInteger()
// Returns a reference to a new BigInteger object in the zero state.
BigInteger newBigInteger(){
	BigInteger I = malloc(sizeof(BigIntegerObj));
	I->list = newList();
	I->sign = 0;
	return I;
}
// freeBigInteger()
// Frees heap memory associated with *pN, sets *pN to NULL.
void freeBigInteger(BigInteger* pN){
	if(pN!=NULL && (*pN)!=NULL){
		makeZero(*pN);
		free((*pN)->list);
		free(*pN);
		*pN = NULL;
	}
	return;
}
// Access functions -----------------------------------------------------------
// sign()
// Returns -1 if N is negative, 1 if N is positive, and 0 if N is in the zero
// state.
int sign(BigInteger N){
	if(N==NULL){
		fprintf(stderr ,"Big Integer Error: calling sign() on NULL BigInteger reference\n");
		exit(EXIT_FAILURE);
   	}
   	return N->sign;
}
// compare()
// Returns -1 if A<B, 1 if A>B, and 0 if A=B.
int compare(BigInteger A, BigInteger B){
	if(A==NULL || B==NULL){
		fprintf( stderr, "Big Integer Error: calling compare() on NULL BigInteger reference\n");
		exit(1);
	}
	if(A == B)	// case: same object is passed
		return 0;
	
   	if(A->sign > B->sign)
		return 1;
	else if (A->sign < B->sign)
		return -1;
	// at this point A and B have the same sign

	List a = A->list;
	List b = B->list;

	// longer list => bigger digit
	// positive and bigger digit = bigger
	// negative and bigger digit = smaller
	if(length(a) > length(b)){
		return(A->sign);
	}
	else if(length(b) > length(a)){
		return(-1 * B->sign);
	}

	// at this point a and b have the same lengh
	// most sig. long in the back
	moveBack(a);
	moveBack(b);

	while(index(a) != -1 && index(b) != -1){
		if(get(a) > get(b)){
			return(A->sign);
		}
		else if(get(b) > get(a)){
			return(-1 * A->sign);
		}
		else{
			movePrev(a);
			movePrev(b);
		}
	}
	// they are the same if they escape that ^ loop
	return 0;
}
// equals()
// Return true (1) if A and B are equal, false (0) otherwise.
int equals(BigInteger A, BigInteger B){
	int eq = 0;				

	if(A==NULL || B==NULL){
		fprintf( stderr, "Big Integer Error: calling equals() on NULL BigInteger reference\n");
		exit(1);
	}
	if(A == B){	// same object is passed
		return 1;
	}
	List M = A->list;
	List N = B->list;

	eq = (length(M) == length(N));
	eq = (A->sign == B->sign);
	moveFront(M);
	moveFront(N);

	while(eq && index(M) != -1 && index(N) != -1){
		eq = (get(N) == get(M));
		moveNext(M);
		moveNext(N);
	}

	return eq;
}

// Manipulation procedures ----------------------------------------------------

// makeZero()
// Re-sets N to the zero state.
// in the zero state list is not NULL, but just empty
void makeZero(BigInteger N){
	if(N==NULL){
		fprintf(stderr ,"Big Integer Error: calling makeZero() on NULL BigInteger reference\n");
		exit(EXIT_FAILURE);
	}
	if(N->list != NULL)
		clear(N->list);
	N->sign = 0;
	return;
}

// negate()
// Reverses the sign of N: positive <--> negative. Does nothing if N is in the
// zero state.
void negate(BigInteger N){
	if(N==NULL){
		fprintf(stderr ,"Big Integer Error: calling negate() on NULL BigInteger reference\n");
		exit(EXIT_FAILURE);
	}
	N->sign = (N->sign*-1);
	return;
}

// BigInteger Arithmetic operations -----------------------------------------------

// stringToBigInteger()
// Returns a reference to a new BigInteger object representing the decimal integer
// represented in base 10 by the string s.
// Pre: s is a non-empty string containing only base ten digits {0,1,2,3,4,5,6,7,8,9}
// and an optional sign {+, -} prefix.
BigInteger stringToBigInteger(char* s){
	if( s==NULL ){
		fprintf(stderr ,"Big Integer Error: calling stringToBigInteger() on empty string\n");
		exit(EXIT_FAILURE);
	}
	int counter = 1;
	BigInteger N = newBigInteger();
	if(*s == '+'){
		N->sign = 1;
		s++;
	}
	else if (*s == '-'){
		N->sign = -1;
		s++;
	}
	else N->sign = 1; // no sign = positive

	while(*s == '0'){	// rid of all leading 0's
		s++;
	}
	if (*s == '\0'){	// case: string with only 0's
		N->sign = 0;
		return N;
	}
	// back of the list is the most significant "digit".
	append(N->list, 0);			// case: must be non zero
	moveFront(N->list);
	// start at the back of the string
	// little endian style
    for (int i = strlen(s)-1; i >= 0; i--){

		long l = s[i] - '0';   // '1'-'0' = 1 
		if (l < 0 || l > 9){
			fprintf(stderr ,"Big Integer Error: calling stringToBigInteger() on invalid string\n");
			exit(EXIT_FAILURE);
		}

		set(N->list, get(N->list) + counter*l);
		counter = counter*10;
		
		// dont made another node if on last element
		if( counter == BASE && i != 0){  // string starts at most sig. 
			counter = 1;   // iteration prepends
			append(N->list, 0);
			moveBack(N->list);
		}
    }
   	return N;
}

// copy()
// Returns a reference to a new BigInteger object in the same state as N.
BigInteger copy(BigInteger N){
  if(N == NULL){
    fprintf(stderr ,"Big Integer Error: calling copy() on NULL BigInteger reference\n");
	exit(EXIT_FAILURE);
	}

	BigInteger A = newBigInteger();
	A->sign = N->sign;

	List n = N->list;
	moveBack(n);
	while(index(n) != -1){
		prepend(A->list, get(n));
		movePrev(n);
	}
	return A;
}

// Other operations -----------------------------------------------------------
// printBigInteger()
// Prints a base 10 string representation of N to filestream out.
void printBigInteger(FILE* out, BigInteger N){
	if(out == NULL){
		fprintf(stderr ,"Big Integer Error: calling printBigInteger() on NULL out file\n");
		exit(EXIT_FAILURE);
  	}
	if(N==NULL){
		fprintf(stderr ,"Big Integer Error: calling makeZero() on NULL BigInteger reference\n");
		exit(EXIT_FAILURE);
	}
	if( N->sign == -1){
		fprintf(out , "%s", "-");
	}
	if( length(N->list) == 0){
		fprintf(out, "0\n");
		return;
	}
	// print most significant long first -> in the back
	// no padded zeros for the most sig. long
	if(length(N->list) > 0)
		fprintf(out, "%ld", back(N->list));
	moveBack(N->list);
	movePrev(N->list);
	while( index(N->list) != -1){
		fprintf(out, "%0*ld", POWER, get(N->list));
		movePrev(N->list);
	}
	fprintf(out, "\n");
}
// add()
// Places the sum of A and B in the existing BigInteger S, overwriting its
// current state: S = A + B
// IMPORTANT S, A, B need to be differnt addresses. 
void add(BigInteger S, BigInteger A, BigInteger B){
	if(S == NULL || A == NULL || B == NULL){
		fprintf(stderr ,"Big Integer Error: calling add() on NULL BigInteger reference\n");
		exit(EXIT_FAILURE);
	}

	BigInteger C = copy(A);
	BigInteger D = copy(B);

	makeZero(S);
	// add least sig. longs first at front

	List a = C->list;
	List b = D->list;

	moveFront(a);
	moveFront(b);

	while(index(a) != -1 && index(b) != -1){
		append(S->list, ((C->sign * get(a))+(D->sign * get(b))));
		moveNext(a);
		moveNext(b);
	}
	while(index(a) != -1){
		append(S->list, C->sign*get(a));
		moveNext(a);
	}
	while(index(b) != -1){
		append(S->list, D->sign*get(b));
		moveNext(b);
	}
	norm(S, 1);

	freeBigInteger(&C);
	freeBigInteger(&D);
	return;
}
BigInteger sum(BigInteger A, BigInteger B){
	if(A == NULL || B == NULL){
		fprintf(stderr ,"Big Integer Error: calling sum() on NULL BigInteger reference\n");
		exit(EXIT_FAILURE);
	}
	BigInteger S = newBigInteger();
	add(S, A, B);
	return S;
}
// subtract()
// Places the difference of A and B in the existing BigInteger D, overwriting
// its current state: D = A - B
void subtract(BigInteger D, BigInteger A, BigInteger B){
	if(D == NULL || A == NULL || B == NULL){
		fprintf(stderr ,"Big Integer Error: calling subtract() on NULL BigInteger reference\n");
		exit(EXIT_FAILURE);
	}

	BigInteger P = copy(A);
	BigInteger Q = copy(B);

	makeZero(D);
	// add least sig. longs first at front
	List a = P->list;
	List b = Q->list;

	moveFront(a);
	moveFront(b);

	while(index(a) != -1 && index(b) != -1){
		append(D->list, ((P->sign * get(a))-(Q->sign * get(b))));
		moveNext(a);
		moveNext(b);
	}
	while(index(a) != -1){
		append(D->list, P->sign*get(a));
		moveNext(a);
	}
	while(index(b) != -1){
		append(D->list, Q->sign*get(b)*-1);
		moveNext(b);
	}
	norm(D, 1);

	freeBigInteger(&P);
	freeBigInteger(&Q);

	return;
}

// diff()
// Returns a reference to a new BigInteger object representing A - B.
BigInteger diff(BigInteger A, BigInteger B){
	if(A == NULL || B == NULL){
		fprintf(stderr ,"Big Integer Error: calling diff() on NULL BigInteger reference\n");
		exit(EXIT_FAILURE);
	}
	BigInteger D = newBigInteger();
	subtract(D, A, B);
	return D;	
}
// multiply()
// Places the product of A and B in the existing BigInteger P, overwriting
// its current state: P = A*B
void multiply(BigInteger P, BigInteger A, BigInteger B){
	if(P == NULL || A == NULL || B == NULL){
		fprintf(stderr ,"Big Integer Error: calling multiply() on NULL BigInteger reference\n");
		exit(EXIT_FAILURE);
	}

	BigInteger C = copy(A);
	BigInteger D = copy(B);

	makeZero(P);
	if( A->sign == 0 || B->sign == 0)
		return;
	// add least sig. longs first at front
	List a = C->list;
	List b = D->list;
	List p = P->list;

	moveFront(a);
	moveFront(b);
	moveFront(p);
	BigInteger temp = newBigInteger();
	List sum = temp->list;
	temp->sign = 1;

	for (int i = 0; i < length(b); ++i){
		clear(sum);

		for (int k = 0; k < i; ++k){
				prepend(sum, 0);
			}
		for (int j = 0; j < length(a); ++j){
			append(sum, get(b)*get(a));
			moveNext(a);
		}
		BigInteger clone = copy(P);
		norm(temp, 1);

		add(P, clone, temp);
		freeBigInteger(&clone);
		moveNext(b);
		moveFront(a);
	}
	freeBigInteger(&temp);

	P->sign = C->sign * D->sign;
	freeBigInteger(&C);
	freeBigInteger(&D);

	return;
}

// prod()
// Returns a reference to a new BigInteger object representing A*B
BigInteger prod(BigInteger A, BigInteger B){
	if(A == NULL || B == NULL){
		fprintf(stderr ,"Big Integer Error: calling prod() on NULL BigInteger reference\n");
		exit(EXIT_FAILURE);
	}
	BigInteger P = newBigInteger();
	multiply(P, A, B);
	return P;		
}
// magic helper function-------------------------------------------------------
// does nothing to empty list. 
// makes sure all values in list is postive and less than the BASE
void norm(BigInteger A, long swap){
	if( A == NULL){
		fprintf(stderr,"Big Integer Error: calling norm() on NULL BigInteger reference\n");
		exit(EXIT_FAILURE);
	}
	while(length(A->list) > 0 && back(A->list) == 0){
		deleteBack(A->list);
	}
	// no empty lists
	if(length(A->list) == 0){
		return;
	}

	List a = A->list;
	long carry = 0;
	moveFront(A->list);

	for (int i = 1; i < length(a); ++i)
	{
		set(a, (carry+get(a)*swap));
		long temp = get(a);
		if(temp < 0){
			carry = (temp/BASE)-1;	// integer division but negative
			set(a, (-1*carry*BASE)+temp);
		}
		else if (temp >= BASE){
			carry = (temp/BASE);
			set(a, temp-(carry*BASE));
		}
		else 
			carry = 0;
		moveNext(a);
	}
	// no borrowing allowed at the most sig. long
	set(a, (carry+get(a)*swap));
	moveBack(a);
	

	if (get(a) >= BASE){
		carry = (get(a)/BASE);
		set(a, get(a)-(carry*BASE));
		append(a, carry);
	}


	// kill all the Zeros
	while(length(a) > 0 && back(a) == 0){
		deleteBack(a);
	}
	if(length(a) == 0){
		A->sign = 0;	
	}
	// if the most significant long is negative the A is negative
	// then swap the sign of each value and normalize again. 
	if(back(a) < 0){
		norm(A, -1);
		A->sign = -1;
		return;
	}
	A->sign = 1;
	return;
}



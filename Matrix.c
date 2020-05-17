// Zesen Feng
// zefeng
// pa2
// Matrix.c
// contains functions for the Matrix ADT
#include <stdio.h>
#include <stdlib.h>
#include "Matrix.h"
#include "List.h"

// structs --------------------------------------------------------------------

typedef struct EntryObj
{
	int column;
	double value;
} EntryObj;
typedef EntryObj* Entry;

typedef struct MatrixObj
{
	int size;
	int NNZ;
	List* row;
} MatrixObj;

// Constructors-Destructors for MatrixObj---------------------------------------------------

// Allocate new memory for MatrixOvj and return Matrix
Matrix newMatrix(int n){
	Matrix M = malloc(sizeof(MatrixObj));
	M->size = n;
	M->NNZ = 0;
	M->row = calloc(n,sizeof(List));
	for (int i = 0; i < n; ++i){
		M->row[i] = newList();
	}
	return M;
}

// free all memory with this Matrix
// needs to make zero then delete every list
void freeMatrix(Matrix* pM){
	List L = NULL;
	if( pM!=NULL && (*pM)!=NULL){
		makeZero(*pM);
		for (int i = 0; i < size(*pM); ++i)
		{
			L = (*pM)->row[i];
			freeList(&(L));
		}
		free((*pM)->row);
		free(*pM);
		*pM = NULL;		// remove referce to freed Matrix.
		return;
	}
}

// Constructors-Destructors for EntryObj---------------------------------------------------

// Allocate new memory for EntryObj and return Entry
Entry newEntry(int c, double v){ 
	Entry E = malloc(sizeof(EntryObj));
	E->column = c;
	E->value = v;
	return E;
}

// free memory associated memory with entry
void freeEntry(Entry* pE){
	if( pE!=NULL && (*pE)!=NULL){
		free(*pE);
		*pE = NULL;		// remove referece to freed struct.
		return;
	}
}
// Helper fuctions----------------------
double vectorDot(List A, List B);
void vectorSum(List A, List B, int i, Matrix M);

// private access functions for Entry--------------------------------------------

int column(Entry E){	// Returns the data of the Node *or useless ?*
	if(E == NULL){
    	fprintf(stderr ,"Entry Error: calling column() on NULL Node reference\n");
    	exit(EXIT_FAILURE);
   }
	return(E->column);
}

double value(Entry E){
	if(E == NULL){
		fprintf(stderr, "Entry Error: calling value() on NULL Entry reference\n");
		exit(EXIT_FAILURE);
	}
	return(E->value);
}

// Access functions -----------------------------------------------------------

int size(Matrix M){	// return the size of square matrix M
	if(M == NULL){
		fprintf(stderr, "Matrix Error: calling size() on NULL Matrix reference\n");
		exit(EXIT_FAILURE);
	}
	return(M->size);
}

int NNZ(Matrix M){	// return the number of non-zero elements in M.
	if(M == NULL){
		fprintf(stderr, "Matrix Error: calling NNZ() on NULL Matrix reference\n");
		exit(EXIT_FAILURE);
	}
	return(M->NNZ);
}

int equals(Matrix A, Matrix B){	// return true (1) if matricies A and B are equal. 0 otherwise
	if(A == NULL){
		fprintf(stderr, "Matrix Error: calling equals() on NULL A Matrix reference\n");
		exit(EXIT_FAILURE);
	}
	if(B == NULL){
		fprintf(stderr, "Matrix Error: calling equals() on NULL B Matrix reference\n");
		exit(EXIT_FAILURE);
	}
	if(A == B){	// case: literally same structure.
		return 1;
	}

	// both the column and value needs to be the same.
	// two triggers to stop the while loop
	int eqC = size(A) == size(B);
	int eqV = size(A) == size(B);

	Entry N = NULL;
	Entry M = NULL;

	//loop for each list.
	for (int i = 0; i < size(A); ++i)
	{
		moveFront(A->row[i]);
		moveFront(B->row[i]);
		if(length(A->row[i]) != length(B->row[i]))
			return 0;
		while(eqC && eqV && index(A->row[i]) != -1 && index(B->row[i]) != -1){
			N = get(A->row[i]);
			M = get(B->row[i]);
			eqC = (column(N) == column(M));
			eqV = (value(N) == value(M));
			moveNext(A->row[i]);
			moveNext(B->row[i]);
		}
	}
	// return 1 only if both flags remain 1
	if(eqC && eqV){
		return 1;
	}
	else
		return 0;
}

// Manipulation procedures---------------------------------------------

// makeZero()
// Re-sets M to the zero Matrix.
void makeZero(Matrix M){
	
	if(M == NULL){
		fprintf(stderr, "Matrix Error: calling makeZero() on NULL Matrix reference\n");
		exit(EXIT_FAILURE);
	}

	Entry E = NULL;
	M->NNZ = 0;
	// each list
	for (int i = 0; i < size(M); ++i)
	{
		List L = M->row[i];
		if(L == NULL){	//this should not happen but check anyways
			fprintf(stderr, "Matrix Error: calling makeZero on NULL List reference\n");
		}
		// each Node
		while(length(L) > 0){
			moveBack(L);
			E = get(L);
			freeEntry(&E);
			deleteBack(L);
		}
	}
}
// changeEntry()
// Changes the ith row, jth column of M to the value x.
// Pre: 1<=i<=size(M), 1<=j<=size(M)
void changeEntry(Matrix M, int i, int j, double x){
	if(M == NULL){
		fprintf(stderr, "Matrix Error: calling changeEntry() on NULL Matrix reference\n");
		exit(EXIT_FAILURE);
	}
	if( i > size(M) || i < 1){
		fprintf(stderr, "Matrix Error: calling changeEntry on invalid row\n");
		exit(EXIT_FAILURE);
	}
	if( j > size(M) || j < 1){
		fprintf(stderr, "Matrix Error: calling changeEntry on invalid column\n");
		exit(EXIT_FAILURE);
	}

	List L = M->row[i-1];
	moveFront(L);
	
	// case: empty list
	if(length(L) == 0){
		if(x == 0){
			return;
		}
		else{
			Entry E = newEntry(j, x);
			append(L, E);
			M->NNZ++;
			return;
		}	
	}

	//stop at the end of list or at something bigger.
	while(index(L) != -1 && column(get(L)) < j){
		moveNext(L);
		if(index(L) < 0 && x != 0){
			Entry E = newEntry(j, x);
			append(L ,E);
			M->NNZ++;
			return;
		}
		if(index(L) < 0 && x == 0){
			return;
		}
	}

	// node exists
	if(column(get(L)) == j){
		Entry E = get(L);
		if(x == 0.0){
			freeEntry(&E);
			delete(L);
			M->NNZ--;
			return;
		}
		else{
			E->value = x;
			return;
		}
	}
	// cursor is on something larger
	else{
		if( x == 0.0){
			return;
		}
		else{
			Entry E = newEntry(j, x);
			insertBefore(L, E);
			M->NNZ++;
		}
	}
	return;
}

// Matrix Arithmetic operations -----------------------------------

// copy
// Return a reference to a new Matrix object having the same entries as A 
Matrix copy(Matrix A){
	if(A == NULL){
		fprintf(stderr, "Matrix Error: calling copy() on NULL Matrix reference\n");
		exit(EXIT_FAILURE);
	}
	List L = NULL;
	Matrix M = newMatrix(size(A));
	// for each row
	for (int i = 0; i < size(A); ++i){
		L = A->row[i];
		moveFront(L);
		while(index(L) != -1){
			Entry E = get(L);
			changeEntry(M, i+1, column(E), value(E));
			moveNext(L);
		}
	}
	return M;
}

// transpose()
// Returns a reference to a new Matrix object representing the transpose
// of A.
Matrix transpose(Matrix A){
	if(A == NULL){
		fprintf(stderr, "Matrix Error: calling transpose() on NULL Matrix reference\n");
		exit(EXIT_FAILURE);
	}
	List L = NULL;
	Matrix M = newMatrix(size(A));
	// for each row
	for (int i = 0; i < size(A); ++i){
		L = A->row[i];
		moveFront(L);
		while(index(L) != -1){
			Entry E = get(L);
			changeEntry(M, column(E), i+1, value(E));
			moveNext(L);
		}
	}
	return M;
}

// scalarMult()
// Returns a reference to a new Matrix object representing xA.
Matrix scalarMult(double x, Matrix A){
	if(A == NULL){
		fprintf(stderr, "Matrix Error: calling scalarMult() on NULL Matrix reference\n");
		exit(EXIT_FAILURE);
	}
	List L = NULL;
	Matrix M = newMatrix(size(A));
	// for each row
	for (int i = 0; i < size(A); ++i){
		L = A->row[i];
		moveFront(L);
		while(index(L) != -1){
			Entry E = get(L);
			changeEntry(M, i+1, column(E), x*value(E));
			moveNext(L);
		}
	}
	return M;
}

// sum()
// Returns a reference to a new Matrix object representing A+B.
// pre: size(A)==size(B)
Matrix sum(Matrix A, Matrix B){
	if(A == NULL || B == NULL){
		fprintf(stderr, "Matrix Error: calling sum() on NULL Matrix reference\n");
		exit(EXIT_FAILURE);
	}
	if(size(A) != size(B)){
		fprintf(stderr, "Matrix Error: calling sum() on Matrices difference size\n");
		exit(EXIT_FAILURE);
	}
	
	Matrix M;
	if( A == B ){
		M = scalarMult(2, A);
		return M;
	}
	M = newMatrix(size(A));
	List a = NULL;
	List b = NULL;
	for (int i = 0; i < size(A); ++i){
		a = A->row[i];
		b = B->row[i];
		if(length(a) == 0 && length(b) == 0)
			continue;
		//M is the list to be modified
		vectorSum(a, b, i+1, M);
	}
	return M;
}

// diff()
// Returns a reference to a new Matrix object representing A-B.
// pre: size(A)==size(B)
Matrix diff(Matrix A, Matrix B){
	if(A == NULL || B == NULL){
		fprintf(stderr, "Matrix Error: calling diff() on NULL Matrix reference\n");
		exit(EXIT_FAILURE);
	}
	if(size(A) != size(B)){
		fprintf(stderr, "Matrix Error: calling diff() on Matrices difference size\n");
		exit(EXIT_FAILURE);
	}

	Matrix m = scalarMult( -1 , B);
	Matrix M = sum(A, m);
	freeMatrix(&m);
	return M;

}
// product()
// Returns a reference to a new Matrix object representing AB
// pre: size(A)==size(B)
Matrix product(Matrix A, Matrix B){
	if(A == NULL || B == NULL){
		fprintf(stderr, "Matrix Error: calling product() on NULL Matrix reference\n");
		exit(EXIT_FAILURE);
	}
	if(size(A) != size(B)){
		fprintf(stderr, "Matrix Error: calling product() on Matrices difference size\n");
		exit(EXIT_FAILURE);
	}

	Matrix M = newMatrix(size(A));
	// temp matrix that is transpose of (B)
	Matrix C = transpose(B);
	
	double product = 0;

	for (int i = 0; i < size(A); ++i){
		if(length(A->row[i]) == 0)
			continue;
		for (int j = 0; j < size(A); ++j){
			if(length(C->row[j]) == 0)
				continue;
			product = vectorDot(A->row[i],C->row[j]);
			if(product !=0)
				changeEntry(M, i+1, j+1, product);
		}
	}
	freeMatrix(&C);
	return M;
}

// printMatrix()
// Prints a string representation of Matrix M to filestream out. Zero rows
// are not printed. Each non-zero is represented as one line consisting
// of the row number, followed by a colon, a space, then a space separated
// list of pairs "(col, val)" giving the column numbers and non-zero values
// in that row. The double val will be rounded to 1 decimal point.
void printMatrix(FILE* out, Matrix M){
	if(M == NULL){
		fprintf(stderr, "Matrix Error: calling printMatrix() on NULL Matrix reference\n");
		exit(EXIT_FAILURE);
	}
	if(out == NULL){
		fprintf(stderr, "Matrix Error: calling printMatrix() on NULL filestream reference\n");
		exit(EXIT_FAILURE);
	}
	List L = NULL;
	Entry E = NULL;
	for (int i = 0; i < size(M); ++i)
	{	

		L = M->row[i];
		moveFront(L);
		if(length(L) == 0)
			continue;
		fprintf(out, "%d:", i+1);
		while(index(L) >= 0){
			E = get(L);
			fprintf(out, " (%d,%.1f)", column(E), value(E));
			moveNext(L);
		}
		fprintf(out, "\n");
	}
}
// HELPER FUNCTIONS ---------------------------------------------
// returns a scalar as dot product of two vectors
double vectorDot(List A, List B){
	if(A == NULL || B == NULL){
		fprintf(stderr, "Matrix Error: calling vectorDot() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if(length(A) == 0 || length(B) == 0)
		return 0;
	moveFront(A);
	moveFront(B);
	double sum = 0.0;

	while(index(A) != -1 && index(B) != -1){
		if(column((Entry)get(A)) == column((Entry)get(B))){
			sum = sum + value((Entry)get(A))*value((Entry)get(B));
			moveNext(A);
			moveNext(B);
		}
		else if(column((Entry)get(A)) < column((Entry)get(B)))
			moveNext(A);
		else
			moveNext(B);
	}
	return sum;
}

// void return
// modifies List A with the sum with B
// i is the row currently being worked on
void vectorSum(List A, List B, int i, Matrix M){
	if(A == NULL || B == NULL){
		fprintf(stderr, "Matrix Error: calling vectorDot() on NULL List reference\n");
		exit(EXIT_FAILURE);
	}
	if(length(A) == 0 && length(B) == 0)
		return;
	moveFront(A);
	moveFront(B);
	
	while(index(B) != -1 || index(A) != -1){
		
		// columnn of A is currently equal or larger
		// if A is at the end just add the rest of the B
		if(index(A) == -1){
			changeEntry(M, i, column((Entry)get(B)), value((Entry)get(B)));
			moveNext(B);
			continue;
		}
		if(index(B) == -1){
			changeEntry(M, i, column((Entry)get(A)), value((Entry)get(A)));
			moveNext(A);
			continue;
		}	
		if (column((Entry)get(A)) == column((Entry)get(B))){	//two entries same column
			changeEntry(M, i, column((Entry)get(B)), value((Entry)get(A))+value((Entry)get(B)));
			moveNext(B);
			moveNext(A);
		}	
		else if (column((Entry)get(A)) > column((Entry)get(B))){	//column of B is larger than A
			changeEntry(M, i, column((Entry)get(B)), value((Entry)get(B)));
			moveNext(B);
		}
		else if (column((Entry)get(A)) < column((Entry)get(B))){
			changeEntry(M, i, column((Entry)get(A)), value((Entry)get(A)));
			moveNext(A);
		}
	}

}

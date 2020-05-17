// Zesen Feng
// zefeng
// pa2
// Sparse.c
// Client for Matrix.c and List.c
// takes file with outlines for two matricies and prints results of various operations
// Important LINELIMIT
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"Matrix.h"

int main(int argc, char* argv[]){
	FILE *in, *out;

	if( argc != 3 ){
    	printf("Usage: %s <input file> <output file>\n", argv[0]);
 		exit(EXIT_FAILURE);
 	}
 	in = fopen(argv[1], "r");
	out = fopen(argv[2], "w");
   	if( in==NULL ){
    	printf("Unable to open file %s for reading\n", argv[1]);
    	exit(1);
   	}
   	if( out==NULL ){
      	printf("Unable to open file %s for writing\n", argv[2]);
      	exit(1);
   	}
   	int aNNZ;	// NNZ for A
   	int bNNZ;	// NNZ for B
   	int k;	// size of two arrays
   	Matrix A;
   	Matrix B;
   	fscanf(in, "%d %d %d", &k, &aNNZ, &bNNZ);
   	A = newMatrix(k);
   	B = newMatrix(k);

   	// clean line 2
   	fscanf(in," ");

   	// loading up A
   	for (int i = 0; i < aNNZ; ++i)
   	{
   		int columns;
   		int rows;
   		double input;
   		fscanf(in, "%d %d %lf", &rows, &columns, &input);
   		changeEntry(A, rows, columns, input);
   	}

   	// clean the empty line
	fscanf(in," ");

	// loading up B

	for (int j = 0; j < bNNZ; ++j)
   	{
   		int columns;
   		int rows;
   		double input;
   		fscanf(in, "%d %d %lf", &rows, &columns, &input);
   		changeEntry(B, rows, columns, input);
   	}

   	fprintf(out, "A has %d non-zero entries:\n", NNZ(A));
   	printMatrix(out, A);

   	fprintf(out, "\nB has %d non-zero entries:\n", NNZ(B));
   	printMatrix(out, B);

   	fprintf(out, "\n(1.5)*A =\n");
   	Matrix C = scalarMult(1.5, A);
   	printMatrix(out, C);
   	freeMatrix(&C);

   	fprintf(out, "\nA+B =\n");
   	Matrix D = sum(A, B);
   	printMatrix(out, D);
   	freeMatrix(&D);

   	fprintf(out, "\nA+A =\n");
   	Matrix E = sum(A, A);
   	printMatrix(out, E);
   	freeMatrix(&E);

   	fprintf(out, "\nB-A =\n");
   	Matrix F = diff(B, A);
   	printMatrix(out, F);
   	freeMatrix(&F);

   	fprintf(out, "\nA-A =\n");
   	Matrix G = diff(A, A);
   	printMatrix(out, G);
   	freeMatrix(&G);

   	fprintf(out, "\nTranspose(A) =\n");
   	Matrix H = transpose(A);
   	printMatrix(out, H);
   	freeMatrix(&H);

   	fprintf(out, "\nA*B =\n");
   	Matrix I = product(A, B);
   	printMatrix(out, I);
   	freeMatrix(&I);

   	fprintf(out, "\nB*B =\n");
   	Matrix J = product(B, B);
   	printMatrix(out, J);
   	freeMatrix(&J);

   	freeMatrix(&A);
   	freeMatrix(&B);

}

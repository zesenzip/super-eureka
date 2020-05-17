#include<stdio.h>
#include<stdlib.h>
#include"BigInteger.h"
// takes input file 
// creates bigIntegers 
// prints to out file 
// A, B, A+B, A-B, A-A, 3A - 2B, AB , A^2, B^2, 9A^2 + 16B^5
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
	int sizeA;
	int sizeB;

	fscanf(in, "%d", &sizeA);
	char s[sizeA+1];
	fscanf(in, "%s", s);

	fscanf(in, "%d", &sizeB);
	char t[sizeB+1];
	fscanf(in, "%s", t);

   fprintf(stdout, "%d\n", sizeA);
   fprintf(stdout, "%s\n", s);

	BigInteger A = stringToBigInteger(s);
	BigInteger A1 = copy(A);
	BigInteger B = stringToBigInteger(t);
 	BigInteger B1 = copy(B);

	printBigInteger(out, A);
	fprintf(out, "\n");

	printBigInteger(out, B);
	fprintf(out, "\n");

	BigInteger C = sum(A,B);
	printBigInteger(out, C);
	fprintf(out, "\n");
	freeBigInteger(&C);

	BigInteger D = diff(A,B);
	printBigInteger(out, D);
	fprintf(out, "\n");
	freeBigInteger(&D);

	BigInteger E = diff(A, A1);
	printBigInteger(out, E);
	fprintf(out, "\n");
	freeBigInteger(&E);

  	BigInteger three = stringToBigInteger("3");
  	BigInteger two = stringToBigInteger("2");
  	BigInteger Ax3 = prod(A, three);
  	BigInteger Bx2 = prod(B, two);

  	BigInteger F = diff(Ax3, Bx2);
  	printBigInteger(out, F);
  	fprintf(out, "\n");
  	freeBigInteger(&F);

  	freeBigInteger(&three);
  	freeBigInteger(&two);
  	freeBigInteger(&Ax3);
  	freeBigInteger(&Bx2);

   BigInteger AB = prod(A,B);
   printBigInteger(out, AB);
   fprintf(out, "\n");
   freeBigInteger(&AB);

  	BigInteger Asq = prod(A, A1);
  	printBigInteger(out, Asq);
  	fprintf(out, "\n");

  	BigInteger Bsq = prod(B, B1);
  	printBigInteger(out, Bsq);
  	fprintf(out, "\n");


  	BigInteger nine = stringToBigInteger("9");
  	BigInteger sixt = stringToBigInteger("16");
  	BigInteger Apower4 = prod(Asq, Asq);
  	BigInteger Bpower4 = prod(Bsq, Bsq);
   BigInteger Bpower5 = prod(Bpower4, B);

   BigInteger A9power4 = prod(Apower4, nine);
   BigInteger B16power5 = prod(Bpower5, sixt);

  	BigInteger G = sum(A9power4, B16power5);
  	printBigInteger(out, G);
  	fprintf(out, "\n");
  	freeBigInteger(&G);	

  	freeBigInteger(&Asq);
 	freeBigInteger(&Bsq);
   freeBigInteger(&nine);
  	freeBigInteger(&sixt);
	freeBigInteger(&Apower4);
   freeBigInteger(&Bpower4);
   freeBigInteger(&Bpower5);
   freeBigInteger(&A9power4);
   freeBigInteger(&B16power5);

   freeBigInteger(&A);
   freeBigInteger(&A1);
   freeBigInteger(&B);
   freeBigInteger(&B1);

   fclose(in);
   fclose(out);

	return (EXIT_SUCCESS);
}
// Zesen Feng
// zefeng
// pa1
// Lex.c
// Client for List.c
// takes input file with list of words 
// outputs a file with lexigraphically organized list. 
// important LINE LIMIT
// uses a dynamically allocated string array.
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"List.h"

#define MAX_LEN 160
#define MAX_LINES 1024

int main(int argc, char* argv[]){

  int n=0;
  int x=0;
  FILE *in, *out;
  char line[MAX_LEN];
  char* arr[MAX_LINES];
  List A = NULL;

  // check command line for correct number of arguments
  if( argc != 3 ){
    printf("Usage: %s <input file> <output file>\n", argv[0]);
 	  exit(EXIT_FAILURE);
	}
  // open files for reading and writing 
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

   	A = newList();
    // insert the first node
   	fgets(line, MAX_LEN, in);
    arr[n] = malloc(sizeof(char)*strlen(line)+1);
    strcpy(arr[n],line);
   	append(A, n);

   	while( fgets(line, MAX_LEN, in) != NULL)  { //every line
   		moveFront(A);
   		n++;
      arr[n] = malloc(sizeof(char)*strlen(line)+1);
      strcpy(arr[n],line);
      //traverse until something bigger or end of list, left to right
   		while( index (A) >= 0 && strcmp(line,arr[get(A)]) > 0){
   			moveNext(A);
   		}
      if(index(A) == -1){ //case: fall off the edge
        append(A, n);
      }
      else
   		 insertBefore(A, n);
   	}

    moveFront(A); //prints to out
   	while (index(A)>=0){
   		x = get(A);
   		fprintf(out, "%s", arr[x]);
   		moveNext(A);
   	}
    // reusing n to traverse
    n = 0;
    while(arr[n] !=NULL){
      free(arr[n]);
      arr[n] = NULL;
      n++;
    }

   	freeList(&A);
   	fclose(in);
   	fclose(out);
   	return(EXIT_SUCCESS);
}

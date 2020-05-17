// Zesen Feng
// zefeng
// pa5
// CSE101
// FindComponents.c
// creates Graph ADT based on input file.
// outputs to file strongly connected components
#include<stdio.h>
#include<stdlib.h>
#include"Graph.h"
// take inputs to create 
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

  int order;
  fscanf(in, "%d", &order);

  Graph G = newGraph(order);
  int u;
  int v;

  fscanf(in, "%d %d", &u, &v);

  while( u > 0 && v > 0){
    addArc(G, u, v);
    fscanf(in, "%d %d", &u, &v);
  }
  fprintf(out, "Adjacency list representation of G:\n" );
  printGraph(out, G);
  fprintf(out, "\n");

  // stack with default convention
  List stack = newList();
  for (int i = 1; i <= order; ++i)
  {
    append(stack, i);
  }

  DFS(G, stack);

  Graph tG = transpose(G);
  DFS(tG, stack);

  moveBack(stack);
  // count number of SCC by looking for NIL parents
  int numSCC = 0;
  while(index(stack) != -1){
    if( getParent(tG, get(stack)) == NIL)
      numSCC++;
    movePrev(stack);
  }

  fprintf(out, "G contains %d strongly connected components:\n", numSCC);
  List SCC = newList();

  // top of the stack is back
  // pop and prepend to current SCC
  // clear current SCC when root is found
  moveBack(stack);
  int SCCnum = 1;
  while(length(stack) > 0){
    prepend(SCC, back(stack));
    if(getParent(tG, back(stack)) == NIL){
      fprintf(out, "Component %d:", SCCnum);
      SCCnum++;
      // print from SCC
      moveFront(SCC);
      while(index(SCC) != -1){
        fprintf(out, " %ld", get(SCC));
        moveNext(SCC);
      }
      clear(SCC);
      fprintf(out, "\n");
    }
    deleteBack(stack);
  }

  freeGraph(&tG);
  freeList(&SCC);
  freeList(&stack);
  freeGraph(&G);
  fclose(in);
  fclose(out);
  return EXIT_SUCCESS;
}
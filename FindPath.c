// Zesen Feng
// zefeng
// pa4
// CSE101
// FindPath.c
// creates Graph ADT based on input file.
// outputs shortest paths
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
    addEdge(G, u, v);
    fscanf(in, "%d %d", &u, &v);
  }

  printGraph(out, G);

  fprintf(out, "\n");

  int origin;
  int terminus;

  fscanf(in, "%d %d", &origin, &terminus);
  
  List L = newList();

  while( terminus > 0 && origin >0){
    BFS(G, origin);
    if(getDist(G, terminus) == INF){
      fprintf(out, "The distance from %d to %d is infinity\n", origin, terminus);
      fprintf(out, "No %d-%d path exists",origin ,terminus);
    }
    else {
      fprintf(out, "The distance from %d to %d is %d\n", origin, terminus, getDist(G, terminus));
      fprintf(out, "A shortest %d-%d path is:",origin ,terminus);
      
      clear(L);
      getPath(L, G, terminus);
      moveFront(L);
      while(index(L) != -1){
        fprintf(out, " %ld", get(L));
        moveNext(L);
      }
    }
    fprintf(out, "\n\n");
    fscanf(in, "%d %d", &origin, &terminus);
  }

  freeList(&L);
  freeGraph(&G);
  fclose(in);
  fclose(out);
}
// Zesen Feng
// zefeng
// pa5
// Graph.c
// CSE101
// header file for Graph ADT
#ifndef _GRAPH_H_INCLUDE_
#define _GRAPH_H_INCLUDE_
#define NIL 0
#define UNDEF -1
#include "List.h"
// Exported type -------------------------------------------------------------
typedef struct GraphObj* Graph;

/*** Constructors-Destructors ***/

// newGraph();
// allocates memeory for Graph with n verticies and no edges.
Graph newGraph(int n);

// freeGraph();
// frees all allocated memory given Graph
void freeGraph(Graph* pG);

/*** Access functions ***/

// getOrder();
// returns order of given Graph
// order is the number of vertices
int getOrder(Graph G);

// getSize();
// returns size of given Graph
// size is the number of edeges
int getSize(Graph G);

// getParent();
// PRE: 1 =< u =< getOrder(G)
// the parent of vertex u in the DFS forest created by DFS()
// NIL if DFS() has not yet been called
int getParent(Graph G, int u);

// getDiscover();
// PRE: 1 =< u =< getOrder(G)
// the discover time of vertex u
// UNDEF if DFS() has not yet been called
int getDiscover(Graph G, int u); 

// getFinish();
// PRE: 1 =< u =< getOrder(G)
// the discover time of vertex u
// UNDEF if DFS() has not yet been called
int getFinish(Graph G, int u);

/*** Manipulation procedures ***/

// addEdge();
// PRE: 1 =< u =< getOrder(G)
// PRE: 1 =< v =< getOrder(G)
// inserts a new edge joining u to v
// u is added to the adjacency List of v, and v to the adjacency List of u
// lists sorted by increasing labels
void addEdge(Graph G, int u, int v);

// addArc();
// PRE: 1 =< u =< getOrder(G)
// PRE: 1 =< v =< getOrder(G)
// inserts a new directed edge from u to v
// v is added to the adjacency List of u (but not u to the adjacency List of v)
void addArc(Graph G, int u, int v);

// DFS();
// runs the DFS algorithm on the Graph G with order convention in S 
// setting the color, parent,and discover, finish fields of G accordingly
void DFS(Graph G, List S);

/*** Other operations ***/

// transpose();
// returns a reference to a new graph object representing the transpose of G,
Graph transpose(Graph G);
// copyGraph();
// returns a reference to a new graph which is a copy of G
// will copy without running DFS
Graph copyGraph(Graph G);
// prints the adjacency list representation of G to the file pointed to by out.
void printGraph(FILE* out, Graph G);

#endif
// Zesen Feng
// zefeng
// pa5
// Graph.c
// CSE101
// source code for functions in Graph ADT
#include <stdio.h>
#include <stdlib.h>
#include "Graph.h"
#include "List.h"

// Structs -----------------------------------------------------------

typedef struct GraphObj{
	List* adj;
	int* color;
	int* parent;
	int* discover;
	int* finish;
	int order;
	int size; 

} GraphObj;
int Visit(Graph G, int x, int time, List stack);
// freeGraph();
// frees all allocated memory given Graph
// each 0 index of array is not used. 
// graph must have at least one vertex
// source = -1 indicates no BFS call.
// white gray black is -1, 0 , 1
Graph newGraph(int n){
	if(n < 1){
		fprintf(stderr, "Graph Error: trying to create a graph with no vertices\n");
		exit(EXIT_FAILURE);
	}
	Graph G = malloc(sizeof(GraphObj));

	G->adj = calloc(n+1,sizeof(List));
	G->color = calloc(n+1,sizeof(int));
	G->parent = calloc(n+1,sizeof(int));
	G->discover = calloc(n+1,sizeof(int));
	G->finish = calloc(n+1,sizeof(int));

	for (int i = 1; i < n+1; ++i){
		G->adj[i] = newList();
		G->color[i] = -1;
		G->parent[i] = NIL;
		G->discover[i] = UNDEF;
		G->finish[i] = UNDEF;
	}

	G->order = n;
	G->size = 0;

	return G;
}

// freeGraph();
// frees all allocated memory given Graph
void freeGraph(Graph* pG){
	if(pG!=NULL && (*pG)!=NULL){
		for (int i = 1; i < getOrder(*pG)+1; ++i){
			freeList(&((*pG)->adj[i]));
	}
		free((*pG)->adj);
		free((*pG)->color);
		free((*pG)->parent);
		free((*pG)->discover);
		free((*pG)->finish);

		free(*pG);
		*pG = NULL;
	}
	return;
}

/*** Access functions ***/

// getOrder();
// returns order of given Graph
// order is the number of vertices
int getOrder(Graph G){
	if(G == NULL){
		fprintf(stderr, "Graph Error: calling getOrder() on NULL Graph reference\n");
		exit(EXIT_FAILURE);
	}
	return G->order;
}

// getSize();
// returns size of given Graph
// size is the number of edeges
int getSize(Graph G){
	if(G == NULL){
		fprintf(stderr, "Graph Error: calling getSize() on NULL Graph reference\n");
		exit(EXIT_FAILURE);
	}
	return G->size;
}

// getParent();
// PRE: 1 =< u =< getOrder(G)
// the parent of vertex u in the DFS forest created by DFS()
// NIL if DFS() has not yet been called
int getParent(Graph G, int u){
	if(G == NULL){
		fprintf(stderr, "Graph Error: calling getParent() on NULL Graph reference\n");
		exit(EXIT_FAILURE);
	}
	if(u < 1 || u > getOrder(G)){
		fprintf(stderr, "Graph Error: calling getParent() on invalid vertex\n");
		exit(EXIT_FAILURE);
	}
	else
		return G->parent[u];
	// constructed sets distance to NIL
	// if DFS is not called it remains NIL
}

// getDiscover();
// PRE: 1 =< u =< getOrder(G)
// the discover time of vertex u
// UNDEF if DFS() has not yet been called
int getDiscover(Graph G, int u){
	if(G == NULL){
		fprintf(stderr, "Graph Error: calling getDiscover() on NULL Graph reference\n");
		exit(EXIT_FAILURE);
	}
	if(u < 1 || u > getOrder(G)){
		fprintf(stderr, "Graph Error: calling getDiscover() on invalid vertex\n");
		exit(EXIT_FAILURE);
	}
	else
		return G->discover[u];
} 

// getFinish();
// PRE: 1 =< u =< getOrder(G)
// the finish time of vertex u
// UNDEF if DFS() has not yet been called
int getFinish(Graph G, int u){
	if(G == NULL){
		fprintf(stderr, "Graph Error: calling getFinish() on NULL Graph reference\n");
		exit(EXIT_FAILURE);
	}
	if(u < 1 || u > getOrder(G)){
		fprintf(stderr, "Graph Error: calling getFinish() on invalid vertex\n");
		exit(EXIT_FAILURE);
	}
	else
		return G->finish[u];
}

/*** Manipulation procedures ***/

// addEdge();
// PRE: 1 =< u =< getOrder(G)
// PRE: 1 =< v =< getOrder(G)
// inserts a new edge joining u to v
// u is added to the adjacency List of v, and v to the adjacency List of u
// lists sorted by increasing labels
void addEdge(Graph G, int u, int v){
	if(G == NULL){
		fprintf(stderr, "Graph Error: calling addEdge() on NULL Graph reference\n");
		exit(EXIT_FAILURE);
	}
	if(u < 1 || u > getOrder(G)){
		fprintf(stderr, "Graph Error: calling addEdge() on invalid u vertex\n");
		exit(EXIT_FAILURE);
	}
	if(u < 1 || v > getOrder(G)){
		fprintf(stderr, "Graph Error: calling addEdge() on invalid v vertex\n");
		exit(EXIT_FAILURE);
	}
	addArc(G, u, v);
	addArc(G, v, u);
	G->size--;
	return;
}

// addArc();
// PRE: 1 =< u =< getOrder(G)
// PRE: 1 =< v =< getOrder(G)
// inserts a new directed edge from u to v
// u is origin v is terminus
// v is added to the adjacency List of u (but not u to the adjacency List of v)
void addArc(Graph G, int u, int v){
	if(G == NULL){
		fprintf(stderr, "Graph Error: calling addArc() on NULL Graph reference\n");
		exit(EXIT_FAILURE);
	}
	if(u < 1 || u > getOrder(G)){
		fprintf(stderr, "Graph Error: calling addArc() on invalid origin\n");
		exit(EXIT_FAILURE);
	}
	if(u < 1 || v > getOrder(G)){
		fprintf(stderr, "Graph Error: calling addArc() on invalid terminus\n");
		exit(EXIT_FAILURE);
	}

	List L = G->adj[u];

	// insert v into adj[u]
	// maintain sorted order
	moveFront(L);
	while(index(L) != -1){
		if(get(L) < v)
			moveNext(L);
		else if(get(L) == v)
			return;
		else{
			insertBefore(L, v);
			G->size++;
			return;
		}
	}
	append(L, v);
	G->size++;
	return;
}


// DFS();
// runs the DFS algorithm on the Graph G with order convention in S 
// setting the color, parent,and discover, finish fields of G accordingly
void DFS(Graph G, List S){
	if(G == NULL){
		fprintf(stderr, "Graph Error: calling DFS() on NULL Graph reference\n");
		exit(EXIT_FAILURE);
	}
	if(getOrder(G) != length(S)){
		fprintf(stderr, "Graph Error: calling DFS() on invalid Stack List\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 1; i < getOrder(G)+1; ++i){
		G->color[i] = -1;
		G->parent[i] = NIL;
	}
	int time = 0;
	// visit will append verticies based on finish time
	int k = getOrder(G);
	for (int i = 1; i <= k; ++i){
		moveBack(S);// these two loops move to the top of the orignal stack
		for (int j = 1; j+i <= k; ++j){
			movePrev(S);
		}

		int x = get(S);

		if(G->color[x] == -1)
			time = Visit(G, x, time, S);
		delete(S);
	}
	return;
}

/*** Other operations ***/

// transpose();
// returns a reference to a new graph object representing the transpose of G,
Graph transpose(Graph G){
	if(G == NULL){
		fprintf(stderr, "Graph Error: calling transpose() on NULL Graph reference\n");
		exit(EXIT_FAILURE);
	}
	Graph H = newGraph(getOrder(G));
	for (int i = 1; i <= getOrder(G); ++i)
	{
		List L = G->adj[i];
		moveFront(L);
		while(index(L) != -1){
			addArc(H, get(L), i);
			moveNext(L);
		}
	}
	return H;
}

// copyGraph();
// returns a reference to a new graph which is a copy of G
// will copy without running DFS
Graph copyGraph(Graph G){
	if(G == NULL){
		fprintf(stderr, "Graph Error: calling copyGraph() on NULL Graph reference\n");
		exit(EXIT_FAILURE);
	}
	Graph H = newGraph(getOrder(G));
	for (int i = 1; i <= getOrder(G); ++i){
		List L = G->adj[i];
		moveFront(L);
		while(index(L) != -1){
			append(H->adj[i], get(L));
			moveNext(L);
		}
	}
	return H;
}

// printGraph();
// prints the adjacency list representation of G to the file pointed to by out.
void printGraph(FILE* out, Graph G){
	if(G == NULL){
		fprintf(stderr, "Graph Error: calling printGraph() on NULL Graph reference\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 1; i < getOrder(G)+1; ++i){
		List L = G->adj[i];
		fprintf(out, "%d:", i);
		moveFront(L);
		while(index(L) != -1){
			fprintf(out, " %ld", get(L));
			moveNext(L);
		}
		fprintf(out, "\n");
	}
	return; 
}

// Visit();
// private helper function of DFS
int Visit(Graph G, int x, int time, List stack){
	G->discover[x] = ++time;
	G->color[x] = 0;

	List L = G->adj[x];
	moveFront(L);
	while(index(L) != -1){
		int y = get(L);
		if(G->color[y] == -1){
			G->parent[y] = x;
			time = Visit(G, y, time, stack);
		}
		moveNext(L);
	}
	G->color[x] = 1;
	G->finish[x] = ++time;
	prepend(stack, x); 
	return time;
}
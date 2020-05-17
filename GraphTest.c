// Zesen Feng
// zefeng
// pa5
// CSE101
// GraphTest.c
// Test Client for Graph ADT


#include<stdio.h>
#include<stdlib.h>
#include"Graph.h"
#include"List.h"

int main(int argc, char* argv[]){

	Graph B = newGraph(100);

        addArc(B, 64, 4);
        addArc(B, 64, 3);
        addArc(B, 42, 2);
        addArc(B, 2, 64);
        addArc(B, 4, 2);
        addArc(B, 3, 42);


        //fprintf(stdout, "%d\n", getDist(B, 2));
	
	//printGraph(stdout, B);
	freeGraph(&B);

	Graph C = newGraph(100);

    addArc(C, 64, 4);
    addArc(C, 64, 3);
	addArc(C, 42, 2);
	addArc(C, 2, 64);
	addArc(C, 4, 2);
	addArc(C, 3, 42);
        
	List stack = newList();
	for(int i = 1; i <= 100; ++i){
                prepend(stack, i);
	}

	DFS(C, stack);
	printList(stdout, stack);
	printf("\n");
	fprintf(stdout, "getDiscover(C,100) %d\n", getDiscover(C,100));
	fprintf(stdout, "getDiscover(C,64) %d\n", getDiscover(C,64));
	fprintf(stdout, "getDiscover(C,4) %d\n", getDiscover(C,4));


	fprintf(stdout, "2nd DFS\n");
	DFS(C, stack);
	printList(stdout, stack);
	printf("\n");
	fprintf(stdout, "getDiscover(C,4) %d\n", getDiscover(C,4));



	freeGraph(&C);
    freeList(&stack);



	return(EXIT_SUCCESS);
}
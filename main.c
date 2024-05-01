#include<stdio.h>
#include <stdlib.h>
#include<time.h>
#include "hmm.h"

#define MAX_SIZE 6000
#define ALET     1000
#define LOCATIONS 100
#define MALLOC   0
#define CALLOC   1
#define REALLOC   2
#define FREE   4
int main() {
    void *ptr[LOCATIONS] = {NULL};
    srand((time(NULL)));
    int loc;
    int size;
    int op ;
    printf("Test starts with \nIlitrations : %d\nMax size :  %d\nLocations :  %d\n",ALET,MAX_SIZE,LOCATIONS);
    printf("\n*************************************\n");
    for(int i =0 ;i<ALET;i++) {
        loc = rand() % LOCATIONS;
        size = rand() % MAX_SIZE;
        op = rand() % 5;

        if(op == MALLOC) {
            ptr[loc] = myMalloc(size);
            printf("[%d] Malloc done,new block at %x with size %d bytes \n",i,ptr[loc],size);
        }

        else if(op == CALLOC) {
            ptr[loc] = myCalloc(size/sizeof(int),sizeof(int));
            printf("[%d] calloc done,new block at %x with size %d bytes \n",i,ptr[loc],size);
        }

        else if(op == REALLOC) {
            ptr[loc] = myRealloc(ptr[loc],size);
            printf("[%d] realloc done,new block at %x with size %d bytes \n",i,ptr[loc],size);
        }

        else if(op == FREE) {
            printf("[%d] For Location %x",i,ptr[loc]);
            myFree(ptr[loc]);
            printf(" ,Free done\n");
        }
    }
    printf("Press any char to continue test\n");
    getchar();
    for(int i = 0;i<LOCATIONS;i++) {
        if(BLOCK_ACCESS(ptr[i] - HEADER_SIZE )->count != 0) {
            printf("[%d] For Location %x",i,ptr[i]);
            myFree(ptr[i]);
            printf(" ,Free done\n");
        }
    }
    printf("\n*************************************\n");
    printf("Test Finished\n");
    return 0;
}
/*
======================================================================================
Name        : hmm.c
Author      : Mohamed Ezzat
Date        : 22/4/2024
=======================================================================================
 */

/***************************************************************************************
 Inlcudes : -
***************************************************************************************/

#include <stdio.h>
#include <unistd.h> /* sbrk syscall */
#include "hmm.h"
#include <string.h> /* memcpy */

/***************************************************************************************
 Global Variables : -
***************************************************************************************/

/* A variable to store the start address of our heap */
static void *heap_start = NULL_PTR;

/* A variable to store the end address of our heap */
static void *heap_end = NULL_PTR;

/***************************************************************************************
 Local Functions Prototype : -
***************************************************************************************/

/* [ Prototype ]   : static bool_t splite(block_t* block,uint64 req_size) ;
 * [ Description ] : A function used to splite the block into two smaller blocks
 * [ Inputs ]      : the block handle and the size of the required block
 * [ Output ]      : True if the block is splited into two blocks
 *                 : False if the block size is bigger than the required size by less than
 *                 : the header size
 */
static bool_t splite(block_t* block,uint64 req_size) ;

/* [ Prototype ]   : static uint8 merge(block_t* block);
 * [ Description ] : A function used to merge the block with the next or previous block if they
 *                 : are empty, and if the size of that block exceeds the max size of block,then
 *                 : the block will be shrinked and the end of heap will move back
 * [ Inputs ]      : the block handle
 * [ Output ]      : Number of blocks merged.
 */
static uint8 merge(block_t* block);

/* [ Prototype ]   : static void *allocate(uint64 req_size);
 * [ Description ] : A Function used to allocate a new block in the heap
 * [ Inputs ]      : the block required size
 * [ Output ]      : the block handle, or NULL_PTR in case of an error.
 */
static void *allocate(uint64 req_size);

#if MODE == STANDARD_MODE
static void *myMalloc(uint64 req_size);

static void *myCalloc(uint64 no,uint64 size);

static void *myRealloc(void *ptr,uint64 size);

static void myFree(void *ptr);

void* malloc(uint64 size){
	return myMalloc(size);
}


void* calloc(uint64 n,uint64 size){
	return myCalloc(n,size);
}


void *realloc(void *ptr,uint64 size){
	return myRealloc(ptr,size);
}

void free(void *ptr){
	myFree(ptr);
}

#endif

/***************************************************************************************
 Functions Definition : -
***************************************************************************************/

#if MODE == STANDARD_MODE
void myFree(void *ptr) {
#else
bool_t myFree(void *ptr) {
#endif
 if(ptr != NULL_PTR && BLOCK_ACCESS(ptr - HEADER_SIZE)->count  !=0 ) {
  ptr = ptr - HEADER_SIZE;
  merge(BLOCK_ACCESS(ptr));
#if MODE == NORMAL_MODE
  return TRUE;
#endif
 }
#if MODE == NORMAL_MODE
 return FALSE;
#endif
}

void *myMalloc(uint64 req_size) {
 if(NULL_PTR == heap_start) {
  heap_start = sbrk(NEW_BLOCK_SIZE);
  heap_end = sbrk( 0) - HEADER_SIZE - 1;
  BLOCK_ACCESS(heap_start) ->next = heap_end;
  BLOCK_ACCESS(heap_start) ->prev = NULL_PTR;
  BLOCK_ACCESS(heap_start) ->count = 0;
  BLOCK_ACCESS(heap_start) ->size = NEW_BLOCK_SIZE - HEADER_SIZE;
 }
 if(req_size > 0) {
  void *ptr = allocate(ALIGN(req_size+HEADER_SIZE));
  return ptr==NULL_PTR?NULL_PTR:ptr+HEADER_SIZE;
 }
 return NULL_PTR;
}

void *myRealloc(void *ptr,uint64 size) {
 if(size == 0) {
  myFree((ptr));
  return  NULL_PTR;
 }
 if(ptr == NULL_PTR ) {
  void *new_block = myMalloc(size);
  return new_block;
 }
 if(ALIGN(size) + HEADER_SIZE <= BLOCK_ACCESS(ptr - HEADER_SIZE)->size) {
  return ptr;
 }
 if(ALIGN(size) + HEADER_SIZE > BLOCK_ACCESS(ptr - HEADER_SIZE)->size) {
  void *new_block = myMalloc(size);
  memcpy(new_block,ptr,BLOCK_ACCESS(ptr - HEADER_SIZE)->size);
  myFree(ptr);
  return new_block;
 }
}

void *myCalloc(uint64 no,uint64 size) {
void *ptr = myMalloc(no*size);
 if(ptr !=NULL)
 memset(ptr,0,BLOCK_ACCESS(ptr-HEADER_SIZE)->size);
 else
  return NULL;
}

void increment(block_t *block) {
  block->count++;
}

void decrement(block_t *block) {
 block->count--;
 if(block->count == 0 ) {
  myFree(block);
 }
}

/***************************************************************************************
 Local Functions Definition : -
***************************************************************************************/

/* [ Prototype ]   : static void *allocate(uint64 req_size);
 * [ Description ] : A Function used to allocate a new block in the heap
 * [ Inputs ]      : the block required size
 * [ Output ]      : the block handle, or NULL_PTR in case of an error.
 */
static void* allocate(uint64 req_size) {
  block_t *temp = heap_start;
 while( temp != NULL && temp->next < heap_end && temp->next!= NULL ) {
  if(temp->count != 0 || temp->size < req_size) {
   temp = temp->next;
  }
  else {
   temp->count = 1;
   splite(temp,req_size);
    return temp;
  }
 }
while(req_size >= temp->size + HEADER_SIZE) {
 sbrk(NEW_BLOCK_SIZE);
 heap_end = sbrk( 0);
 temp->size += NEW_BLOCK_SIZE;
}
 heap_end -=HEADER_SIZE;
 temp->next = heap_end;
   splite(temp,req_size);
   temp->count = 1;
   return temp;
}


/* [ Prototype ]   : static bool_t splite(block_t* block,uint64 req_size) ;
 * [ Description ] : A function used to splite the block into two smaller blocks
 * [ Inputs ]      : the block handle and the size of the required block
 * [ Output ]      : True if the block is splited into two blocks
 *                 : False if the block size is bigger than the required size by less than
 *                 : the header size
 */
static bool_t splite(block_t* block,uint64 req_size) {

 if(block != NULL_PTR && req_size > 1 && block->size > req_size + HEADER_SIZE) {
  block_t *new_block = BLOCK_ACCESS( (uint8*)block + (req_size) + HEADER_SIZE );
  if(block->next == NULL_PTR || block->next < heap_end) {
   new_block->next = NULL_PTR;
  }
  else {
   new_block->next = block->next;
  }
  block->next = new_block;
  new_block->prev = (void*)block;
  new_block->size = block->size - req_size - 2 * HEADER_SIZE;
  block->size = req_size;
  if(new_block ->next != heap_end) {
   BLOCK_ACCESS(new_block->next)->prev = new_block;
  }
  return TRUE;
 }

 else {
  return FALSE;
 }

}

/* [ Prototype ]   : static uint8 merge(block_t* block);
 * [ Description ] : A function used to merge the block with the next or previous block if they
 *                 : are empty, and if the size of that block exceeds the max size of block,then
 *                 : the block will be shrinked and the end of heap will move back
 * [ Inputs ]      : the block handle
 * [ Output ]      : Number of blocks merged.
 */
static uint8 merge(block_t* block) {
 uint8 blocks = 0;
 if(block != NULL) {
  /* next node is empty */
 if(block->next != NULL && block->next < heap_end && BLOCK_ACCESS(block->next)->count == 0) {
  block->size += BLOCK_ACCESS(block->next)->size + HEADER_SIZE;
  block->next = BLOCK_ACCESS(block->next)->next;
  if(block->next != NULL_PTR) {
   BLOCK_ACCESS(block->next)->prev = block;
  }
  BLOCK_ACCESS(block)->count = 0;
  blocks++;
 }
  /* previous node is empty */
  if(block->prev != NULL && BLOCK_ACCESS(block->prev)->count == 0) {
   BLOCK_ACCESS(block->prev)->size += block->size + HEADER_SIZE;
   BLOCK_ACCESS(block->prev)->next = block->next;
   if(BLOCK_ACCESS(block->next) != NULL) {
    BLOCK_ACCESS(block->next)->prev = block->prev;
   }
   BLOCK_ACCESS(block->prev)->count = 0;
   blocks++;
  }
  while(block->size > NEW_BLOCK_SIZE && block->next == NULL_PTR) {
   brk(heap_end - NEW_BLOCK_SIZE + HEADER_SIZE);
   block->size -=NEW_BLOCK_SIZE;
   heap_end = sbrk(0) - HEADER_SIZE - 1;
  }
  return blocks;
 }
 return FALSE;
}

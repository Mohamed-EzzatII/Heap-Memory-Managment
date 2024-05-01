/*
======================================================================================
Name        : hmm.h
Author      : Mohamed Ezzat
Date        : 22/4/2024
=======================================================================================
 */

#ifndef HMM_H_
#define HMM_H_

/***************************************************************************************
 Inlcudes : -
***************************************************************************************/

#include "std_types.h"

/***************************************************************************************
 Defines : -
***************************************************************************************/

/* Operating M0de */
#define STANDARD_MODE 0
#define NORMAL_MODE   1
#define MODE         STANDARD_MODE

/* size of word */
#define WORD_LENGTH         64

/* size of meta data */
#define HEADER_SIZE         32

/* the size at which the heap is increased by at each time we call sbrk */
#define NEW_BLOCK_SIZE      64*WORD_LENGTH

/***************************************************************************************
 MACROS : -
***************************************************************************************/

/* macro to allign the new allocated block to be n of words */
#define ALIGN(size)       ( ( (size) + (WORD_LENGTH - 1) ) & (~ (WORD_LENGTH - 1)) )

/* just a macro to cast any pointer to block type */
#define BLOCK_ACCESS(block)   ( (block_t*) (block) )

/***************************************************************************************
 Datatypes Definition : -
***************************************************************************************/

/* a datatype to define block metadata */
typedef struct {
 void *next;
 void *prev;
 uint64 count;/* number of pointers points to the block */
 uint64 size; /* size of block in words without header */
}block_t;

/***************************************************************************************
 Functions Prototypes : -
***************************************************************************************/

#if MODE == STANDARD_MODE
void free(void *ptr);
void *malloc(uint64 size);
void *calloc(uint64 n,uint64 size);
void *realloc(void *ptr,uint64 size);

#else
bool_t myFree(void *ptr);

void increment(block_t *block);

void decrement(block_t *block);

void *myMalloc(uint64 req_size);

void *myCalloc(uint64 no,uint64 size);

void *myRealloc(void *ptr,uint64 size);

#endif


#endif

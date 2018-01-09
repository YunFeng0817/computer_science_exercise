/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
        /* Team name */
        "ateam",
        /* First member's full name */
        "Harry Bovik",
        /* First member's email address */
        "bovik@cs.cmu.edu",
        /* Second member's full name (leave blank if none) */
        "",
        /* Second member's email address (leave blank if none) */
        ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Basic constants and macros */
#define WSIZE       4       /* word size (bytes) */
#define DSIZE       8       /* doubleword size (bytes) */
#define CHILDSIZE   8       /* the child pointer size */
#define CHUNKSIZE  (1<<12)  /* initial heap size (bytes) */
#define OVERHEAD    8       /* overhead of header and footer (bytes) */

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)       (*(size_t *)(p))
#define PUT(p, val)  (*(size_t *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       (bp - WSIZE)
#define FTRP(bp)       (bp + GET_SIZE(HDRP(bp)) - DSIZE)

/*Given unused block ptr bp, get address of its left child and right child*/
#define GET_LCHILD(bp)  ((char*)(bp))
#define GET_RCHILD(bp)  ((char*)(bp+CHILDSIZE))

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  (bp + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp)  (bp - GET_SIZE((bp - DSIZE)))

/* $end mallocmacros */

/* Global variables */
char *heap_listp;  /* pointer to first block */
char *root = NULL;  /*the root of the tree(unused blocks)*/

/*Fuction not in head file*/
static void *extend_heap(size_t words);
static void insert(char **tree, char *place);  /*given the length of the clock ,insert it into the tree*/
static void delete(char **tree);
static void *search(char *tree, size_t works);
static void place(void *bp, size_t asize);
static void *coalesce(void *ptr);

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == NULL)
        return -1;
    PUT(heap_listp, 0);                        /* alignment padding */
    PUT(heap_listp + WSIZE, PACK(OVERHEAD, 1));  /* prologue header */
    PUT(heap_listp + DSIZE, PACK(OVERHEAD, 1));  /* prologue footer */
    PUT(heap_listp + WSIZE + DSIZE, PACK(0, 1));   /* epilogue header */
    heap_listp += DSIZE;

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;
    heap_listp += WSIZE;
    PUT(heap_listp, PACK(CHUNKSIZE / WSIZE, 0));
    PUT(heap_listp + CHUNKSIZE / WSIZE - WSIZE, PACK(CHUNKSIZE / WSIZE, 0));
    PUT(heap_listp + DSIZE, 0); /*set the left child to null*/
    PUT(heap_listp + WSIZE + DSIZE, 0);  /*set the right child to null*/
    insert(&root, heap_listp);
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    size_t asize;      /* adjusted block size */
    size_t extendsize; /* amount to extend heap if no fit */
    char *bp;

    /* Ignore spurious requests */
    if (size <= 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = DSIZE + OVERHEAD;
    else
        asize = DSIZE * ((size + (OVERHEAD) + (DSIZE - 1)) / DSIZE);

    if ((bp = search(root, asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) {
    size_t size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    ptr = coalesce(ptr);
    insert(&root, ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    copySize = GET_SIZE(oldptr);
    if (size < copySize)
        copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}


/*
 * coalesce - boundary tag coalescing. Return ptr to coalesced block
 */
static void *coalesce(void *ptr) {
    size_t size;
    char *pr, *next;
    pr = PREV_BLKP(ptr);
    next = NEXT_BLKP(ptr);
    /*if both behind block and behind block empty,three blocks coalesce*/
    if (!GET_ALLOC(pr) && !GET_ALLOC(next)) {
        size = GET_SIZE(ptr) + GET_SIZE(pr) + GET_SIZE(next);
        delete(PREV_BLKP(ptr));
        PUT(HDRP(pr), PACK(size, 0));
        PUT(FTRP(next), PACK(size, 0));
        insert(&root, pr);
        return pr;
    }
        /*only behind block empty,two blocks coalesce*/
    else if (!GET_ALLOC(pr)) {
        size = GET_SIZE(ptr) + GET_SIZE(pr);
        delete(PREV_BLKP(ptr));
        PUT(HDRP(pr), PACK(size, 0));
        PUT(FTRP(ptr), PACK(size, 0));
        insert(&root, pr);
        return pr;
    }
        /*only behind block empty,two blocks coalesce*/
    else if (!GET_ALLOC(next)) {
        size = GET_SIZE(ptr) + GET_SIZE(next);
        delete(PREV_BLKP(ptr));
        PUT(HDRP(ptr), PACK(size, 0));
        PUT(FTRP(next), PACK(size, 0));
        insert(&root, ptr);
    }
    return ptr;
}

/*
 * place - Place block of asize bytes at start of free block bp
 *         and split if remainder would be at least minimum block size
 */
static void place(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) > (DSIZE + OVERHEAD)) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(HDRP(bp) + asize - WSIZE, PACK(asize, 1));
        bp = bp + asize;
        PUT(bp, PACK(csize - asize, 0));
        PUT(bp + csize - asize - WSIZE, PACK(csize - asize, 0));
        insert(&root, bp + WSIZE);
    } else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

/*
 * insert - Insert the unused block to the BST
 */
void insert(char **tree, char *place) {
    static char flag = 0;
    if (*tree == NULL) {
        *tree = place;
        return;
    }
    if (GET_SIZE(*tree) > GET_SIZE(HDRP(place))) {
        return insert(tree, place);
    } else if (GET_SIZE(*root) < GET_SIZE(HDRP(place))) {
        return insert(tree + CHILDSIZE, place);
    } else {
        flag = !flag;
        switch (flag) {
            case 0:
                return insert(tree, place);
            case 1:
                return insert(tree + WSIZE, place);
            default:;
        }
    }
}

/*
 * search - Search the specific length unused block in the BST
 */
void *search(char *tree, size_t works) {
    if (*tree == NULL)
        return NULL;
    if (GET_SIZE(tree) > works) {
        if(*tree==NULL)
        {
            delete(&tree);
            return tree;
        }
        else{
            return search((char*)*tree,works);
        }
    }
    else if(GET_SIZE(tree)<works)
    {
        if(*(tree+CHILDSIZE)==NULL)
        {
            return NULL;
        }
        else
        {
            return search((char*)(*tree+CHILDSIZE),works);
        }
    }
    else{
        delete(&tree);
        return tree;
    }
}

/*
 * delete - Delete the unused block in the BST
 */
static void delete(char **tree) {
    if (**tree == NULL && *(*tree + CHILDSIZE) == NULL) {
        if (*tree == root)
            root = NULL;
        else {
            *tree = NULL;
        }
    } else if (**tree == NULL) {
        *tree = (char *) *(*tree + CHILDSIZE);
    } else if (*(*tree + CHILDSIZE) == NULL) {
        *tree = (char *) **tree;
    } else {
        char *pr, *p;
        pr = p = (char *) *(*tree + CHILDSIZE);
        while (*(p + CHILDSIZE)) {
            pr = p;
            p = (char *) *(pr + CHILDSIZE);
        }
        if (pr != p) {
            *pr = *(p + CHILDSIZE);
        } else {
            *(*tree + CHILDSIZE) = *(p + CHILDSIZE);
        }
    }
}












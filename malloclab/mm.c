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
#define WSIZE       8       /* word size (bytes) */
#define DSIZE       16       /* doubleword size (bytes) */
#define CHILDSIZE   8       /* the child pointer size */
#define CHUNKSIZE  (1<<15)  /* initial heap size (bytes) */
#define LEFT    1
#define RIGHT   0

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
#define HDRP(bp)       ((char *)bp - WSIZE)
#define FTRP(bp)       ((char *)bp + GET_SIZE(HDRP(bp)) - DSIZE)

/*Given unused block ptr bp, get address of its left child and right child*/
#define SET_LCHILD(bp, val)  (*(long*)(bp)=val)
#define SET_RCHILD(bp, val)  (*(long*)((char*)bp+CHILDSIZE)=val)
#define GET_LCHILD(bp)      *((long *)(bp))
#define GET_RCHILD(bp)      *((long *)((char*)bp+CHILDSIZE))

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)bp + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp)  ((char *)bp - GET_SIZE(((char *)bp - DSIZE)))

/* $end mallocmacros */

/* Global variables */
static long *heap_listp;  /* pointer to first block */
static long *root = NULL;  /*the root of the tree(unused blocks)*/
static long *pre_root = NULL;   /*storage the previous node of the BST node*/
static char left_or_right;  /*storage the current node is left child or right child to its parent node*/

/*Fuction not in head file*/
static void *extend_heap(size_t words);

static void insert(long *tree, long *place);  /*given the length of the clock ,insert it into the tree*/

static void delete(long *tree);

static void *search(long *tree, size_t works);

static void place(long *bp, size_t asize);

static void *coalesce(long *ptr);

static void delete_node(long *tree, long *place);

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if ((heap_listp = extend_heap(CHUNKSIZE / WSIZE)) == NULL)
        return -1;
    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    size_t asize;      /* adjusted block size */
    size_t extendsize; /* amount to extend heap if no fit */
    long *bp;

    /* Ignore spurious requests */
    if (size <= 0)
        return NULL;

    asize = (size + 7 - (size + 7) % 8) + DSIZE;


    if ((bp = search(root, asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    if ((bp = extend_heap(size / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) {
//    PUT(HDRP(ptr), PACK(size, 0));
//    PUT(FTRP(ptr), PACK(size, 0));
    //ptr = coalesce(ptr);
    long *temp = (long *) ptr;
    size_t size = GET_SIZE(HDRP(temp));
    PUT(HDRP(temp), PACK(size, 0));
    PUT(FTRP(temp), PACK(size, 0));
    SET_LCHILD(temp, 0);
    SET_RCHILD(temp, 0);
    insert(root,temp);
//    coalesce(temp);
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
static void *coalesce(long *ptr) {
    size_t size;
    long *pr, *next;
    size_t temp = GET_SIZE(((char *) ptr - DSIZE));
    pr = (long *) PREV_BLKP(ptr);
    next = (long *) NEXT_BLKP(ptr);
    /*if both behind block and behind block empty,three blocks coalesce*/
    if (pr > heap_listp && !GET_ALLOC(pr) && !GET_ALLOC(next)) {
        size = GET_SIZE(HDRP(ptr)) + GET_SIZE(HDRP(pr)) + GET_SIZE(HDRP(next));
        delete_node(root, pr);
        delete_node(root, next);
        PUT(HDRP(pr), PACK(size, 0));
        PUT(FTRP(next), PACK(size, 0));
        SET_LCHILD(pr, 0);
        SET_RCHILD(pr, 0);
        insert(root, pr);
        return pr;
    }
        /*only behind block empty,two blocks coalesce*/
    else if (!GET_ALLOC(pr)) {
        size = GET_SIZE(HDRP(ptr)) + GET_SIZE(HDRP(pr));
        delete_node(root, pr);
        PUT(HDRP(pr), PACK(size, 0));
        PUT(FTRP(ptr), PACK(size, 0));
        SET_LCHILD(pr, 0);
        SET_RCHILD(pr, 0);
        insert(root, pr);
        return pr;
    }
        /*only behind block empty,two blocks coalesce*/
    else if (!GET_ALLOC(next)) {
        size = GET_SIZE(HDRP(ptr)) + GET_SIZE(HDRP(next));
        delete_node(root, next);
        PUT(HDRP(ptr), PACK(size, 0));
        PUT(FTRP(next), PACK(size, 0));
        SET_LCHILD(ptr, 0);
        SET_RCHILD(ptr, 0);
        insert(root, ptr);
    } else
        insert(root, ptr);
    return ptr;
}

/*
 * place - Place block of asize bytes at start of free block bp
 *         and split if remainder would be at least minimum block size
 */
static void place(long *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));
    long *rest;

    if ((csize - asize) > (2 * DSIZE)) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT((char *) HDRP(bp) + asize - WSIZE, PACK(asize, 1));
        rest = (char*)bp + asize;
        PUT(HDRP(rest), PACK(csize - asize, 0));
        PUT(FTRP(rest), PACK(csize - asize, 0));
        SET_LCHILD(rest, 0);
        SET_RCHILD(rest, 0);
        insert(root, rest);
    } else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

/*
 * insert - Insert the unused block to the BST
 */
void insert(long *tree, long *place) {
    static char flag = 0;
    if (root == NULL) {
        root = place;
        return;
    }
    if (GET_SIZE(HDRP(tree)) > GET_SIZE(HDRP(place))) {
        flag = 0;
        if (GET_LCHILD(tree) == NULL) {
            GET_LCHILD(tree) = place;
            return;
        } else
            return insert(GET_LCHILD(tree), place);
    } else if (GET_SIZE(HDRP(tree)) < GET_SIZE(HDRP(place))) {
        flag = 0;
        if (GET_RCHILD(tree) == NULL) {
            GET_RCHILD(tree) = place;
            return;
        }
        return insert(GET_RCHILD(tree), place);
    } else {
        flag = !flag;
        switch (flag) {
            case 0:
                if (*tree == NULL) {
                    GET_LCHILD(tree) = place;
                    return;
                } else
                    return insert(GET_LCHILD(tree), place);
            case 1:
                if (GET_RCHILD(tree) == NULL) {
                    GET_RCHILD(tree) = place;
                    return;
                }
                return insert(GET_RCHILD(tree), place);
            default:;
        }
    }
}

/*
 * search - Search the specific length unused block in the BST
 */
void *search(long *tree, size_t works) {
    if (root == NULL)
        return NULL;
    if(tree==root)
        pre_root=root;
    if (GET_SIZE(HDRP(tree)) > works) {
        if (GET_LCHILD(tree) == NULL) {
            long *temp;
            temp=tree;
            delete(tree);
            return temp;
        } else {
            left_or_right = LEFT;
            pre_root = tree;
            return search(GET_LCHILD(tree), works);
        }
    } else if (GET_SIZE(HDRP(tree)) < works) {
        if (GET_RCHILD(tree) == NULL) {
            if(pre_root!=NULL){
                long *temp;
                temp=pre_root;
                delete_node(root,pre_root);
                return temp;
            }
            return NULL;
        } else {
            left_or_right = RIGHT;
            pre_root = tree;
            return search(GET_RCHILD(tree), works);
        }
    } else {
        long *temp;
        temp = tree;
        delete(tree);
        return temp;
    }
}

/*
 * delete - Delete the unused block in the BST
 */
static void delete(long *tree) {
    if (tree == root) {
        root = NULL;
        return;
    }
    if (GET_LCHILD(tree) == NULL && GET_RCHILD(tree) == NULL) {
        switch (left_or_right) {
            case LEFT:
                GET_LCHILD(pre_root) = NULL;
                break;
            case RIGHT:
                GET_RCHILD(pre_root) = NULL;
            default:;
        }
    } else if (GET_LCHILD(tree) == NULL) {
        switch (left_or_right) {
            case LEFT:
                GET_LCHILD(pre_root) = GET_RCHILD(tree);
                break;
            case RIGHT:
                GET_RCHILD(pre_root) = GET_RCHILD(tree);
            default:;
        }
    } else if (GET_RCHILD(tree) == NULL) {
        switch (left_or_right) {
            case LEFT:
                GET_LCHILD(pre_root) = GET_LCHILD(tree);
                break;
            case RIGHT:
                GET_RCHILD(pre_root) = GET_LCHILD(tree);
            default:;
        }
    } else {
        long *pr, *p;
        pr = p = GET_RCHILD(tree);
        while (GET_RCHILD(p)) {
            pr = p;
            p = GET_RCHILD(p);
        }

        if (pr != p) {
            GET_LCHILD(pr) = GET_RCHILD(p);
            GET_LCHILD(p) = GET_LCHILD(tree);
            GET_RCHILD(p) = GET_RCHILD(tree);
        } else {
            GET_LCHILD(p) = GET_LCHILD(tree);
        }
        switch (left_or_right) {
            case LEFT:
                GET_LCHILD(pre_root) = p;
                break;
            case RIGHT:
                GET_RCHILD(pre_root) = p;
            default:;
        }
    }
    pre_root=root;
}

/*
 * extend_heap - Extend heap with free block and return its block pointer
 */
static void *extend_heap(size_t words) {
    long *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((bp = mem_sbrk(size)) == NULL)
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    bp += WSIZE;
    PUT(HDRP(bp), PACK(size - WSIZE, 0));         /* free block header */
    PUT(FTRP(bp), PACK(size - WSIZE, 0));         /* free block footer */
    PUT(bp, 0);
    PUT(bp + CHILDSIZE, 0);
    if (root == NULL)
        root = bp;
    /* Coalesce if the previous block was free */
    return bp;
}

void delete_node(long *tree, long *place) {
    static char flag = 0;
    if (root == place) {
        root = NULL;
        return;
    }
    if (GET_SIZE(HDRP(tree)) > GET_SIZE(HDRP(place))) {
        flag = 0;
        left_or_right = LEFT;
        pre_root = tree;
        return delete_node((long *) GET_LCHILD(tree), place);
    } else if (GET_SIZE(HDRP(tree)) < GET_SIZE(HDRP(place))) {
        flag = 0;
        left_or_right = RIGHT;
        pre_root = tree;
        return delete_node((long *) GET_RCHILD(tree), place);
    } else {
        if (tree == place)
            delete(place);
        else {
            flag = !flag;
            switch (flag) {
                case 0:
                    left_or_right = LEFT;
                    return delete_node((long *) GET_LCHILD(tree), place);
                case 1:
                    left_or_right = RIGHT;
                    return delete_node((long *) GET_RCHILD(tree), place);
                default:;
            }
        }
    }
    pre_root=root;
}
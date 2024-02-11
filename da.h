#ifndef DA_H_
#define DA_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum daFields {
    SIZE          = 0,
    CAPACITY      = 1,
    TYPE          = 2,
    DATA          = 3,
};
                                       
typedef void * dynarr;
dynarr _daCreate(size_t initCapacity, size_t type, size_t size);
dynarr _daPush(dynarr da, void * item);
void _daPop(dynarr da, void * element);
void daPopDiscard(dynarr da);
void daFree(dynarr da);

size_t daSize(dynarr da);
size_t daCap(dynarr da);
size_t daType(dynarr da);

size_t * daField(dynarr da, enum daFields field);
void * daGetRef(dynarr da, size_t index);

typedef void(* func)(void * item);
void daForeach(dynarr da, func f);

typedef char * heapstr;
heapstr daToCStr(dynarr da);
void daBzero(dynarr da);

#define dynarr(type) type *
#define daCreate(type, cap) _daCreate(cap, sizeof(type), 0)
#define daPush(da, item) da = _daPush(da, &item)
#define daPop(da, item) _daPop(da, &item)

#endif // DA_H_
           
#ifdef DA_IMPLEMENTATION 

dynarr _daCreate(size_t cap, size_t type, size_t size)
{
    size_t * da = (size_t *) malloc(type * cap + DATA * sizeof(size_t));
    da[SIZE] = size;
    da[CAPACITY] = cap;
    da[TYPE] = type;

    return (dynarr) (da + DATA);
}                        

void daFree(dynarr da)
{
    free((size_t *) da - DATA);
}

dynarr daCopy(dynarr da)
{
    dynarr temp = _daCreate(daCap(da), daType(da), daSize(da));
    memcpy(temp, da, daSize(da) * daType(da));
    return temp;
}

dynarr daResize(dynarr da)
{
    dynarr temp = _daCreate(daCap(da) * 2, daType(da), daSize(da));
    memcpy(temp, da, daSize(da) * daType(da));
    daFree(da);
    return temp;
}

dynarr _daPush(dynarr da, void * item)
{
    if (daSize(da) >= daCap(da)) da = daResize(da);
    memcpy((char *)da + daSize(da) * daType(da), item, daType(da));
    *daField(da, SIZE) += 1;
    return da;
}

void _daPop(dynarr da, void * elem)
{   
    *daField(da, SIZE) -= 1;
    memcpy(elem, (char *)da + daSize(da) * daType(da), daType(da));
}

void daPopDiscard(dynarr da)
{   
    *daField(da, SIZE) -= 1;
}

size_t * daField(dynarr da, enum daFields field)
{
    if (field < SIZE || field > TYPE)
    {
        fprintf(stderr, "*------------------------------*\n");
        fprintf(stderr, "ERROR: Wrong field: %d\n", field);
        fprintf(stderr, "AVAILABLE: SIZE, CAPACITY, TYPE\n");
        fprintf(stderr, "*------------------------------*\n");
        exit(1);
    }

    return ((size_t *) da - DATA + field);
}

size_t daSize(dynarr da)
{
    return ((size_t *) da - DATA)[SIZE];
}

size_t daCap(dynarr da)
{
    return ((size_t *) da - DATA)[CAPACITY];
}

size_t daType(dynarr da)
{
    return ((size_t *) da - DATA)[TYPE];
}

void daForeach(dynarr da, func f)
{
    for (size_t i = 0; i < daSize(da); ++i)
        f((char *) da + i * daType(da));
}

heapstr daToCStr(dynarr da)
{
    char * buffer = (char *) calloc(1, daSize(da) + 1);
    memcpy(buffer, da, daSize(da) * daType(da));
    return buffer;
}

void daBzero(dynarr da)
{
    memset(da, '\0', daCap(da));
    *daField(da, SIZE) = 0;
}

void * daGetRef(dynarr da, size_t index)
{
    if (index >= daSize(da))
    {
        fprintf(stderr, "*------------------------------*\n");
        fprintf(stderr, "ERROR: Access out of bounds\n");
        fprintf(stderr, "*------------------------------*\n");
        exit(1);
    }

    return (void *)((char *)da + index * daType(da));
}

#endif // DA_IMPLEMENTATION

//
// Created by Yaokai Liu on 2023/2/18.
//

#ifndef X_ALLOC_H
#define X_ALLOC_H

#include "xtypes.h"

#ifndef XRE_BASIC_ALLOCATE_LENGTH
#define XRE_BASIC_ALLOCATE_LENGTH   32
#endif

#if (XRE_BASIC_ALLOCATE_LENGTH <= 0)
#error "Macro 'XRE_BASIC_ALLOCATE_LENGTH' must be a positive integer!"
#endif

typedef struct {
    xVoid * (*malloc)(xSize);
    xVoid * (*calloc)(xSize, xSize);
    xVoid * (*realloc)(xVoid *, xSize);
    xVoid (*free)(xVoid *);
    xVoid (*memcpy)(xVoid *target,const xVoid *src, xSize size);
    xVoid (*memset)(xVoid *addr, xSize size, xuByte value);
} Allocator;

#endif //X_ALLOC_H

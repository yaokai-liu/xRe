//
// Created by Yaokai Liu on 2023/2/18.
//

#ifndef X_ALLOC_H
#define X_ALLOC_H

#include "xtypes.h"

#ifndef XRE_BASIC_ALLOCATE_LENGTH
#define XRE_BASIC_ALLOCATE_LENGTH   32
#endif

typedef struct {
    xVoid * (*malloc)(xSize);
    xVoid * (*calloc)(xSize, xSize);
    xVoid * (*realloc)(xVoid *, xSize);
    xVoid (*free)(xVoid *);
    xVoid (*memcpy)(xVoid * src, xVoid * target, xSize size);
} Allocator;

#endif //X_ALLOC_H

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
    xVoid * (*malloc)(xuLong);
    xVoid * (*calloc)(xuLong, xSize);
    xVoid * (*realloc)(xVoid *, xuLong);
    xVoid (*free)(xVoid *);
} Allocator;

#endif //X_ALLOC_H

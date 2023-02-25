//
// Created by Yaokai Liu on 2023/2/18.
//

#ifndef X_ALLOCATOR_H
#define X_ALLOCATOR_H

#include "xtypes.h"

typedef struct {
    xVoid * (*malloc)(xuLong);
    xVoid * (*calloc)(xuLong, xSize);
    xVoid * (*realloc)(xVoid *, xuLong);
    xVoid (*free)(xVoid *);
} Allocator;

#endif //X_ALLOCATOR_H

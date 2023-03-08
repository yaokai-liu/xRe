//
// Created by Yaokai Liu on 2023/2/18.
//

#ifndef X_PARSE_H
#define X_PARSE_H

#include "alloc.h"
#include "structs.h"


#ifndef XRE_PARSE_ALLOCATE_SIZE
#define XRE_PARSE_ALLOCATE_SIZE   (32 * sizeof(xVoid *))
#endif

typedef struct XReParser XReParser;

void xReParser(XReParser * parser, Allocator * allocator);

#endif //X_PARSE_H

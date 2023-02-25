//
// Created by Yaokai Liu on 2023/2/18.
//

#ifndef X_PARSE_H
#define X_PARSE_H

#include "allocator.h"
#include "structs.h"


#ifndef XRE_PARSE_ALLOCATE_SIZE
#define XRE_PARSE_ALLOCATE_SIZE   (32 * sizeof(xVoid *))
#endif

Group * parse(xReChar * regexp, xuLong * offs, Allocator * allocator);

#endif //X_PARSE_H

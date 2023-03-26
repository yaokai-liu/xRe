//
// Created by Yaokai Liu on 2023/2/18.
//

#ifndef X_REGEXP_H
#define X_REGEXP_H

#include "alloc.h"
#include "structs.h"


typedef struct XReProcessor XReProcessor;

typedef xInt (matcher)(const xReChar * source, xInt * length);
typedef struct XReProcessor {
    struct {
        xuLong position;
        xReChar message[256];
    } errorLog;
    enum {
        regular = 0,
        arithmetic = 1,
    } state;
    Allocator * allocator;
    Group * (* parse)(XReProcessor * processor, xReChar * regexp);
    xInt (* match_o)(ReObj * obj, const xReChar * source, xInt * length);
    xInt (* match_c)(const xReChar * pattern, const xReChar * source, xInt * length);
    matcher * (* compile_o)(ReObj * obj);
    matcher * (* compile_c)(const xReChar * pattern);
} XReProcessor;

XReProcessor * xReProcessor(Allocator * allocator);

#endif //X_REGEXP_H

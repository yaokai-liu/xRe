//
// Created by Yaokai Liu on 2023/3/26.
//

#include "xregexp.h"

#define MEM_SIZE 4096

xuByte MEMORY[MEM_SIZE] = {};
struct {
    xVoid * addr;
    xSize   size;
} ENTITY_TABLE[MEM_SIZE];

xSize CURRENT_PTR = 0;
xuLong ENTITY_COUNT = 0;

xVoid * malloc(xuLong);
xVoid * calloc(xuLong, xSize);
xVoid * realloc(xVoid *, xuLong);
xVoid free(xVoid *);
xVoid memcpy(xVoid *src, xVoid *target, xuLong size);

int main() {
    Allocator allocator = {
            .malloc = (xVoid *) malloc,
            .calloc = (xVoid *) calloc,
            .realloc = (xVoid *) realloc,
            .free = (xVoid *) free
            };
    XReProcessor * processor = xReProcessor(&allocator);
    Group * group = processor->parse(processor, xReString("aaa"));
    allocator.free(group);
    return 0;
}

xVoid * malloc(xuLong size) {
    if (size == 0) {
        return nullptrof(xVoid);
    }
    if (CURRENT_PTR + size > MEM_SIZE) {
        return nullptrof(xVoid);
    }
    xVoid * ptr = MEMORY + CURRENT_PTR;
    CURRENT_PTR += size;
    ENTITY_TABLE[ENTITY_COUNT].addr = ptr;
    ENTITY_TABLE[ENTITY_COUNT].size = size;
    ENTITY_COUNT ++;
    return ptr;
}
xVoid * calloc(xuLong count, xSize size) {
    return malloc(count * size);
}
xVoid * realloc(xVoid * src, xuLong size) {
    xVoid * ptr =  malloc(size);
    if (ptr) {
        memcpy(src, ptr, size);
        free(src);
    }

}

xVoid memcpy(xVoid *src, xVoid *target, xuLong size) {
    for (int i = 0; i < size; i ++) {
        ((xuByte *)target)[i] = ((xuByte *)src)[i];
    }
}

xVoid free(xVoid * src) {
}

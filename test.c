//
// Created by Yaokai Liu on 2023/3/26.
//

#include "xregexp.h"
#include "stdio.h"

#define MEM_SIZE 4096

xuByte MEMORY[MEM_SIZE] = {};
struct {
    xVoid * addr;
    xSize   size;
} ENTITY_TABLE[MEM_SIZE];

xSize CURRENT_PTR = 0;
xuLong ENTITY_COUNT = 0;

xVoid * t_malloc(xSize);
xVoid * t_calloc(xSize, xSize);
xVoid * t_realloc(xVoid *, xSize);
xVoid t_free(xVoid *);
xVoid t_memcpy(xVoid *target, const xVoid *src, xSize size);

xInt main() {
    Allocator allocator = {
            .malloc = t_malloc,
            .calloc = t_calloc,
            .realloc = t_realloc,
            .free = t_free,
            .memcpy = t_memcpy,
            };
    XReProcessor * processor = xReProcessor(&allocator);
    xReChar * test_str = xReString("23456789{,2, 3}");
    Group * group = processor->parse(processor, test_str);
    if (group) {
        printf("%d\n", group->branches[0].cur_len);
        releaseObj(group, &allocator);
    }
    allocator.free(processor);
    return 0;
}

xVoid * t_malloc(xSize size) {
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
    printf("Allocate: %lld, %llu. MEM_USED = %llu\n", (xuByte *)ptr - MEMORY, size, CURRENT_PTR);
    return ptr;
}
xVoid * t_calloc(xSize count, xSize size) {
    xuByte* p = t_malloc(count * size);
    for (int i = 0; i < size; i++) p[i] = 0;
    return p;
}
xVoid * t_realloc(xVoid * src, xSize size) {
    xVoid * ptr =  t_malloc(size);
    if (ptr) {
        t_memcpy(ptr, src, size);
        t_free(src);
    }
    return ptr;
}

xVoid t_memcpy(xVoid *target, const xVoid *src, xSize size) {
    for (xInt i = 0; i < size; i ++) {
        ((xuByte *)target)[i] = ((xuByte *)src)[i];
    }
}

xVoid t_free(xVoid * src) {
    if ((xuByte *)src < MEMORY) {
        xVoid (*p)(xVoid *) = 0;
        p(src);
    }
    for(xInt i = 0; i < ENTITY_COUNT; i ++) {
        if (ENTITY_TABLE[i].addr == src) {
            printf("Release: %lld, %llu\n", (xuByte *)src - MEMORY, ENTITY_TABLE[i].size);
            ENTITY_TABLE[i].addr = nullptrof(xVoid);
            ENTITY_TABLE[i].size = 0;
        }
    }
}

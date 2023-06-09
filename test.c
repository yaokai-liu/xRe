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
    xReChar * test_str = xReString("(abc=<sss>)@<sss>$<sss>@<.sss>");
    Group * group = processor->parse(processor, test_str);
    if (!group) {
        printf("\nParsing failed at:\n");
        printf("\t%s\n\t", test_str);
        for (int i = 0; i < processor->errorLog.position; i++) printf("~");
        printf("^\n");
        return -1;
    }
    printf("\nBranch Count = %d\n\n", group->n_branches);
    releaseObj((ReObj *) group, &allocator);
    allocator.free(processor->global_group_array.array);
    clearLabelArray(processor->global_label_array, &allocator);
    allocator.free(processor);
    xInt unreleased = 0;
    for (int i = 0; i < ENTITY_COUNT; i++) {
        if (ENTITY_TABLE[i].addr) {
            printf("Unreleased Addr: %zu, Size: %lu, Index = %d\n",
                   ENTITY_TABLE[i].addr - (xVoid *) MEMORY, ENTITY_TABLE[i].size, i);
            unreleased ++ ;
        }
    }
    printf("\nUnreleased Entity Count: %d\n", unreleased);
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
    printf("Allocate: %ld, %lu. MEM_USED = %lu, Index = %lu\n",
           (xuByte *)ptr - MEMORY, size, CURRENT_PTR, ENTITY_COUNT);
    ENTITY_COUNT ++;
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

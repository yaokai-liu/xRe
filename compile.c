//
// Created by Yaokai Liu on 2023/3/25.
//

#include "compile.h"
#include "structs.h"
#include "rvm.h"

#define ALIGN_TARGET(_size, _type)     (_size / sizeof(_type))

#define SET_FLAG(_obj) \
    cur_inst.operate = set_flag; \
    cur_inst.pattern[0] = (_obj)->is_inverse; \
    cur_inst.pattern[1] = (_obj)->only_match; \
    if (arrayAppend(&array, &cur_inst, allocator) < 0) { \
        allocator->free(array.array); \
        return -1; \
    } \

#define RESET_FLAG \
    cur_inst.operate = set_flag; \
    cur_inst.pattern[0] = 0; \
    cur_inst.pattern[1] = 0; \
    if (arrayAppend(&array, &cur_inst, allocator) < 0) { \
        allocator->free(array.array); \
        return -1; \
    } \

xInt compileSeq(Sequence * seq, Inst ** inst_seg, xuInt * seg_len, Allocator * allocator);
xInt compileSet(Set * set, Inst ** inst_seg, xuInt * seg_len, Allocator * allocator);
xInt compileGroup(Group * grp, Inst ** inst_seg, xuInt * seg_len, Array * data_array, Allocator * allocator);

xInt compileArray(Array * array, xuByte * src, xuInt length, xInt size, const opcode op_array[], Allocator * allocator) {
    xuInt offset = 0; xuInt remains = length; Inst cur_inst;
    for (xuByte t = 8, l = 0; t >= size; t >>= 1, l++) {
        xuInt lpp = t / size;
        xuInt spp = lpp * size;
        for (xuInt i = 0; i < remains / lpp; i++, offset += lpp) {
            cur_inst.operate = op_array[l];
            allocator->memcpy(cur_inst.pattern, src + offset, lpp);
            for (xuInt j = spp; j < t; j++) cur_inst.pattern[j] = 0;
            if (arrayAppend(array, &cur_inst, allocator) < 0) {
                allocator->free(array->array);
                return -1;
            }
        }
        remains -= offset;
    }
    return 0;
}


xInt compileSeq(Sequence * seq, Inst ** inst_seg, xuInt * seg_len, Allocator * allocator) {
    Array array; Inst cur_inst;
    arrayInit(&array, sizeof(Inst), allocator);

    SET_FLAG(seq)

    opcode op_array[] = {plain_8, plain_4, plain_2, plain_1};
    if (compileArray(&array, (xuByte *) seq->value, seq->len,
                     sizeof(seq->value[0]), op_array, allocator) < 0) {
        return -1;
    }

    RESET_FLAG

    arrayRetreat(&array, allocator);
    *inst_seg = array.array;
    *seg_len = array.cur_len;

    return 0;
}

xInt compileSet(Set * set, Inst ** inst_seg, xuInt * seg_len, Allocator * allocator) {
    Array array; Inst cur_inst;
    arrayInit(&array, sizeof(Inst), allocator);

    SET_FLAG(set)

    opcode ver_op_array[] = {ver_8, ver_4, ver_2, ver_1};
    if (compileArray(&array, (xuByte *) set->plains, set->n_plains,
                     sizeof(set->plains[0]), ver_op_array, allocator) < 0) {
        return -1;
    }

    opcode range_op_array[] = {range_4, range_2, range_1};
    if (compileArray(&array, (xuByte *) set->ranges, set->n_ranges,
                     sizeof(set->ranges[0]), range_op_array, allocator) < 0) {
        return -1;
    }

    RESET_FLAG

    arrayRetreat(&array, allocator);
    *inst_seg = array.array;
    *seg_len = array.cur_len;

    return 0;
}

xInt compileObj(ReObj * obj, Array * inst_array, Allocator * allocator) {

}

xInt compileBranch(Array * obj_array, Array * inst_array, Allocator * allocator) {
    Array obj_inst_array;
    arrayInit(&obj_inst_array, sizeof(Inst), allocator);
    for (xuInt i = 0; i < obj_array->cur_len; i ++) {
        compileObj(((ReObj **) obj_array->array)[i], &obj_inst_array, allocator);
        arrayConcat(inst_array, obj_inst_array.array, obj_inst_array.cur_len, allocator);
    }
    allocator->free(obj_inst_array.array);
    return 0;
}

xInt compileGroup(Group * grp, Inst ** inst_seg, xuInt * seg_len, Array * data_array, Allocator * allocator) {
    Array inst_array; Inst cur_inst;
    arrayInit(&inst_array, sizeof(Inst), allocator);

    Variable var = {};
    for (xInt i = 0; i < grp->n_labels; i++) {
        arrayAppend(data_array, &var, allocator);
        ((Label *)grp->labels)[i].offset = data_array->cur_len - 1;
    }

    for (xInt i = 0; i < grp->n_groups; i++) {
        arrayAppend(data_array, &var, allocator);
        ((Group *)grp->groups)[i].offset = data_array->cur_len - 1;
    }

    cur_inst.operate = stage;
    allocator->memset(cur_inst.pattern, TARGET_BYTE_SIZE, 0);
    arrayAppend(&inst_array, &cur_inst, allocator);
    for (xInt i = 0; i < grp->n_branches; i ++) {
        Array branch_array;
        arrayInit(&branch_array, sizeof(Inst), allocator);

        compileBranch(&((Array *) grp->branches)[i], &branch_array, allocator);
        arrayConcat(&inst_array, branch_array.array, branch_array.cur_len, allocator);

        // instruct jump to 'reset' if not capture.
        cur_inst.operate = jnc;
        allocator->memcpy(cur_inst.pattern, "text", 4);
        struct { xuInt addr:TARGET_BYTE_SIZE - 4;} addr = {.addr = inst_array.cur_len - 1 + 4};
        allocator->memcpy(cur_inst.pattern + 4, (xuByte *) &(addr), TARGET_BYTE_SIZE - 4);
        arrayAppend(&inst_array, &cur_inst, allocator);

        cur_inst.operate = capture;
        allocator->memcpy(cur_inst.pattern, "data", 4);
        addr.addr = grp->offset;
        allocator->memcpy(cur_inst.pattern + 4, (xuByte *) &(addr), TARGET_BYTE_SIZE - 4);
        arrayAppend(&inst_array, &cur_inst, allocator);

        cur_inst.operate = ret;
        allocator->memset(cur_inst.pattern, TARGET_BYTE_SIZE, 0);
        arrayAppend(&inst_array, &cur_inst, allocator);

        cur_inst.operate = reset;
        allocator->memset(cur_inst.pattern, TARGET_BYTE_SIZE, 0);
        arrayAppend(&inst_array, &cur_inst, allocator);
    }
    arraySet(&inst_array, inst_array.cur_len - 1, &cur_inst, allocator);

    arrayRetreat(&inst_array, allocator);
    *inst_seg = inst_array.array,
    *seg_len = inst_array.cur_len;

    return 0;
}

xInt compileCount(Count * cnt, Inst ** inst_seg, xuInt * seg_len, Allocator * allocator) {
    Array inst_array; Inst cur_inst;
    arrayInit(&inst_array, sizeof(Inst), allocator);



}

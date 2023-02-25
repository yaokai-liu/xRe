//
// Created by Yaokai Liu on 2023/2/18.
//

#ifndef X_STRUCTS_H
#define X_STRUCTS_H

#include "char_t.h"

typedef enum {
    SEQ, SET, GRP,
    UNI, CNT, EXP
} obj_type;

#define OBJ_BASIC_ATTRIBUTE \
    obj_type id; \
    xBool only_parse; \
    xBool is_inverse; \
    xReChar *regexp; \

typedef struct {
    OBJ_BASIC_ATTRIBUTE
} ReObj;

typedef struct {
    OBJ_BASIC_ATTRIBUTE
    xSize len;
    xReChar *value;
} Sequence;

typedef struct Range {
    xReChar left;
    xReChar right;
} Range;

typedef struct {
    OBJ_BASIC_ATTRIBUTE
    xSize n_plains;
    xReChar *plain;
    xSize n_ranges;
    Range * ranges;
} Set;

typedef struct {
    OBJ_BASIC_ATTRIBUTE
    xSize n_subs;
    ReObj ** sub_objects;
} Group;

typedef struct {
    OBJ_BASIC_ATTRIBUTE
    Group *LHS;
    Group *RHS;
} Union;

typedef struct {
    OBJ_BASIC_ATTRIBUTE

} Expression;

typedef struct {
    OBJ_BASIC_ATTRIBUTE
    Expression * min;
    Expression * max;
    ReObj * obj;
} Count;


#include "allocator.h"

Sequence * createSeq(xSize len, xReChar * value, Allocator * allocator);
Set * createSet(xSize n_plains, xReChar * plain_buffer, xSize n_ranges, Range * range_buffer , Allocator * allocator);
Group * createGrp(xSize n_subs, ReObj ** sub_objects, Allocator * allocator);
Union * createUni(Group * lhs, Group * rhs, Allocator * allocator);
Count * createCnt(Expression * min, Expression * max, ReObj *obj, Allocator * allocator);

xVoid releaseObj(xVoid * obj, Allocator * allocator);

#undef OBJ_BASIC_ATTRIBUTE

#endif //X_STRUCTS_H

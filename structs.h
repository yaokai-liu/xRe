//
// Created by Yaokai Liu on 2023/2/18.
//

#ifndef X_STRUCTS_H
#define X_STRUCTS_H

#include "char_t.h"
#include "label.h"

typedef enum {
    SEQ, SET, GRP,
    CNT, EXP
} obj_type;

#define OBJ_BASIC_ATTRIBUTE \
    obj_type id; \
    xBool only_match; \
    xBool is_inverse; \
    xReChar *regexp; \

typedef struct {
    OBJ_BASIC_ATTRIBUTE
} ReObj;

typedef struct ObjArray {
    xSize n_objs;
    ReObj ** objects;
} ObjArray;

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


typedef struct Group Group;
typedef struct Group {
    OBJ_BASIC_ATTRIBUTE
    xuInt n_branches;
    ObjArray * branches;
    xuInt   n_groups;
    Group ** groups;
    xuInt   n_labels;
    Label * labels;
} Group;

typedef struct {
    OBJ_BASIC_ATTRIBUTE

} Expression;

typedef struct {
    OBJ_BASIC_ATTRIBUTE
    Expression * min;
    Expression * max;
    Expression * step;
    ReObj * obj;
} Count;


#include "alloc.h"

Sequence * createSeq(xSize len, xReChar * value, Allocator * allocator);
Set * createSet(xSize n_plains, xReChar * plain_buffer, xSize n_ranges, Range * range_buffer , Allocator * allocator);
Group *createGrp(xuInt n_branches, ObjArray branches[], xuInt n_groups, Group *groups[], xuInt n_labels, Label *labels,
                 Allocator *allocator);
Count * createCnt(Expression ** expression, ReObj *obj, Allocator * allocator);

xVoid releaseObj(xVoid * obj, Allocator * allocator);
xVoid clearObjArray(ObjArray * array, Allocator * allocator);
#undef OBJ_BASIC_ATTRIBUTE

typedef enum {
    ssi_escape = 0,
    ssi_space = 1,
    ssi_n = 2,
    ssi_r = 3,
    ssi_f = 4,
    ssi_v = 5,
    ssi_t = 6,
    ssi_whitespace = 7,
    ssi_non_whitespace = 8,
    ssi_word = 9,
    ssi_non_word = 10,
} special_set_index_t;

extern Set SPECIAL_SET_ARRAY[];


#endif //X_STRUCTS_H

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


typedef struct {
    OBJ_BASIC_ATTRIBUTE
} ReObj;

typedef struct ObjArray {
    xuInt n_objects;
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
    xuInt n_plains;
    xuInt n_ranges;
    xReChar *plains;
    Range * ranges;
} Set;

typedef struct {
    xReChar     key[8];
    xVoid *     value;
    xReChar *   last;
    xInt        last_len;
} Label;


typedef struct Group Group;
typedef struct Group {
    OBJ_BASIC_ATTRIBUTE
    xuInt n_branches;
    xuInt   n_groups;
    xuInt   n_labels;
    ObjArray * branches;
    Group ** groups;
    Label * labels;
    xReChar *   last;
    xInt        last_len;
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
Set * createSet(xuInt n_plains, xReChar * plain_buffer, xuInt n_ranges, Range * range_buffer , Allocator * allocator);
Group *createGrp(xuInt n_branches, ObjArray branches[], xuInt n_groups, Group *groups[], xuInt n_labels, Label *labels,
                 Allocator *allocator);
Count * createCnt(Expression ** expression, ReObj *obj, Allocator * allocator);

xVoid releaseObj(xVoid * obj, Allocator * allocator);
xVoid clearObjArray(ObjArray * array, Allocator * allocator);
#undef OBJ_BASIC_ATTRIBUTE

extern ReObj * SPECIAL_OBJ_ARRAY[];


#endif //X_STRUCTS_H

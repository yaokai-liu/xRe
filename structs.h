//
// Created by Yaokai Liu on 2023/2/18.
//

#ifndef X_STRUCTS_H
#define X_STRUCTS_H

#include "char_t.h"

typedef enum {
    NON = 0,
    SEQ, SET, GRP,
    CNT, EXP, LBL,
} obj_type;

#define OBJ_BASIC_ATTRIBUTE \
    obj_type id; \
    xBool only_match; \
    xBool is_inverse; \


typedef struct {
    OBJ_BASIC_ATTRIBUTE
} ReObj;

typedef struct {
    xuInt   cur_len;
    xuInt   buf_len;
    xSize   ele_size;
    xVoid * array;
} Array;

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

#ifndef LABEL_NAME_LEN
#define LABEL_NAME_LEN  8
#endif
typedef struct {
    xReChar     name[LABEL_NAME_LEN];
    xVoid *     object;
    xReChar *   last;
    xInt        last_len;
} Label;


typedef struct Group Group;
typedef struct Group {
    OBJ_BASIC_ATTRIBUTE
    xuInt n_branches;
    xuInt   n_groups;
    xuInt   n_labels;
    Array * branches;
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


#undef OBJ_BASIC_ATTRIBUTE

#include "alloc.h"

Sequence * createSeq(xSize len, xReChar * value, Allocator * allocator);
Set * createSet(xuInt n_plains, xReChar * plain_buffer, xuInt n_ranges, Range * range_buffer , Allocator * allocator);
Group *createGrp(xuInt n_branches, Array branches[], xuInt n_groups, Group *groups[], xuInt n_labels, Label *labels,
                 Allocator *allocator);
Count * createCnt(Expression ** expression, ReObj *obj, Allocator * allocator);
Label * createLabel(const xReChar * name, xuInt len, ReObj * obj, Allocator * allocator);

xVoid releaseObj(xVoid * obj, Allocator * allocator);
xVoid clearObjArray(Array _array, Allocator * allocator);

xVoid arrayInit(Array *_array, xSize _type_size, Allocator *allocator);
xInt arrayAppend(Array * _array, const xVoid *_element, Allocator * allocator);
xInt arrayConcat(Array * _array, xVoid * _element, xuInt count, Allocator * allocator);
xVoid arrayRetreat(Array * _array, Allocator * allocator);
xInt arrayFindByAttr(Array *_array, const xuByte *key, xuByte *(*get_attr)(xVoid *), xSize attr_len);
xVoid * arrayPop(Array * _array);

extern ReObj * SPECIAL_OBJ_ARRAY[];

#endif //X_STRUCTS_H

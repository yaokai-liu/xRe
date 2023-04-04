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
    CAL,
} obj_type;

#define OBJ_BASIC_ATTRIBUTE \
    obj_type id; \
    xBool unreleasable; \
    xBool only_match; \
    xBool is_inverse; \


typedef struct {
    OBJ_BASIC_ATTRIBUTE
} ReObj;

typedef struct {
    xBool releasable;
    ReObj * obj;
} ObjItem;

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
    obj_type    id;
    xuInt       offset;
    xReChar     name[LABEL_NAME_LEN];
    xVoid *     object;
} Label;


typedef struct Group Group;
typedef struct Group {
    OBJ_BASIC_ATTRIBUTE
    xBool at_begin: 1;
    xBool at_end: 1;
    xuInt  offset;
    xuInt n_branches;
    xuInt n_groups;
    xuInt n_labels;
    Array * branches;
    Group ** groups;
    Label * labels;
} Group;


enum exp_type{
    number,
    label
};
typedef struct {
    OBJ_BASIC_ATTRIBUTE
    enum exp_type exp_type;
    xVoid * value;
} Expression;


typedef struct {
    OBJ_BASIC_ATTRIBUTE
    Expression * min;
    Expression * max;
    Expression * step;
    ObjItem obj;
} Count;

typedef enum {
    callee,
    last_value
} call_t;
typedef struct {
    OBJ_BASIC_ATTRIBUTE
    call_t  call_type;
    Array * array;
    xuInt   index;
} Callee;

#undef OBJ_BASIC_ATTRIBUTE

#include "alloc.h"

Sequence * createSeq(xSize len, xReChar * value, Allocator * allocator);
Set * createSet(xuInt n_plains, xReChar * plain_buffer, xuInt n_ranges, Range * range_buffer , Allocator * allocator);
Group *createGrp(xBool at_begin, xBool at_end, xuInt n_branches, Array branches[], xuInt n_groups, Group *groups[],
                 xuInt n_labels, Label *labels, Allocator *allocator);
Count *createCnt(Expression *min, Expression *max, Expression *step, ObjItem *obj, Allocator *allocator);
Expression * createExp(enum exp_type type, xVoid * value, Allocator * allocator);
Callee *createCallee(Array *array, xuInt index, call_t call_type, Allocator *allocator);
xInt *initLabel(Label *label, const xReChar *name, xuInt len, ReObj *obj, Allocator *allocator);

xVoid releaseObj(ReObj *obj, Allocator * allocator);

xVoid clearObjArray(Array _array, Allocator * allocator);

xVoid clearLabelArray(Array _array, Allocator * allocator);

xVoid arrayInit(Array *_array, xSize _type_size, Allocator *allocator);
xInt arrayAppend(Array * _array, const xVoid *_element, Allocator * allocator);
xInt arraySet(Array * _array, xuInt index, const xVoid *_element, Allocator * allocator);
xInt arrayConcat(Array * _array, xVoid * _element, xuInt count, Allocator * allocator);
xVoid arrayRetreat(Array * _array, Allocator * allocator);
xInt arrayFindByAttr(Array *_array, const xuByte *key, xuByte *(*get_attr)(xVoid *), xSize attr_size);
xInt arrayPop(Array *_array, xVoid *_dest, Allocator *allocator);

extern ReObj * CONVERT_OBJ_ARRAY[];
extern ReObj * MACRO_OBJ_ARRAY[];
#endif //X_STRUCTS_H

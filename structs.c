//
// Created by Yaokai Liu on 2023/2/18.
//

#include "structs.h"

Sequence * createSeq(xSize len, xReChar * value, Allocator * allocator) {
    Sequence * seq = allocator->calloc(1, sizeof(Sequence));
    if (!seq) {
        allocator->free(value);
        return nullptrof(Sequence);
    }
    seq->id = SEQ;
    seq->is_inverse = false;
    seq->only_parse = false;
    seq->len = len;
    seq->value = value;
    return seq;
}

Set * createSet(xSize n_plains, xReChar * plain_buffer,
                xSize n_ranges, Range * range_buffer,
                Allocator * allocator) {
    Set *set = allocator->calloc(1, sizeof(Set));
    set->id = SET;
    set->n_plains = n_plains;
    set->plain = plain_buffer;
    set->n_ranges = n_ranges;
    set->ranges = range_buffer;
    return set;
}

Group * createGrp(xSize n_subs, ReObj ** sub_objects, Allocator * allocator) {
    Group * group = allocator->calloc(1, sizeof(Group));
    group->id = GRP;
    group->n_subs = n_subs;
    group->sub_objects = sub_objects;
    return group;
}

Union * createUni(Group * lhs, Group * rhs, Allocator * allocator) {
    Union * uni = allocator->calloc(1, sizeof(Union));
    uni->id = UNI;
    uni->LHS = lhs;
    uni->RHS = rhs;
    return uni;
}

xVoid releaseSeq(Sequence *seq, Allocator * allocator);
xVoid releaseSet(Set *set, Allocator * allocator);
xVoid releaseGrp(Group *grp, Allocator * allocator);
xVoid releaseUni(Union *uni, Allocator * allocator);
xVoid releaseCnt(Count *cnt, Allocator * allocator);
xVoid releaseExp(Expression *exp, Allocator * allocator);

xVoid (*RELEASE_ARRAY[6])(xVoid *, xVoid (*)(xVoid *)) = {
        [SEQ] = (xVoid (*)(xVoid *, xVoid (*)(xVoid *))) releaseSeq,
        [GRP] = (xVoid (*)(xVoid *, xVoid (*)(xVoid *))) releaseGrp,
        [SET] = (xVoid (*)(xVoid *, xVoid (*)(xVoid *))) releaseSet,
        [UNI] = (xVoid (*)(xVoid *, xVoid (*)(xVoid *))) releaseUni,
        [CNT] = (xVoid (*)(xVoid *, xVoid (*)(xVoid *))) releaseCnt,
        [EXP] = (xVoid (*)(xVoid *, xVoid (*)(xVoid *))) releaseExp
};

xVoid releaseObj(xVoid *obj, Allocator * allocator) {
    obj_type id = ((obj_type *) obj)[0];
    RELEASE_ARRAY[id](obj, allocator->free);
}

#define FREE_OBJ(_obj) \
    if ((_obj)->regexp) { \
        allocator->free((_obj)->regexp); \
    } \
    allocator->free(_obj); \

xVoid releaseSeq(Sequence *seq, Allocator * allocator) {
    allocator->free(seq->value);
}

void releaseGrp(Group *grp, Allocator * allocator) {
    for (xSize i = 0; i < grp->n_subs; i++)
        releaseObj(grp->sub_objects[i], allocator);
    allocator->free(grp->sub_objects);
    FREE_OBJ(grp)
}

void releaseSet(Set *set, Allocator * allocator) {
    allocator->free(set->ranges);
    allocator->free(set->plain);
    FREE_OBJ(set)
}

void releaseUni(Union *uni, Allocator * allocator) {
    releaseObj(uni->LHS, allocator);
    releaseGrp(uni->RHS, allocator);
    FREE_OBJ(uni)
}

void releaseCnt(Count *cnt, Allocator * allocator) {
    releaseObj(cnt->obj, allocator);
    FREE_OBJ(cnt)
}

void releaseExp(Expression *exp, Allocator * allocator) {

}

#undef FREE_OBJ
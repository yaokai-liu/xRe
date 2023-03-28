//
// Created by Yaokai Liu on 2023/2/18.
//

#include "structs.h"
#include "meta.h"

Sequence SPECIAL_SEQ_ARRAY[] = {
    { .id = SEQ, .value = xReString("\n"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("\r"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("\f"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("\v"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("\t"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString(" "), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("("), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString(")"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("["), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("]"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("{"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("}"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("<"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString(">"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("\\"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("|"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("@"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("#"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("$"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("~"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString(":"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("="), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("+"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("-"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("*"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("/"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("?"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("!"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("^"), .is_inverse = false, .only_match = false },
    { .id = SEQ, .value = xReString("."), .is_inverse = true, .only_match = false },
};

Set SPECIAL_SET_ARRAY[] = {
    { .id = SET, .n_plains = 6, .plains = WHITESPACE, .only_match = false, .is_inverse = false},
    { .id = SET, .n_plains = 6, .plains = WHITESPACE, .only_match = false, .is_inverse = true},
    { .id = SET, .n_plains = 1, .plains = xReString("_"), .only_match = false, .is_inverse = false,
            .n_ranges = 3, .ranges = (Range *) xReString("09azAZ")},
    { .id = SET, .n_plains = 1, .plains = xReString("_"), .only_match = false, .is_inverse = true,
            .n_ranges = 3, .ranges = (Range *) xReString("09azAZ")},
};

ReObj * SPECIAL_OBJ_ARRAY[] = {
    // sequence
    (ReObj *) &SPECIAL_SEQ_ARRAY[0],
    (ReObj *) &SPECIAL_SEQ_ARRAY[1],
    (ReObj *) &SPECIAL_SEQ_ARRAY[2],
    (ReObj *) &SPECIAL_SEQ_ARRAY[3],
    (ReObj *) &SPECIAL_SEQ_ARRAY[4],
    (ReObj *) &SPECIAL_SEQ_ARRAY[5],
    (ReObj *) &SPECIAL_SEQ_ARRAY[6],
    (ReObj *) &SPECIAL_SEQ_ARRAY[7],
    (ReObj *) &SPECIAL_SEQ_ARRAY[8],
    (ReObj *) &SPECIAL_SEQ_ARRAY[9],
    (ReObj *) &SPECIAL_SEQ_ARRAY[10],
    (ReObj *) &SPECIAL_SEQ_ARRAY[11],
    (ReObj *) &SPECIAL_SEQ_ARRAY[12],
    (ReObj *) &SPECIAL_SEQ_ARRAY[13],
    (ReObj *) &SPECIAL_SEQ_ARRAY[14],
    (ReObj *) &SPECIAL_SEQ_ARRAY[15],
    (ReObj *) &SPECIAL_SEQ_ARRAY[16],
    (ReObj *) &SPECIAL_SEQ_ARRAY[17],
    (ReObj *) &SPECIAL_SEQ_ARRAY[18],
    (ReObj *) &SPECIAL_SEQ_ARRAY[19],
    (ReObj *) &SPECIAL_SEQ_ARRAY[20],
    (ReObj *) &SPECIAL_SEQ_ARRAY[21],
    (ReObj *) &SPECIAL_SEQ_ARRAY[22],
    (ReObj *) &SPECIAL_SEQ_ARRAY[23],
    (ReObj *) &SPECIAL_SEQ_ARRAY[24],
    (ReObj *) &SPECIAL_SEQ_ARRAY[25],
    (ReObj *) &SPECIAL_SEQ_ARRAY[26],
    (ReObj *) &SPECIAL_SEQ_ARRAY[27],
    (ReObj *) &SPECIAL_SEQ_ARRAY[28],
    (ReObj *) &SPECIAL_SEQ_ARRAY[29],
    // set
    (ReObj *) &SPECIAL_SET_ARRAY[0],
    (ReObj *) &SPECIAL_SET_ARRAY[1],
    (ReObj *) &SPECIAL_SET_ARRAY[2],
    (ReObj *) &SPECIAL_SET_ARRAY[3],
};

int genSeqRegexp(Sequence *seq, xReChar *regexp, Allocator *allocator);
int genSetRegexp(Set *set, xReChar *regexp, Allocator *allocator);

Sequence * createSeq(xSize len, xReChar * value, Allocator * allocator) {
    Sequence * seq = allocator->calloc(1, sizeof(Sequence));
    if (!seq) {
        allocator->free(value);
        return nullptrof(Sequence);
    }
    seq->id = SEQ;
    seq->len = len;
    seq->value = value;
    return seq;
}

Set * createSet(xuInt n_plains, xReChar * plain_buffer,
                xuInt n_ranges, Range * range_buffer,
                Allocator * allocator) {
    Set *set = allocator->calloc(1, sizeof(Set));
    set->id = SET;
    set->n_plains = n_plains;
    set->plains = plain_buffer;
    set->n_ranges = n_ranges;
    set->ranges = range_buffer;
    return set;
}

Group *createGrp(xuInt n_branches, ObjArray branches[], xuInt n_groups, Group *groups[], xuInt n_labels, Label *labels,
                 Allocator *allocator) {
    Group * group = allocator->calloc(1, sizeof(Group));
    group->id = GRP;
    group->n_branches = n_branches;
    group->branches = branches;
    group->n_groups = n_groups;
    group->groups = groups;
    group->n_labels = n_labels;
    group->labels = labels;
    return group;
}

Count * createCnt(Expression * expression[], ReObj * obj, Allocator * allocator) {
    Count * count = allocator->calloc(1, sizeof(Count));
    count->id = CNT;
    count->min = expression[0];
    count->max = expression[1];
    count->step = expression[2];
    count->obj = obj;
    return count;
}

xVoid releaseSeq(Sequence *seq, Allocator * allocator);
xVoid releaseSet(Set *set, Allocator * allocator);
xVoid releaseGrp(Group *grp, Allocator * allocator);
xVoid releaseCnt(Count *cnt, Allocator * allocator);
xVoid releaseExp(Expression *exp, Allocator * allocator);

xVoid releaseObj(xVoid *obj, Allocator * allocator) {
    static xVoid (*RELEASE_ARRAY[6])(xVoid *, Allocator *) = {
            [SEQ] = (xVoid (*)(xVoid *, Allocator *)) releaseSeq,
            [GRP] = (xVoid (*)(xVoid *, Allocator *)) releaseGrp,
            [SET] = (xVoid (*)(xVoid *, Allocator *)) releaseSet,
            [CNT] = (xVoid (*)(xVoid *, Allocator *)) releaseCnt,
            [EXP] = (xVoid (*)(xVoid *, Allocator *)) releaseExp
    };
    obj_type id = ((obj_type *) obj)[0];
    RELEASE_ARRAY[id](obj, allocator);
}

#define FREE_OBJ(_obj) \
    allocator->free(_obj); \

xVoid releaseSeq(Sequence *seq, Allocator * allocator) {
    allocator->free(seq->value);
}

void releaseGrp(Group *grp, Allocator * allocator) {
    for (xSize i = 0; i < grp->n_branches; i++)
        clearObjArray(&grp->branches[i], allocator);
    allocator->free(grp->branches);
    allocator->free(grp->groups);
    allocator->free(grp->labels);
    FREE_OBJ(grp)
}

void releaseSet(Set *set, Allocator * allocator) {
    allocator->free(set->ranges);
    allocator->free(set->plains);
    FREE_OBJ(set)
}


void releaseCnt(Count *cnt, Allocator * allocator) {
    releaseObj(cnt->obj, allocator);
    FREE_OBJ(cnt)
}

void releaseExp(Expression *exp, Allocator * allocator) {

}

xVoid clearObjArray(ObjArray * array, Allocator * allocator) {
#define IS_STATIC__(_obj, _cat) \
     (  ((xVoid *) _obj) - (xVoid *) SPECIAL_##_cat##_ARRAY < sizeof(SPECIAL_##_cat##_ARRAY) \
     && ((xVoid *) _obj) >=(xVoid *) SPECIAL_##_cat##_ARRAY \
     && _obj->id == _cat)
#define IS_STATIC(_obj) \
    (  IS_STATIC__(array->objects[i], SET) \
    || IS_STATIC__(array->objects[i], SEQ))

    for (typeof(array->n_objects) i = 0; i < array->n_objects; i ++) {
        if (!IS_STATIC(array->objects[i])) {
            releaseObj(array->objects[i], allocator);
        }
    }
    allocator->free(array->objects);
    array->n_objects = 0;
#undef IS_STATIC__
#undef IS_STATIC
}
#undef FREE_OBJ

#define SET_ATTR(_obj) \
(_obj)->only_match ? (regexp[i] = xReChar('!'), i++) : 0; \
(_obj)->is_inverse ? (regexp[i] = xReChar('^'), i++) : 0; \

int genSeqRegexp(Sequence *seq, xReChar *regexp, Allocator *allocator) {
    if (!regexp) return -1;
    int i = 0;
    SET_ATTR(seq)
    allocator->memcpy(seq->value, regexp + i, seq->len);
    return 0;
}

int genSetRegexp(Set *set, xReChar *regexp, Allocator *allocator) {
    if (!regexp) return -1;
    int i = 0;
    (regexp[i] = beginS), i ++;
    SET_ATTR(set)
    allocator->memcpy(set->plains, regexp + i, set->n_plains);
    for (int j = 0; j < set->n_ranges; j ++) {
        (regexp[i] = set->ranges[j].left), i++;
        (regexp[i + 1] = rangeTO), i++;
        (regexp[i + 2] = set->ranges[j].right), i++;
    }
    regexp[i] = endS;
    return 0;
}

#undef SET_ATTR
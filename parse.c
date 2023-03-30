//
// Created by Yaokai Liu on 2023/3/25.
//

#include "parse.h"
#include "meta.h"
#include "string.h"

static Group *parse(xReChar *regexp, xReChar _begin_char, xReChar _end_char, xuLong *offs, Allocator *allocator);
static ReObj * parseCrt(xReChar *regexp, xuLong *offs, Allocator *allocator);
static Sequence * parseSeq(xReChar * regexp, xuLong * offs, Allocator * allocator);
static Set * parseSet(xReChar * regexp, xuLong * offs, Allocator * allocator);
static Label * parseAssignLabel(xReChar *regexp, ReObj *obj, xuLong *offs, Allocator *allocator);
static ReObj * parseCallLabel(xReChar *regexp, xuLong *offs, Array *label_array, Allocator *allocator);
static Expression ** parseCntExp(xReChar * regexp, xuLong * offs, Allocator * allocator);
static Expression * parseExp(xReChar * regexp, xuLong * offs, Allocator * allocator);

Group * xReProcessor_parse(XReProcessor *processor, xReChar *pattern) {
    return parse(pattern, 0, xReChar('\0'), &(processor->errorLog.position), processor->allocator);
}

ReObj * parseCrt(xReChar *regexp, xuLong *offs, Allocator *allocator) {
    *offs = 0;
    xReChar *sp = regexp;

    if (sp[*offs] != xReChar('\\')) {
        return nullptrof(ReObj);
    }
    (*offs)++;

    xInt i = 0;
    if ((i = hasChar(SINGLE_ESCAPE, sp[*offs])) >= 0) {
        (*offs) ++;
        return SPECIAL_OBJ_ARRAY[i];
    }

    switch (sp[*offs]) {
        case xReChar('u'): {
            // todo: unicode
            return nullptrof(ReObj);
        }
        case xReChar('x'): {
            // todo: unsigned hexadecimal integer
            return nullptrof(ReObj);
        }
        case xReChar('c'): {
            // todo: control character
            return nullptrof(ReObj);
        }
        default:
            return nullptrof(ReObj);
    }
}

Group *parse(xReChar *regexp, xReChar _begin_char, xReChar _end_char, xuLong *offs, Allocator *allocator) {
    *offs = 0;
    xReChar * sp = regexp;

    if (_begin_char) {
        if (sp[*offs] != _begin_char)
            return nullptrof(Group);
        else
            (*offs) ++;
    }


    Array branch_array = {};
    arrayInit(&branch_array, sizeof(Array), allocator);

    Array obj_array = {};
    arrayInit(&obj_array, sizeof(ReObj *), allocator);

    Array group_array = {};
    arrayInit(&group_array, sizeof(Group *), allocator);

    Array label_array = {};
    arrayInit(&label_array, sizeof(Label), allocator);

    xBool _only_match = false; xBool _is_inverse = false;
    while (sp[*offs] && sp[*offs] != _end_char) {
        xVoid * obj = nullptrof(xVoid);
        xuLong offset = 0;
        switch (sp[*offs]) {
            case only_match:{
                (*offs) ++;
                _only_match = true;
                continue;
            }
            case is_inverse:{
                (*offs) ++;
                _is_inverse = true;
                continue;
            }
            case escape: {
                obj = parseCrt(sp + *offs, &offset, allocator);
                break;
            }
            case beginS: {
                obj = parseSet(sp + *offs, &offset, allocator);
                break;
            }
            case beginG: {
                obj = parse(sp + *offs, beginG, endG, &offset, allocator);
                if (arrayAppend(&group_array, &obj, allocator) < 0) {
                    releaseObj(obj, allocator);
                    goto __failed_parse_group;
                }
                break;
            }
            case beginC: {
                Count * cnt = nullptrof(Count);
                break;
            }
            case unionOR: {
                arrayRetreat(&obj_array, allocator);
                arrayAppend(&branch_array, &obj_array, allocator);
                arrayInit(&obj_array, sizeof(ReObj *), allocator);
                obj = nullptrof(xVoid);
                (*offs) ++;
                goto __label_000001_clean_attributes;
            }
            case assign: {
                obj = parseAssignLabel(sp + *offs, ((ReObj **)obj_array.array)[obj_array.cur_len - 1], &offset, allocator);
                (*offs) += offset;
                if (!obj) goto __failed_parse_group;
                if (arrayAppend(&label_array, obj, allocator) < 0) {
                    releaseObj(obj, allocator);
                    goto __failed_parse_group;
                }
                continue;
            }
            case call: {
                obj = parseCallLabel(sp + *offs, &offset, &label_array, allocator);
                *offs += offset;
                if (!obj) {
                    goto __failed_parse_group;
                }
                goto __label_000002_append_obj;
            }
            case copy:
            case lastValue:
            case attribute: {
                // TODO: match condition.
            }
            default:
                if (hasChar(WHITESPACE, sp[*offs]) < 0) {
                    obj = parseSeq(sp + *offs, &offset, allocator);
                } else {
                    while (hasChar(WHITESPACE, sp[*offs]) >= 0) (*offs) ++;
                    goto __label_000001_clean_attributes;
                }
        }
        *offs += offset;

        if (!obj) {
            goto __failed_parse_group;
        }
        ((ReObj *)obj)->only_match = _only_match;
        ((ReObj *)obj)->is_inverse = _is_inverse;

        __label_000002_append_obj:
        if (arrayAppend(&obj_array, &obj, allocator) < 0) {
            goto __failed_parse_group;
        }

        __label_000001_clean_attributes:
        _only_match = false;
        _is_inverse = false;
    }
    arrayRetreat(&obj_array, allocator);
    arrayAppend(&branch_array, &obj_array, allocator);

    if (_end_char) {
        if (sp[*offs] != _end_char)
            goto __failed_parse_group;
        else
            (*offs) ++;
    }

    arrayRetreat(&branch_array, allocator);
    arrayRetreat(&group_array, allocator);
    arrayRetreat(&label_array, allocator);

    return createGrp(branch_array.cur_len, branch_array.array,
                     group_array.cur_len, group_array.array,
                     label_array.cur_len, label_array.array, allocator);

    __failed_parse_group:
    for (xuInt i = 0; i < branch_array.cur_len; i++) {
        Array array = ((Array *) branch_array.array)[i];
        clearObjArray(array, allocator);
    }
    allocator->free(branch_array.array);
    allocator->free(group_array.array);
    allocator->free(label_array.array);
    return nullptrof(Group);
}

Sequence * parseSeq(xReChar * regexp, xuLong * offs, Allocator * allocator) {
    *offs = 0;
    xReChar * sp = regexp;

    Array plain_array = {};
    arrayInit(&plain_array, sizeof(xReChar), allocator);

    while (sp[*offs] && hasChar(NON_PLAIN, sp[*offs]) < 0) {
        (*offs) ++;
    }
    if (*offs == 0) {
        goto __failed_parse_sequence;
    }
    arrayConcat(&plain_array, sp, *offs, allocator);
    arrayRetreat(&plain_array, allocator);
    return createSeq(plain_array.cur_len, plain_array.array, allocator);

    __failed_parse_sequence:
    allocator->free(plain_array.array);
    return nullptrof(Sequence);
}

Set * parseSet(xReChar * regexp, xuLong * offs, Allocator * allocator) {
    *offs = 0;
    xReChar * sp = regexp;
    if (!sp[0] || sp[0] != beginS) {
        return nullptrof(Set);
    }
    (*offs)++;

    Array plain_array = {}, range_array = {};
    arrayInit(&plain_array, sizeof(xReChar), allocator);
    arrayInit(&range_array, sizeof(Range), allocator);

    while (sp[*offs] && sp[*offs] != endS) {
        switch (sp[*offs]) {
            case escape:{
                xuLong offset = 0;
                ReObj * obj = parseCrt(sp + *offs, &offset, allocator);
                if (obj->id == SEQ) {
                    arrayConcat(&plain_array, ((Sequence *) obj)->value, ((Sequence *) obj)->len, allocator);
                } else if (obj->id == SET) {
                    arrayConcat(&plain_array, ((Set *) obj)->plains, ((Set *) obj)->n_plains, allocator);
                    arrayConcat(&range_array, ((Set *) obj)->ranges, ((Set *) obj)->n_ranges, allocator);
                } else {
                    goto __failed_parse_set;
                }
                *offs += offset;
                break;
            }
            case rangeTO: {
                (*offs) ++;
                if (!sp[(*offs)] || (sp[(*offs)] != escape && hasChar(METAS, sp[(*offs)]) >= 0)) {
                    goto __failed_parse_set;
                }
                Range r = {sp[(*offs) - 2], sp[(*offs)]};
                arrayAppend(&range_array, &r, allocator);
                arrayPop(&plain_array);
                (*offs) ++;
                break;
            }
            case dot: {
                break;
            }
            default: {
                arrayAppend(&plain_array, sp + *offs, allocator);
                (*offs) ++;
            }
        }
    }
    if (sp[*offs] != endS) {
        goto __failed_parse_set;
    }
    (*offs) ++;

    if (!plain_array.cur_len && range_array.cur_len) {
        goto __failed_parse_set;
    }

    arrayRetreat(&plain_array, allocator);
    arrayRetreat(&range_array, allocator);

    return createSet(plain_array.cur_len, plain_array.array,
                     range_array.cur_len, range_array.array, allocator);

    __failed_parse_set:
    allocator->free(plain_array.array);
    allocator->free(range_array.array);
    return nullptrof(Set);
}

Expression ** parseCntExp(xReChar * regexp, xuLong * offs, Allocator * allocator) {
    *offs = 0;
    xReChar * sp = regexp;
    if (!sp[0] || sp[0] != beginS) {
        return nullptrof(Expression *);
    }
    (*offs)++;

}


Label * parseAssignLabel(xReChar *regexp, ReObj *obj, xuLong *offs, Allocator *allocator) {
    *offs = 0;
    xReChar  * sp = regexp;

    if (!sp[*offs] || sp[*offs] != assign) {
        return nullptrof(Label);
    }
    (*offs)++;
    while (hasChar(WHITESPACE, sp[*offs]) >= 0) {
        (*offs) ++;
    }
    if (!sp[*offs] || sp[*offs] != beginL) {
        return nullptrof(Label);
    }
    (*offs)++;
    while (hasChar(WHITESPACE, sp[*offs]) >= 0) {
        (*offs) ++;
    }
    xuInt p = *offs;
    while (xReChar('a') <= sp[*offs] && sp[*offs] <= xReChar('z') ) (*offs) ++;
    xuInt l = (*offs - p <= 8) ? *offs - p : 8;

    while (hasChar(WHITESPACE, sp[*offs]) >= 0) {
        (*offs) ++;
    }
    if (!sp[*offs] || sp[*offs] != endL) {
        return nullptrof(Label);
    }
    (*offs)++;

    if (!obj) {
        return nullptrof(Label);
    }

    Label * label = createLabel(sp + p, l, obj, allocator);
    return label;
}

xuByte *labelGetName(Label * label) {
    return (xuByte *) &(label->name);
}

ReObj *parseCallLabel(xReChar *regexp, xuLong *offs, Array *label_array, Allocator *allocator) {
    *offs = 0;
    xReChar  * sp = regexp;

    if (!sp[*offs] || sp[*offs] != call) {
        return nullptrof(ReObj);
    }
    (*offs)++;
    while (hasChar(WHITESPACE, sp[*offs]) >= 0) {
        (*offs) ++;
    }
    if (!sp[*offs] || sp[*offs] != beginL) {
        return nullptrof(ReObj);
    }
    (*offs)++;
    while (hasChar(WHITESPACE, sp[*offs]) >= 0) {
        (*offs) ++;
    }
    xuInt p = *offs;
    while (xReChar('a') <= sp[*offs] && sp[*offs] <= xReChar('z') ) (*offs) ++;
    xuInt l = (*offs - p <= 8) ? *offs - p : 8;

    while (hasChar(WHITESPACE, sp[*offs]) >= 0) {
        (*offs) ++;
    }
    if (!sp[*offs] || sp[*offs] != endL) {
        return nullptrof(ReObj);
    }
    (*offs)++;

    xInt i = arrayFindByAttr(label_array, (xuByte *) sp + p,
                             (xuByte *(*)(void *)) labelGetName, l);
    return i >=0 ? ((Label *) label_array->array)[i].object : nullptrof(ReObj);
}

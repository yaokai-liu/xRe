//
// Created by Yaokai Liu on 2023/3/25.
//

#include "parse.h"
#include "meta.h"
#include "string.h"

#define LAMBDA

static Group *
parse(xReChar *regexp, Array *g_group_array, Array *g_label_array, xReChar _begin_char, xReChar _end_char, xuLong *offs,
      Allocator *allocator);
static ReObj * parseCrt(xReChar *regexp, xuLong *offs, Allocator *allocator);
static Count * parseMacroCount(xReChar *regexp, ObjItem *obj, xuLong *offs, Allocator *allocator);
static Sequence * parseSeq(xReChar * regexp, xuLong * offs, Allocator * allocator);
static Set * parseSet(xReChar * regexp, xuLong * offs, Allocator * allocator);
static Callee * parseAssignLabel(xReChar *regexp, ReObj *obj, Array *context[], xuLong *offs, Allocator *allocator);
static Callee * parseCallLabel(xReChar *regexp, Array *context[], xuLong *offs, Allocator *allocator);
static Count *parseCntExp(xReChar *regexp, ObjItem *obj, Array *context[], xuLong *offs, Allocator *allocator);

LAMBDA xuByte *labelGetName(Label * label) {
    return (xuByte *) &(label->name);
}

Group * xReProcessor_parse(XReProcessor *processor, xReChar *pattern) {
    if (!processor->global_group_array.array)
        arrayInit(&processor->global_group_array, sizeof(Group *), processor->allocator);
    if (!processor->global_label_array.array)
        arrayInit(&processor->global_label_array, sizeof(Label), processor->allocator);
    return parse(pattern,
                 &processor->global_group_array, &processor->global_label_array,
                 0, xReChar('\0'),
                 &(processor->errorLog.position),
                 processor->allocator);
}

Group * parse(xReChar *regexp,
      Array *g_group_array, Array *g_label_array,
      xReChar _begin_char, xReChar _end_char,
      xuLong *offs, Allocator *allocator) {
    *offs = 0;
    xReChar * sp = regexp;

    if (_begin_char) {
        if (sp[*offs] != _begin_char)
            return nullptrof(Group);
        else
            (*offs) ++;
    }

    xuInt global_index = 0;
    {
        Group *g = nullptrof(Group);
        if (arrayAppend(g_group_array, &g, allocator) < 0)
            return nullptrof(Group);
        global_index = g_group_array->cur_len - 1;
    }

    xBool at_begin = false;
    if (sp[*offs] == atBegin) { at_begin = true; (*offs) ++; }

    Array branch_array = {};
    arrayInit(&branch_array, sizeof(Array), allocator);

    ObjItem ob_obj = {};
    Array obj_array = {};
    arrayInit(&obj_array, sizeof(ObjItem), allocator);

    Array group_array = {};
    arrayInit(&group_array, sizeof(Group *), allocator);

    Array label_array = {};
    arrayInit(&label_array, sizeof(Label), allocator);

    Array * context[4] = {&group_array, &label_array, g_group_array, g_label_array};

    xBool _only_match = false; xBool _is_inverse = false;
    while (sp[*offs] && sp[*offs] != _end_char && sp[*offs] != atEnd) {
        ob_obj.releasable = true;
        ob_obj.obj = nullptrof(ReObj);
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
            case dot: {
                ob_obj.obj = MACRO_OBJ_ARRAY[0];
                ob_obj.releasable = false;
                break;
            }
            case plus:
            case star:
            case quesMark:{
                xInt s = arrayPop(&obj_array, &ob_obj, allocator);
                if (s < 0) goto __failed_parse_group;
                ob_obj.obj = (ReObj *) parseMacroCount(sp + *offs, &ob_obj, &offset, allocator);
                break;
            }
            case escape: {
                ob_obj.obj = parseCrt(sp + *offs, &offset, allocator);
                ob_obj.releasable = false;
                break;
            }
            case beginS: {
                ob_obj.obj = (ReObj *) parseSet(sp + *offs, &offset, allocator);
                break;
            }
            case beginG: {
                ob_obj.obj = (ReObj *) parse(sp + *offs,
                                             g_group_array, g_label_array,
                                             beginG, endG,
                                             &offset, allocator);
                if (arrayAppend(&group_array, &ob_obj.obj, allocator) < 0) {
                    releaseObj(ob_obj.obj, allocator);
                    goto __failed_parse_group;
                }
                break;
            }
            case beginC: {
                xInt s = arrayPop(&obj_array, &ob_obj, allocator);
                if (s < 0) goto __failed_parse_group;
                ob_obj.obj = (ReObj *) parseCntExp(sp + *offs, &ob_obj, context, &offset, allocator);
                ob_obj.releasable = true;
                break;
            }
            case unionOR: {
                arrayRetreat(&obj_array, allocator);
                arrayAppend(&branch_array, &obj_array, allocator);
                arrayInit(&obj_array, sizeof(ObjItem), allocator);
                (*offs) ++;
                goto __label_000001_clean_attributes;
            }
            case assign: {
                xInt s = arrayPop(&obj_array, &ob_obj, allocator);
                if (s < 0) goto __failed_parse_group;
                ob_obj.obj = (ReObj *) parseAssignLabel(sp + *offs, ob_obj.obj, context, &offset, allocator);
                ob_obj.releasable = true;
                break;
            }
            case call:
            case lastValue: {
                ob_obj.obj = (ReObj *) parseCallLabel(sp + *offs, context, &offset, allocator);
                break;
            }
            default:
                if (stridx(WHITESPACE, sp[*offs]) < 0) {
                    ob_obj.obj = (ReObj *) parseSeq(sp + *offs, &offset, allocator);
                } else {
                    while (stridx(WHITESPACE, sp[*offs]) >= 0) (*offs) ++;
                    goto __label_000001_clean_attributes;
                }
        }
        *offs += offset;

        if (!ob_obj.obj) {
            goto __failed_parse_group;
        }
        ((ReObj *)ob_obj.obj)->only_match = _only_match;
        ((ReObj *)ob_obj.obj)->is_inverse = _is_inverse;

        if (arrayAppend(&obj_array, &ob_obj, allocator) < 0) {
            goto __failed_parse_group;
        }

        __label_000001_clean_attributes:
        _only_match = false;
        _is_inverse = false;
    }
    arrayRetreat(&obj_array, allocator);
    arrayAppend(&branch_array, &obj_array, allocator);

    xBool at_end = false;
    if (sp[*offs] == atEnd) { at_end = true; (*offs) ++; }

    if (_end_char) {
        if (sp[*offs] != _end_char)
            goto __failed_parse_group;
        else
            (*offs) ++;
    }

    arrayRetreat(&branch_array, allocator);
    arrayRetreat(&group_array, allocator);
    arrayRetreat(&label_array, allocator);

    Group * grp = createGrp(at_begin, at_end,
                     branch_array.cur_len, branch_array.array,
                     group_array.cur_len, group_array.array,
                     label_array.cur_len, label_array.array,
                     allocator);
    arraySet(g_group_array, global_index, &grp, allocator);
    return grp;

    __failed_parse_group:
    clearObjArray(obj_array, allocator);
    for (xuInt i = 0; i < branch_array.cur_len; i++) {
        Array array = ((Array *) branch_array.array)[i];
        clearObjArray(array, allocator);
    }
    allocator->free(branch_array.array);
    allocator->free(group_array.array);
    allocator->free(label_array.array);
    return nullptrof(Group);
}


ReObj * parseCrt(xReChar *regexp, xuLong *offs, Allocator *allocator) {
    *offs = 0;
    xReChar *sp = regexp;

    if (sp[*offs] != xReChar('\\')) {
        return nullptrof(ReObj);
    }
    (*offs)++;

    xInt i = 0;
    if ((i = stridx(SINGLE_ESCAPE, sp[*offs])) >= 0) {
        (*offs) ++;
        return CONVERT_OBJ_ARRAY[i];
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

Count * parseMacroCount(xReChar *regexp, ObjItem *obj, xuLong *offs, Allocator *allocator) {
    *offs = 0;
    xReChar *sp = regexp;

    Count * template;
    switch (sp[*offs]) {
        case plus: {
            template = (Count *) MACRO_OBJ_ARRAY[1];
            break;
        }
        case star: {
            template = (Count *) MACRO_OBJ_ARRAY[2];
            break;
        }
        case quesMark: {
            template = (Count *) MACRO_OBJ_ARRAY[3];
            break;
        }
        default:
            return nullptrof(Count);
    }
    (*offs) ++;

    return createCnt(template->min, template->max, template->step, obj, allocator);
}

Sequence * parseSeq(xReChar * regexp, xuLong * offs, Allocator * allocator) {
    *offs = 0;
    xReChar * sp = regexp;

    Array plain_array = {};
    arrayInit(&plain_array, sizeof(xReChar), allocator);

    while (sp[*offs] && stridx(NON_PLAIN, sp[*offs]) < 0) {
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
                if (!sp[(*offs)] || (sp[(*offs)] != escape && stridx(METAS, sp[(*offs)]) >= 0)) {
                    goto __failed_parse_set;
                }
                Range r = {sp[(*offs) - 2], sp[(*offs)]};
                arrayAppend(&range_array, &r, allocator);
                arrayPop(&plain_array, NULL, allocator);
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

    if (!plain_array.cur_len && !range_array.cur_len) {
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

static Expression *parseExp(xReChar *regexp, Array *context[], xuLong *offs, Allocator *allocator);

Count *parseCntExp(xReChar *regexp, ObjItem *obj, Array *context[], xuLong *offs, Allocator *allocator) {
    *offs = 0;
    xReChar * sp = regexp;

    if (!sp[0] || sp[0] != beginC) {
        return nullptrof(Count);
    }
    (*offs)++;

    xuLong offset = 0;
    Expression * min = nullptrof(Expression);
    Expression * max = nullptrof(Expression);
    Expression * step = nullptrof(Expression);
    xBool max_same_min = true;

    while (stridx(WHITESPACE, sp[*offs]) >= 0) (*offs) ++;
    min = parseExp(sp + *offs, context, &offset, allocator);
    *offs += offset;
    while (stridx(WHITESPACE, sp[*offs]) >= 0) (*offs) ++;
    if (sp[*offs] == comma) {
        max_same_min = false;
        (*offs) ++;
    }

    while (stridx(WHITESPACE, sp[*offs]) >= 0) (*offs) ++;
    max = parseExp(sp + *offs, context, &offset, allocator);
    *offs += offset;
    while (stridx(WHITESPACE, sp[*offs]) >= 0) (*offs) ++;
    if (sp[*offs] == comma) {
        (*offs) ++;
    }

    while (stridx(WHITESPACE, sp[*offs]) >= 0) (*offs) ++;
    step = parseExp(sp + *offs, context, &offset, allocator);
    *offs += offset;
    while (stridx(WHITESPACE, sp[*offs]) >= 0) (*offs) ++;
    if (sp[*offs] != endC) goto __failed_parse_count;
    (*offs) ++;

    if (!min) min = createExp(number, nullptrof(Array), 0, allocator);
    if (!max) max =  max_same_min ? createExp(min->exp_type, min->value.array, min->value.index, allocator)
                                  : createExp(number, nullptrof(Array), 0, allocator);
    if (!step) step = createExp(number, nullptrof(Array), 1, allocator);

    return createCnt(min, max, step, obj, allocator);

    __failed_parse_count:
    if (min) releaseObj((ReObj *) min, allocator);
    if (max && max != min) releaseObj((ReObj *) max, allocator);
    if (step) releaseObj((ReObj *) step, allocator);
    return nullptrof(Count);
}

xInt parseLabelName(xReChar *regexp, Array *context[], xuInt *idx, xInt *signal, xReChar **p, xuInt *len,
                    xReChar _begin_char, xReChar _end_char, xuLong *offs);

Expression *parseExp(xReChar *regexp, Array *context[], xuLong *offs, Allocator *allocator) {
    *offs = 0;
    xReChar * sp = regexp;

    xuInt index = 0; xuInt idx = 0;
    enum exp_type e_type = number;

    while (stridx(WHITESPACE, sp[*offs]) >= 0) {
        (*offs) ++;
    }
    *offs = str2ul_d(sp + *offs, (xuLong *) &index);

    if (!*offs) {
        e_type = label;
        xuLong offset = 0; xInt signal = 0; xReChar * p; xuInt len = 0;
        index = parseLabelName(sp + *offs, context, &idx, &signal,
                               &p, &len, 0, 0, &offset);
        *offs += offset;
        if (signal < 0 || index < 0) {
            return nullptrof(Expression);
        }
    }

    return createExp(e_type, context[idx], index, allocator);
}

Callee * parseAssignLabel(xReChar *regexp, ReObj *obj, Array *context[], xuLong *offs, Allocator *allocator) {
    *offs = 0;
    xReChar  * sp = regexp;

    if (!sp[*offs] || sp[*offs] != assign) {
        releaseObj(obj, allocator);
        return nullptrof(Callee);
    }
    (*offs)++;

    xuLong offset = 0; xInt index = 0, signal = 0; xuInt idx = 0; xReChar * p; xuInt len = 0;
    index = parseLabelName(sp + *offs, context, &idx, &signal, &p, &len, beginL, endL, &offset);
    *offs += offset;
    if (signal < 0 || index > 0 || idx % 2 == 0) {
        releaseObj(obj, allocator);
        return nullptrof(Callee);
    }

    if (!obj) {
        releaseObj(obj, allocator);
        return nullptrof(Callee);
    }

    Label label = {};
    initLabel(&label, p, len, obj, allocator);
    if (arrayAppend(context[idx], &label, allocator) < 0) {
        releaseObj(obj, allocator);
        return nullptrof(Callee);
    }
    return createCallee(context[idx], context[idx]->cur_len - 1, callee, allocator);
}

Callee * parseCallLabel(xReChar *regexp, Array *context[], xuLong *offs, Allocator *allocator) {
    *offs = 0;
    xReChar  * sp = regexp;

    call_t call_type = 0;
    switch (sp[*offs]) {
        case call:
            call_type = callee;
            break;
        case lastValue:
            call_type = last_value;
            break;
        default:
            return nullptrof(Callee);
    }
    (*offs) ++;

    xuLong offset = 0; xInt index = 0, signal = 0; xuInt idx = 0; xReChar * p; xuInt len = 0;
    index = parseLabelName(sp + *offs, context, &idx, &signal, &p, &len, beginL, endL, &offset);
    if (signal < 0) {
        return nullptrof(Callee);
    }
    if (index < 0) {
        (*offs) ++;
        return nullptrof(Callee);
    }
    *offs += offset;

    return createCallee(context[idx], index, call_type, allocator);
}

xInt parseLabelName(xReChar *regexp, Array *context[], xuInt *idx, xInt *signal, xReChar **p, xuInt *len,
                    xReChar _begin_char, xReChar _end_char, xuLong *offs) {
    *offs = 0; *signal = 0;
    xReChar  * sp = regexp;

    while (stridx(WHITESPACE, sp[*offs]) >= 0) {
        (*offs) ++;
    }
    if (_begin_char) {
        if (sp[*offs] != _begin_char){
            *signal = -1;
            return -1;
        }
        (*offs)++;
    }
    while (stridx(WHITESPACE, sp[*offs]) >= 0) {
        (*offs) ++;
    }

    *idx = 2;
    xInt index = -1;
    if (sp[*offs] == dot) { *idx -= 2; (*offs) ++; }
    if (xReChar('a') <= sp[*offs] && sp[*offs] <= xReChar('z')) {
        *p = sp + *offs; *idx += 1;
        while (xReChar('a') <= sp[*offs] && sp[*offs] <= xReChar('z') ) (*offs) ++;
        *len = *offs - (*p - sp);
        index = arrayFindByAttr(context[*idx], (xuByte *) *p,
                                 (xuByte *(*)(void *)) labelGetName, *len * sizeof(xReChar));
    } else if (xReChar('0') <= sp[*offs] && sp[*offs] <= xReChar('9')) {
        *len = str2ul_d(sp + *offs, (xuLong *) &index);
        *offs += *len;
        if (index > context[*idx]->cur_len) index = -1;
    } else {
        *signal = -1; return -1;
    }

    while (stridx(WHITESPACE, sp[*offs]) >= 0) {
        (*offs) ++;
    }
    if (_end_char) {
        if (sp[*offs] != _end_char){
            *signal = -1;
            return -1;
        }
        (*offs) ++;
    }

    return index;
}

#undef LAMBDA

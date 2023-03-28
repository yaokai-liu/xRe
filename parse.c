//
// Created by Yaokai Liu on 2023/3/25.
//

#include "parse.h"
#include "meta.h"
#include "string.h"

#define BUFFER_LEN XRE_BASIC_ALLOCATE_LENGTH

static Group *parse(xReChar *regexp, xReChar _begin_char, xReChar _end_char, xuLong *offs, Allocator *allocator);
static ReObj * parseCrt(xReChar *regexp, xuLong *offs, Allocator *allocator);
static Sequence * parseSeq(xReChar * regexp, xuLong * offs, Allocator * allocator);
static Set * parseSet(xReChar * regexp, xuLong * offs, Allocator * allocator);
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


    xuLong n_bch = 0, branch_arr_len = BUFFER_LEN;
    ObjArray * branches = allocator->malloc(branch_arr_len * sizeof(ObjArray));

    branches[n_bch].n_objects = 0; xuLong obj_arr_len = BUFFER_LEN;
    branches[n_bch].objects = allocator->malloc(obj_arr_len * sizeof(ReObj *));
    n_bch ++;

    xuLong n_groups = 0, grp_arr_len = BUFFER_LEN;
    Group ** groups = allocator->malloc(grp_arr_len * sizeof(Group *));

    xuLong n_labels = 0, label_arr_len = BUFFER_LEN;
    Label * labels = allocator->malloc(label_arr_len * sizeof(Label));

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
                groups[n_groups] = obj;
                n_groups ++;
                break;
            }
            case beginC: {
                Count * cnt = nullptrof(Count);
                break;
            }
            case unionOR: {
                while (n_bch >= branch_arr_len) {
                    branch_arr_len += BUFFER_LEN;
                    xVoid * new = allocator->realloc(branches, branch_arr_len * sizeof(typeof(branches[0])));
                    if (!new) {
                        goto __failed_parse_group;
                    }
                    branches = new;
                }
                branches[n_bch].n_objects = 0; obj_arr_len = BUFFER_LEN;
                branches[n_bch].objects = allocator->malloc(obj_arr_len * sizeof(ReObj *));
                n_bch ++; (*offs) ++;
                continue;
            }
            case assign: {
                // TODO: assignment.
                break;
            }
            case call: {
                // TODO: call.
                break;
            }
            case copy: {
                // TODO: order.
                break;
            }
            case lastValue: {
                // TODO: last matched value.
            }
            case attribute: {
                // TODO: match condition.
            }
            default:
                while (hasChar(WHITESPACE, sp[*offs]) >= 0) {
                    (*offs) ++;
                }
                obj = parseSeq(sp + *offs, &offset, allocator);
        }
        *offs += offset;

        if (!obj) {
            goto __failed_parse_group;
        }
        ((ReObj *)obj)->only_match = _only_match;
        ((ReObj *)obj)->is_inverse = _is_inverse;
        _only_match = false; _is_inverse = false;

        if (branches[n_bch - 1].n_objects >= obj_arr_len) {
            obj_arr_len += BUFFER_LEN;
            xVoid * new = allocator->realloc(branches[n_bch - 1].objects, obj_arr_len * sizeof(ReObj *) );
            if (!new) {
                goto __failed_parse_group;
            }
            branches[n_bch - 1].objects = new;
        }
        branches[n_bch - 1].objects[branches[n_bch - 1].n_objects] = obj;
        branches[n_bch - 1].n_objects ++;
    }

    if (_end_char) {
        if (sp[*offs] != _end_char)
            goto __failed_parse_group;
        else
            (*offs) ++;
    }

    {
        xVoid *new = allocator->realloc(branches, (n_bch) * sizeof(ObjArray));
        if (new) {
            branches = new;
        }
    }
    for (typeof(n_bch) i = 0; i < n_bch; i ++) {
        if (branches[i].n_objects == 0) {
            goto __failed_parse_group;
        }
        xVoid * new = allocator->realloc(branches[i].objects, (branches[i].n_objects) * sizeof(ReObj *));
        if (new) {
            branches[i].objects = new;
        }
    }

    return createGrp(n_bch, branches, n_groups, groups, n_labels, labels, allocator);

    __failed_parse_group:
    for (typeof(n_bch) i = 0; i < n_bch; i++) {
        clearObjArray(&branches[i], allocator);
    }
    allocator->free(branches);
    allocator->free(groups);
    allocator->free(labels);
    return nullptrof(Group);
}

Sequence * parseSeq(xReChar * regexp, xuLong * offs, Allocator * allocator) {
    *offs = 0;
    xReChar * sp = regexp;
    xuLong seq_buf_len = BUFFER_LEN;
    xReChar * value = allocator->malloc(seq_buf_len * sizeof(xReChar));

    while (sp[*offs] && hasChar(NON_PLAIN, sp[*offs]) < 0) {
        while (*offs >= seq_buf_len) {
            seq_buf_len += BUFFER_LEN;
            xVoid * new = allocator->realloc(value, seq_buf_len * sizeof(xReChar));
            if (!new) {
                goto __failed_parse_sequence;
            }
            value = new;
        }
        value[*offs] = sp[*offs];
        (*offs) ++;
    }
    if (*offs == 0) {
        if (hasChar(WHITESPACE, sp[*offs]) < 0)
            goto __failed_parse_sequence;
    }
    xVoid * new = allocator->realloc(value, ((*offs) + 1)* sizeof(xReChar));
    if (new) {
        value = new;
    }
    value[(*offs) + 1] = 0;
    return createSeq(*offs, value, allocator);

    __failed_parse_sequence:
    allocator->free(value);
    return nullptrof(Sequence);
}

Set * parseSet(xReChar * regexp, xuLong * offs, Allocator * allocator) {
    *offs = 0;
    xReChar * sp = regexp;
    if (!sp[0] || sp[0] != beginS) {
        return nullptrof(Set);
    }
    (*offs)++;

    xuLong plain_buffer_len = BUFFER_LEN;
    xuLong range_buffer_len = BUFFER_LEN;
    xReChar * plain_buffer = allocator->malloc(plain_buffer_len * sizeof(xReChar));
    Range * range_buffer = allocator->malloc(range_buffer_len * sizeof(Range));
    xuLong n_plains = 0, n_ranges = 0;
    while (sp[*offs] && sp[*offs] != endS) {
        switch (sp[*offs]) {
            case escape:{
                xuLong offset = 0;
                ReObj * obj = parseCrt(sp + *offs, &offset, allocator);
                if (obj->id == SEQ) {
                    while (n_plains + ((Sequence *)obj)->len >= plain_buffer_len) {
                        plain_buffer_len += BUFFER_LEN;
                        xVoid * new = allocator->realloc(plain_buffer, plain_buffer_len * sizeof(xReChar));
                        if (!new) {
                            goto __failed_parse_set;
                        }
                        plain_buffer = new;
                    }
                    allocator->memcpy(((Sequence *)obj)->value, plain_buffer + n_plains, ((Sequence *)obj)->len);
                    n_plains += ((Sequence *)obj)->len;
                } else if (obj->id == SET) {
                    while (n_plains + ((Set *)obj)->n_plains >= plain_buffer_len) {
                        plain_buffer_len += BUFFER_LEN;
                        xVoid * new = allocator->realloc(plain_buffer, plain_buffer_len * sizeof(xReChar));
                        if (!new) {
                            goto __failed_parse_set;
                        }
                        plain_buffer = new;
                    }
                    allocator->memcpy(((Set *)obj)->plains, plain_buffer + n_plains, ((Set *)obj)->n_plains);
                    n_plains += ((Set *)obj)->n_plains;

                    while (n_ranges + ((Set *)obj)->n_ranges >= range_buffer_len) {
                        range_buffer_len += BUFFER_LEN;
                        xVoid * new = allocator->realloc(range_buffer, range_buffer_len * sizeof(Range));
                        if (!new) {
                            goto __failed_parse_set;
                        }
                        range_buffer = new;
                    }
                    allocator->memcpy(((Set *)obj)->ranges, range_buffer + n_ranges, ((Set *)obj)->n_ranges);
                    n_ranges += ((Set *)obj)->n_ranges;
                }
                // set from crt can not be free.
                break;
            }
            case rangeTO: {
                (*offs) ++;
                if (!sp[(*offs)] || (sp[(*offs)] != escape && hasChar(METAS, sp[(*offs)]) >= 0)) {
                    goto __failed_parse_set;
                }
                while (n_ranges >= range_buffer_len) {
                    range_buffer_len += BUFFER_LEN;
                    xVoid * new = allocator->realloc(range_buffer, range_buffer_len * sizeof(Range));
                    if (!new) {
                        goto __failed_parse_set;
                    }
                    range_buffer = new;
                }
                n_plains --;
                range_buffer[n_ranges].left = plain_buffer[n_plains];
                plain_buffer[n_plains] = 0;
                range_buffer[n_ranges].right = sp[(*offs)];
                n_ranges ++;
                (*offs) ++;
                break;
            }
            case dot: {
                break;
            }
            default: {
                while (n_plains >= plain_buffer_len) {
                    plain_buffer_len += BUFFER_LEN;
                    xVoid * new = allocator->realloc(plain_buffer, plain_buffer_len * sizeof(xReChar));
                    if (!new) {
                        goto __failed_parse_set;
                    }
                    plain_buffer = new;
                }
                plain_buffer[n_plains] = sp[(*offs)];
                (*offs) ++;
                n_plains ++;
            }
        }
    }
    if (sp[*offs] != endS) {
        goto __failed_parse_set;
    }
    (*offs) ++;

    if (n_plains == 0 && n_ranges == 0) {
        goto __failed_parse_set;
    }

    xVoid * new;
    new = allocator->realloc(plain_buffer, (n_plains + 1) * sizeof(xReChar));
    if (new) {
        plain_buffer = new;
    }
    plain_buffer[(n_plains + 1)] = 0;
    new = allocator->realloc(range_buffer, n_ranges * sizeof(Range));
    if (new) {
        range_buffer = new;
    }
    return createSet(n_plains, plain_buffer, n_ranges, range_buffer, allocator);

    __failed_parse_set:
    allocator->free(range_buffer);
    allocator->free(plain_buffer);
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

#undef BUFFER_LEN
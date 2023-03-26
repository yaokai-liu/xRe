//
// Created by Yaokai Liu on 2023/3/25.
//

#include "parse.h"
#include "meta.h"
#include "string.h"

#define BUFFER_LEN XRE_BASIC_ALLOCATE_LENGTH

static Group *parse(xReChar *regexp, xReChar _end_char, xuLong *offs, Allocator *allocator);
static Sequence * parseSeq(xReChar * regexp, xuLong * offs, Allocator * allocator);
static Set * parseSet(xReChar * regexp, xuLong * offs, Allocator * allocator);
static Group * parseGrp(xReChar * regexp, xuLong * offs, Allocator * allocator);
static Group * parseUniRHS(xReChar * regexp, xuLong * offs, Allocator * allocator);
static Expression ** parseCntExp(xReChar * regexp, xuLong * offs, Allocator * allocator);
static Expression * parseExp(xReChar * regexp, xuLong * offs, Allocator * allocator);

Group * xReProcessor_parse(XReProcessor *processor, xReChar *pattern) {
    return parse(pattern, xReChar('\0'), &(processor->errorLog.position), processor->allocator);
}

Set *parseCrt(xReChar *regexp, xuLong *offs, Allocator *allocator) {
    *offs = 0;
    xReChar *sp = regexp;

    if (sp[*offs] != xReChar('\\')) {
        return nullptrof(Set);
    }
    (*offs)++;

    switch (sp[*offs]) {
        case xReChar('u'): {
            // todo: unicode
            return nullptrof(Set);
        }
        case xReChar('x'): {
            // todo: unsigned hexadecimal integer
            return nullptrof(Set);
        }
        case xReChar('c'): {
            // todo: control character
            return nullptrof(Set);
        }
        case xReChar(' '):
            return &SPECIAL_SET_ARRAY[ssi_space];
        case xReChar('n'):
            return &SPECIAL_SET_ARRAY[ssi_n];
        case xReChar('r'):
            return &SPECIAL_SET_ARRAY[ssi_r];
        case xReChar('f'):
            return &SPECIAL_SET_ARRAY[ssi_f];
        case xReChar('v'):
            return &SPECIAL_SET_ARRAY[ssi_v];
        case xReChar('t'):
            return &SPECIAL_SET_ARRAY[ssi_t];
        case xReChar('s'):
            return &SPECIAL_SET_ARRAY[ssi_whitespace];
        case xReChar('S'):
            return &SPECIAL_SET_ARRAY[ssi_non_whitespace];
        case xReChar('w'):
            return &SPECIAL_SET_ARRAY[ssi_word];
        case xReChar('W'):
            return &SPECIAL_SET_ARRAY[ssi_non_word];
        default:
            return nullptrof(Set);
    }
}

Group *parse(xReChar *regexp, xReChar _end_char, xuLong *offs, Allocator *allocator) {
    *offs = 0;
    xReChar * sp = regexp;

    xuLong n_bch = 0, branch_arr_len = BUFFER_LEN;
    ObjArray * branches = allocator->malloc(branch_arr_len * sizeof(ObjArray));

    branches[n_bch].n_objs = 0; xuLong obj_arr_len = BUFFER_LEN;
    branches[n_bch].objects = allocator->malloc(obj_arr_len * sizeof(ReObj *));
    n_bch ++;

    xuLong n_groups = 0, grp_arr_len = BUFFER_LEN;
    Group ** groups = allocator->malloc(grp_arr_len * sizeof(Group *));

    xuLong n_labels = 0, label_arr_len = BUFFER_LEN;
    Label * labels = allocator->malloc(label_arr_len * sizeof(Label));

    while (sp[*offs] && sp[*offs] != _end_char) {
        xVoid * obj = nullptrof(xVoid);
        xuLong offset = 0;
        switch (sp[*offs]) {
            case escape: {
                obj = parseCrt(sp + *offs, &offset, allocator);
                break;
            }
            case beginS: {
                obj = parseSeq(sp + *offs, &offset, allocator);
                break;
            }
            case beginG: {
                obj = parse(sp + *offs, endG, &offset, allocator);
                groups[n_groups] = obj;
                n_groups ++;
                break;
            }
            case beginC: {
                Count * cnt = nullptrof(Count);
                Expression ** expressions = parseCntExp(sp + *offs, &offset, allocator);
                if (expressions) {
                    (branches[n_bch].n_objs) --;
                    obj = branches[n_bch].objects[branches[n_bch].n_objs];
                    cnt = createCnt(expressions, obj, allocator);
                }
                obj = cnt;
                break;
            }
            case unionOR: {
                while (n_bch > branch_arr_len) {
                    branch_arr_len += BUFFER_LEN;
                    xVoid * new = allocator->realloc(branches, branch_arr_len * sizeof(typeof(branches[0])));
                    if (!new) {
                        goto __failed_parse_group;
                    }
                    branches = new;
                }
                branches[n_bch].n_objs = 0; obj_arr_len = BUFFER_LEN;
                branches[n_bch].objects = allocator->malloc(obj_arr_len * sizeof(ReObj *));
                n_bch ++;
                break;
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
                obj = parseSeq(sp + *offs, &offset, allocator);
        }
        *offs += offset;

        if (!obj) {
            goto __failed_parse_group;
        }
        if (branches[n_bch - 1].n_objs >= obj_arr_len) {
            obj_arr_len += BUFFER_LEN;
            xVoid * new = allocator->realloc(branches[n_bch - 1].objects, obj_arr_len * sizeof(ReObj *) );
            if (!new) {
                goto __failed_parse_group;
            }
            branches[n_bch - 1].objects = new;
        }
        branches[n_bch - 1].objects[branches[n_bch].n_objs] = obj;
        branches[n_bch - 1].n_objs ++;
    }

    if (_end_char && sp[*offs] != _end_char) {
        goto __failed_parse_group;
    }

    {
        xVoid *new = allocator->realloc(branches, (n_bch) * sizeof(ObjArray));
        if (new) {
            branches = new;
        }
    }
    for (typeof(n_bch) i = 0; i < n_bch; i ++) {
        if (branches[i].n_objs == 0) {
            goto __failed_parse_group;
        }
        xVoid * new = allocator->realloc(branches[i].objects, (branches[i].n_objs) * sizeof(ReObj *));
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

    while (sp[*offs] && !hasChar(NON_PLAIN, sp[*offs])) {
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
        if (!hasChar(WHITESPACE, sp[*offs]))
            goto __failed_parse_sequence;
    }
    xVoid * new = allocator->realloc(value, *offs * sizeof(xReChar));
    if (new) {
        value = new;
    }
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
    sp++;

    xuLong plain_buffer_len = BUFFER_LEN;
    xuLong range_buffer_len = BUFFER_LEN;
    xReChar * plain_buffer = allocator->malloc(plain_buffer_len * sizeof(xReChar));
    Range * range_buffer = allocator->malloc(range_buffer_len * sizeof(Range));
    xuLong n_plains = 0, n_ranges = 0;
    while (sp[*offs] && sp[*offs] != endS) {
        if (sp[*offs] && !hasChar(METAS, sp[*offs])) {
            while (n_plains  >= plain_buffer_len) {
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
            continue;
        }
        if (sp[*offs] == escape) {
            xuLong offset = 0;
            Set * set = parseCrt(sp + *offs, &offset, allocator);
            for (int i = 0; i < set->n_plains; i ++) {
                if (!hasChar(plain_buffer, set->plain[i])) {
                    while (n_plains  >= plain_buffer_len) {
                        plain_buffer_len += BUFFER_LEN;
                        xVoid * new = allocator->realloc(plain_buffer, plain_buffer_len * sizeof(xReChar));
                        if (!new) {
                            goto __failed_parse_set;
                        }
                        plain_buffer = new;
                    }
                    plain_buffer[n_plains] = set->plain[i];
                    n_plains ++;
                }
            }
            for (int i = 0; i < set->n_ranges; i ++) {
                xBool not_include_range = true;
                for (int j = 0; j < n_ranges; j ++) {
                    if (set->ranges[i].left >= range_buffer[j].left
                     && set->ranges[i].right >= range_buffer[j].right) {
                        not_include_range = false;
                        break;
                    }
                }
                if (!not_include_range) {
                    while (n_ranges >= range_buffer_len) {
                        range_buffer_len += BUFFER_LEN;
                        xVoid * new = allocator->realloc(range_buffer, range_buffer_len * sizeof(Range));
                        if (!new) {
                            goto __failed_parse_set;
                        }
                        range_buffer = new;
                    }
                    range_buffer[n_ranges] = set->ranges[i];
                    n_ranges ++;
                }
            }
            // set from crt can not be free.
        }
        if (sp[*offs] == rangeTO) {
            if (!sp[(*offs) + 1] || (sp[(*offs) + 1] != escape && hasChar(METAS, sp[*offs]))) {
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
            (*offs) ++;
            n_plains --;
            range_buffer[n_ranges].left = plain_buffer[n_plains];
            range_buffer[n_ranges].right = sp[(*offs) + 1];
            n_ranges ++;
            continue;
        }
        goto __failed_parse_set;
    }
    if (sp[*offs] != endS) {
        goto __failed_parse_set;
    }
    (*offs) ++;

    if (n_plains == 0 && n_ranges == 0) {
        goto __failed_parse_set;
    }

    xVoid * new;
    new = allocator->realloc(plain_buffer, n_plains * sizeof(xReChar));
    if (new) {
        plain_buffer = new;
    }
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

    if (sp[*offs] != beginC) {
        return nullptrof(Expression *);
    }
    (*offs) ++;

    xuLong offset;
    Expression ** expressions = allocator->calloc(3, sizeof(Expression *));

    static xReChar * default_expr[] = { xReString("0"), xReString("0"), xReString("1")};
    for (int i = 0; i < 3; i ++){
        expressions[i] = parseExp(sp + *offs, &offset, allocator);
        if (offset > 0) {
            (*offs) += offset;
            if (!expressions[i]) {
                goto __failed_parse_count;
            }
        } else {
            expressions[i] = parseExp(default_expr[i], &offset, allocator);
        }
        if (sp[*offs] != comma && sp[*offs] != endC) {
            goto __failed_parse_count;
        }
        (*offs)++;
    }

    return expressions;

    __failed_parse_count:
    for (int i = 0; i < 3; i++) {
        if (expressions[i]) {
            releaseObj(expressions[i], allocator);
        }
    }
    allocator->free(expressions);
    return nullptrof(Expression *);

}

Expression * parseExp(xReChar * regexp, xuLong * offs, Allocator * allocator) {

}

#undef BUFFER_LEN
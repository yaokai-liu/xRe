//
// Created by Yaokai Liu on 2023/2/18.
//

#include "parse.h"
typedef enum {
    escape      = '\\',     // escape char: '\'

    beginG      = '(',      // begin of group : '('
    endG        = ')',      // end of group: ')'
    beginS      = '[',      // begin of set: '['
    endS        = ']',      // end of set: ']'
    beginC      = '{',      // begin of count: '{'
    endC        = '}',      // end of count: '}'
    beginV      = '<',      // begin of variable: '<'
    endV        = '>',      // end of variable: '>'

    rangeTO     = '-',      // range to: '-'

    comma       = ',',       // comma: ','
    unionOR     = '|',       // group union: '|'
    inverse     = '!',       // group inverse: '^'
    assign      = '=',       // group assign to variable: '='
    call        = '@',       // call variable: '@'
    orderOf     = '#',       // order of variable: '#'
    lastValue   = '$',       // last matched result of variable: '￥'
    condWith    = '~',       // match condition: '~'
    onlyParse   = '^',       // only parse but not execute: '!'

    alias       = 128,       // alias char like: '*', '+', '?', etc.

    plain       = 128 + 127,      // plain char
} cat_t;

xBool CAT_ARRAY[] = {
        [escape]    = escape,
        [beginG]    = beginG,
        [endG]      = endG,
        [beginS]    = beginS,
        [endS]      = endS,
        [beginC]    = beginC,
        [endC]      = endC,
        [beginV]    = beginV,
        [endV]      = endV,
        [rangeTO]   = rangeTO,
        [comma]     = comma,
        [unionOR]   = unionOR,
        [inverse]   = inverse,
        [assign]    = assign,
        [call]      = call,
        [orderOf]   = orderOf,
        [condWith]  = condWith,
        [onlyParse] = onlyParse,
};

Sequence * parseSeq(xReChar * regexp, xuLong * offs, Allocator * allocator);
Set * parseSet(xReChar * regexp, xuLong * offs, Allocator * allocator);
Group * parseGrp(xReChar * regexp, xuLong * offs, Allocator * allocator);
Group * parseUniRHS(xReChar * regexp, xuLong * offs, Allocator * allocator);
Expression ** parseCntExp(xReChar * regexp, xuLong * offs, Allocator * allocator);
Expression * parseExp(xReChar * regexp, xuLong * offs, Allocator * allocator);

Group * parse(xReChar * regexp, xuLong * offs, Allocator * allocator) {

}

Sequence * parseSeq(xReChar * regexp, xuLong * offs, Allocator * allocator) {
    *offs = 0;
    xReChar * sp = regexp;
    xReChar * value = allocator->malloc(XRE_PARSE_ALLOCATE_SIZE);
    if (!value) {
        return nullptrof(Sequence);
    }
    xuLong seq_buf_len = XRE_PARSE_ALLOCATE_SIZE;
    while (sp[*offs] && (sp[*offs] >= lenof(CAT_ARRAY) || CAT_ARRAY[*sp] == 0)) {
        while ((*offs) * sizeof(xReChar) >= seq_buf_len) {
            seq_buf_len += XRE_PARSE_ALLOCATE_SIZE;
            void * new = allocator->realloc(value, seq_buf_len);
            if (!new) {
                goto __failed_parse_sequence;
            }
            value = new;
        }
        value[*offs] = sp[*offs];
        (*offs) ++;
    }
    if (*offs == 0) {
        goto __failed_parse_sequence;
    }
    void * new = allocator->realloc(value, *offs * sizeof(xReChar));
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
    xuLong plain_buffer_len = XRE_PARSE_ALLOCATE_SIZE;
    xuLong range_buffer_len = XRE_PARSE_ALLOCATE_SIZE;
    xReChar * plain_buffer = allocator->malloc(plain_buffer_len);
    Range * range_buffer = allocator->malloc(range_buffer_len);
    xuLong n_plains = 0, n_ranges = 0;
    while (sp[*offs] && sp[*offs] != endS) {
        if (sp[*offs] >= lenof(CAT_ARRAY) || CAT_ARRAY[*sp] == 0) {
            while (n_plains * sizeof(xReChar) > plain_buffer_len) {
                plain_buffer_len += XRE_PARSE_ALLOCATE_SIZE;
                xVoid * new = allocator->realloc(plain_buffer, plain_buffer_len);
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
        if (sp[*offs] == rangeTO) {
            while (n_ranges * sizeof(Range) > range_buffer_len) {
                range_buffer_len += XRE_PARSE_ALLOCATE_SIZE;
                xVoid * new = allocator->realloc(range_buffer, range_buffer_len);
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

Group * parseGrp(xReChar * regexp, xuLong * offs, Allocator * allocator) {
    *offs = 0;
    xReChar * sp = regexp;
    if (sp[*offs] != beginG) {
        return nullptrof(Group);
    }
    (*offs) ++;

    xuLong n_subs = 0;
    xuLong sub_buffer_len = XRE_PARSE_ALLOCATE_SIZE;
    ReObj ** sub_buffer = allocator->malloc(sub_buffer_len);
    xBool is_inverse = false;
    xBool only_parse = false;
    while (sp[*offs] && sp[*offs] != endG) {
        xVoid * obj = nullptrof(xVoid);
        xuLong offset = 0;
        switch (sp[*offs]) {
            case inverse:
                is_inverse = true;
                (*offs) ++;
                continue;
            case onlyParse:
                only_parse = true;
                (*offs) ++;
                continue;
            case beginS: {
                obj = parseSeq(sp + *offs, &offset, allocator);
                break;
            }
            case beginG: {
                obj = parseGrp(sp + *offs, &offset, allocator);
                break;
            }
            case beginC: {
                Count * cnt = nullptrof(Count);
                Expression ** expressions = parseCntExp(sp + *offs, &offset, allocator);
                if (expressions) {
                    n_subs --;
                    cnt = createCnt(expressions[0], expressions[1], sub_buffer[n_subs], allocator);
                }
                obj = cnt;
                break;
            }
            case unionOR: {
                Group * rhs = parseUniRHS(sp + *offs, &offset, allocator);
                if(rhs) {
                    void * new = allocator->realloc(sub_buffer, n_subs * sizeof(ReObj *));
                    if (new) {
                        sub_buffer = new;
                    }
                    xVoid * lhs = createGrp(n_subs, sub_buffer, allocator);
                    Union * uni = createUni(lhs, rhs, allocator);
                    obj = uni;
                    n_subs = 0;
                    sub_buffer_len = XRE_PARSE_ALLOCATE_SIZE;
                    sub_buffer = allocator->malloc(sub_buffer_len);
                } else {
                    obj = nullptrof(Union);
                }
                break;
            }
            case assign: {
                break;
            }
            default:
                obj = parseSeq(sp + *offs, &offset, allocator);
        }
        if (!obj) {
            goto __failed_parse_group;
        }
        while (n_subs * sizeof(ReObj *) >= sub_buffer_len) {
            sub_buffer_len += XRE_PARSE_ALLOCATE_SIZE;
            void * new = allocator->realloc(sub_buffer, sub_buffer_len);
            if (!new) {
                goto __failed_parse_group;
            }
            sub_buffer = new;
        }
        ((ReObj *) obj)->is_inverse = is_inverse;
        ((ReObj *) obj)->only_parse = only_parse;
        is_inverse = false;
        only_parse = false;
        sub_buffer[n_subs] = obj;
        *offs += offset;
        n_subs ++;
    }

    if (sp[*offs] != endG) {
        goto __failed_parse_group;
    }
    (*offs) ++;

    if (n_subs == 0) {
        goto __failed_parse_group;
    }

    void * new = allocator->realloc(sub_buffer, n_subs * sizeof(ReObj *));
    if (new) {
        sub_buffer = new;
    }
    return createGrp(n_subs, sub_buffer, allocator);

    __failed_parse_group:
    for (xuLong i = 0; i < n_subs; i++) {
        releaseObj(sub_buffer[i], allocator);
    }
    allocator->free(sub_buffer);
    return nullptrof(Group);
}

Group * parseUniRHS(xReChar * regexp, xuLong * offs, Allocator * allocator) {
    *offs = 0;
    xReChar * sp = regexp;
    if (sp[*offs] != unionOR) {
        return nullptrof(Group);
    }

    xuLong n_subs = 0;
    xuLong sub_buffer_len = XRE_PARSE_ALLOCATE_SIZE;
    ReObj ** sub_buffer = allocator->malloc(sub_buffer_len);
    xBool is_inverse = false;
    xBool only_parse = false;
    while (sp[*offs]) {
        xBool end_rhs = false;
        xVoid * obj = nullptrof(xVoid);
        xuLong offset = 0;
        switch (sp[*offs]) {
            case inverse:
                is_inverse = true;
                (*offs) ++;
                continue;
            case onlyParse:
                only_parse = true;
                (*offs) ++;
                continue;
            case beginS: {
                obj = parseSeq(sp + *offs, &offset, allocator);
                break;
            }
            case beginG: {
                obj = parseGrp(sp + *offs, &offset, allocator);
                break;
            }
            case beginC: {
                Count * cnt = nullptrof(Count);
                Expression ** expressions = parseCntExp(sp + *offs, &offset, allocator);
                if (expressions) {
                    n_subs --;
                    cnt = createCnt(expressions[0], expressions[1], sub_buffer[n_subs], allocator);
                }
                obj = cnt;
                break;
            }
            case unionOR: {
                Group * rhs = parseUniRHS(sp + *offs, &offset, allocator);
                if(rhs) {
                    void * new = allocator->realloc(sub_buffer, n_subs * sizeof(ReObj *));
                    if (new) {
                        sub_buffer = new;
                    }
                    xVoid * lhs = createGrp(n_subs, sub_buffer, allocator);
                    Union * uni = createUni(lhs, rhs, allocator);
                    obj = uni;
                    n_subs = 0;
                    sub_buffer_len = XRE_PARSE_ALLOCATE_SIZE;
                    sub_buffer = allocator->malloc(sub_buffer_len);
                } else {
                    obj = nullptrof(Union);
                }
                break;
            }
            case assign: {
                break;
            }
            default:
                obj = parseSeq(sp + *offs, &offset, allocator);
                if (!obj) {
                    end_rhs = true;
                }
        }
        if (!obj) {
            if (end_rhs) {
                break;
            }
            goto __failed_parse_union_rhs;
        }
        while (n_subs * sizeof(ReObj *) >= sub_buffer_len) {
            sub_buffer_len += XRE_PARSE_ALLOCATE_SIZE;
            void * new = allocator->realloc(sub_buffer, sub_buffer_len);
            if (!new) {
                goto __failed_parse_union_rhs;
            }
            sub_buffer = new;
        }
        ((ReObj *) obj)->is_inverse = is_inverse;
        ((ReObj *) obj)->only_parse = only_parse;
        is_inverse = false;
        only_parse = false;
        sub_buffer[n_subs] = obj;
        *offs += offset;
        n_subs ++;
    }

    if (n_subs == 0) {
        goto __failed_parse_union_rhs;
    }

    return createGrp(n_subs, sub_buffer, allocator);

    __failed_parse_union_rhs:
    for (xuLong i = 0; i < n_subs; i++) {
        releaseObj(sub_buffer[i], allocator);
    }
    allocator->free(sub_buffer);
    return nullptrof(Group);
}

Expression ** parseCntExp(xReChar * regexp, xuLong * offs, Allocator * allocator) {
    *offs = 0;
    xReChar * sp = regexp;

    if (sp[*offs] != beginC) {
        return nullptrof(Expression *);
    }
    (*offs) ++;
    xuLong offset;
    Expression ** expressions = allocator->malloc(2 * sizeof(Expression *));
    expressions[0] = parseExp(sp + *offs, &offset, allocator);
    if (!expressions[0]) {
        if (offset > 0) {
            allocator->free(expressions);
            return nullptrof(Expression *);
        }
        expressions[0] = parseExp(xReChar("0"), &offset, allocator);
    } else {
        (*offs) += offset;
    }
    expressions[1] = parseExp(sp + *offs, &offset, allocator);
    if (!expressions[1]) {
        if (offset > 0) {
            releaseObj(expressions[0], allocator);
            allocator->free(expressions);
            return nullptrof(Expression *);
        }
        expressions[1] = parseExp(xReChar("0"), &offset, allocator);
    } else {
        (*offs) += offset;
    }

    return expressions;
}

Expression * parseExp(xReChar * regexp, xuLong * offs, Allocator * allocator) {

}

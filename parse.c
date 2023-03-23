//
// Created by Yaokai Liu on 2023/2/18.
//

#include "parse.h"
#include "meta.h"
#include "string.h"

typedef struct XReParser {
    struct {
        xuLong position;
        xReChar message[256];
    } errorLog;
    enum {
        regular = 0,
        arithmetic = 1,
    } state;
    Allocator * allocator;
    Group * (* parse)(XReParser * parser, xReChar * regexp);
} XReParser;

static Group * xReParser_parse(XReParser *parser, xReChar *regexp);

Group * parse(xReChar * regexp, xuLong * offs, Allocator * allocator);
Sequence * parseSeq(xReChar * regexp, xuLong * offs, Allocator * allocator);
Set * parseSet(xReChar * regexp, xuLong * offs, Allocator * allocator);
Group * parseGrp(xReChar * regexp, xuLong * offs, Allocator * allocator);
Group * parseUniRHS(xReChar * regexp, xuLong * offs, Allocator * allocator);
Expression ** parseCntExp(xReChar * regexp, xuLong * offs, Allocator * allocator);
Expression * parseExp(xReChar * regexp, xuLong * offs, Allocator * allocator);

static Group * xReParser_parse(XReParser *parser, xReChar *regexp) {
    return parse(regexp, &(parser->errorLog.position), parser->allocator);
}

XReParser * xReParser(Allocator * _allocator) {
    XReParser * parser = _allocator->calloc(1, sizeof(XReParser));
    parser->allocator = _allocator;
    parser->parse = xReParser_parse;
    return parser;
}



typedef enum {
    escape      = xReChar('\\'),     // escape char: '\'

    beginG      = xReChar('('),      // begin of group : '('
    endG        = xReChar(')'),      // end of group: ')'
    beginS      = xReChar('['),      // begin of set: '['
    endS        = xReChar(']'),      // end of set: ']'
    beginC      = xReChar('{'),      // begin of count: '{'
    endC        = xReChar('}'),      // end of count: '}'
    beginV      = xReChar('<'),      // begin of expression: '<'
    endV        = xReChar('>'),      // end of expression: '>'

    rangeTO     = xReChar('-'),      // range to: '-'

    comma       = xReChar(','),       // comma: ','
    unionOR     = xReChar('|'),       // group union: '|'
    assign      = xReChar('='),       // group assign to variable: '='
    call        = xReChar('@'),       // call variable: '@'
    orderOf     = xReChar('#'),       // copy: of variable: '#'
    lastValue   = xReChar('$'),       // last matched result of variable: '￥'
    attribute   = xReChar('~'),       // attribute: '~'
    dot         = xReChar('.'),       // get label: '.'
} meta_cat_t;

xBool META_CAT_ARRAY[] = {
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
        [assign]    = assign,
        [call]      = call,
        [orderOf]   = orderOf,
        [attribute]  = attribute,
};

Group * parse(xReChar * regexp, xuLong * offs, Allocator * allocator) {
    *offs = 0;
    xReChar * sp = regexp;

    xuLong n_subs = 0;
    xuLong sub_buffer_len = XRE_PARSE_ALLOCATE_SIZE;
    ReObj ** sub_buffer = allocator->malloc(sub_buffer_len);
    while (sp[*offs]) {
        xVoid * obj = nullptrof(xVoid);
        xuLong offset = 0;
        switch (sp[*offs]) {
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
                // TODO: assignment.
                break;
            }
            case call: {
                // TODO: call.
                break;
            }
            case orderOf: {
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
        while (n_subs * sizeof(ReObj *) >= sub_buffer_len) {
            sub_buffer_len += XRE_PARSE_ALLOCATE_SIZE;
            void * new = allocator->realloc(sub_buffer, sub_buffer_len);
            if (!new) {
                goto __failed_parse_group;
            }
            sub_buffer = new;
        }
        sub_buffer[n_subs] = obj;
        n_subs ++;
    }

    if (*offs == 0 && n_subs == 0) {
        return nullptrof(Group);
    }

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

Sequence * parseSeq(xReChar * regexp, xuLong * offs, Allocator * allocator) {
    *offs = 0;
    xReChar * sp = regexp;
    xReChar * value = allocator->malloc(XRE_PARSE_ALLOCATE_SIZE);
    if (!value) {
        return nullptrof(Sequence);
    }
    xuLong seq_buf_len = XRE_PARSE_ALLOCATE_SIZE;
    while (sp[*offs] && (sp[*offs] >= lenof(META_CAT_ARRAY) || META_CAT_ARRAY[*sp] == 0)) {
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
        if (sp[*offs] >= lenof(META_CAT_ARRAY) || META_CAT_ARRAY[*sp] == 0) {
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
    while (sp[*offs] && sp[*offs] != endG) {
        xVoid * obj = nullptrof(xVoid);
        xuLong offset = 0;
        switch (sp[*offs]) {
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
        *offs += offset;

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
        sub_buffer[n_subs] = obj;
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
    while (sp[*offs]) {
        xBool end_rhs = false;
        xVoid * obj = nullptrof(xVoid);
        xuLong offset = 0;
        switch (sp[*offs]) {
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
        *offs += offset;

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
        sub_buffer[n_subs] = obj;
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
    if (offset > 0) {
        (*offs) += offset;
        if (!expressions[0]) {
            allocator->free(expressions);
            return nullptrof(Expression *);
        }
    } else {
        expressions[0] = parseExp(xReString("0"), &offset, allocator);
    }

    if (sp[*offs] != comma && sp[*offs] != endC) {
        releaseObj(expressions[0], allocator);
        allocator->free(expressions);
        return nullptrof(Expression *);
    }
    (*offs) ++;

    expressions[1] = parseExp(sp + *offs, &offset, allocator);
    if (offset > 0) {
        (*offs) += offset;
        if (!expressions[1]) {
            releaseObj(expressions[0], allocator);
            allocator->free(expressions);
            return nullptrof(Expression *);
        }
    } else {
        expressions[1] = parseExp(xReString("0"), &offset, allocator);
    }

    if (sp[*offs] != endC) {
        releaseObj(expressions[0], allocator);
        releaseObj(expressions[1], allocator);
        allocator->free(expressions);
        return nullptrof(Expression *);
    }
    (*offs) ++;

    return expressions;
}

Expression * parseExp(xReChar * regexp, xuLong * offs, Allocator * allocator) {

}

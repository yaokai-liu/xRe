//
// Created by Yaokai Liu on 2023/3/23.
//

#ifndef X_META_H
#define X_META_H

#include "char_t.h"
typedef enum {
    escape      = xReChar('\\'),     // escape char: '\'

    beginG      = xReChar('('),      // atBegin of group : '('
    endG        = xReChar(')'),      // atEnd of group: ')'
    beginS      = xReChar('['),      // atBegin of set: '['
    endS        = xReChar(']'),      // atEnd of set: ']'
    beginC      = xReChar('{'),      // atBegin of count: '{'
    endC        = xReChar('}'),      // atEnd of count: '}'
    beginL      = xReChar('<'),      // atBegin of expression: '<'
    endL        = xReChar('>'),      // atEnd of expression: '>'

    endP        = xReChar(' '),      // atEnd of plain text: '\s'
    rangeTO     = xReChar('-'),      // range to: '-'

    comma       = xReChar(','),       // comma: ','
    unionOR     = xReChar('|'),       // group union: '|'
    assign      = xReChar('='),       // group assign to variable: '='
    call        = xReChar('@'),       // call variable: '@'
    lastValue   = xReChar('$'),       // last matched result of variable: '$'

    atBegin     = xReChar('&'),       // atBegin: '&'
    atEnd       = xReChar('~'),       // atEnd: '~'
    dot         = xReChar('.'),       // macro or get label: '.'
    plus        = xReChar('+'),       // macro: '+'
    star        = xReChar('*'),       // macro: '*'
    quesMark    = xReChar('?'),       // macro: '?'

    only_match  = xReChar('!'),       // only match but not capture: '!'
    is_inverse  = xReChar('^'),       // inverse match: '^'
} meta_cat_t;

#define METAS xReString("()[]{}<>\\|@#$~:=+-*/?!^.")

#define WHITESPACE xReString("\n\r\f\v\t ")

#define CRLF xReString("\n\r")

// NON_PLAIN = METAS + WHITESPACE
#define NON_PLAIN xReString("\n\r\f\v\t ()[]{}<>\\|@#$~:=+-*/?!^.")

#define SINGLE_ESCAPE xReString("nrfvt ()[]{}<>\\|@#$~:=+-*/?!^.sSwW")


#endif //X_META_H

//
// Created by Yaokai Liu on 2023/3/23.
//

#ifndef X_META_H
#define X_META_H

#include "char_t.h"
typedef enum {
    escape      = xReChar('\\'),     // escape char: '\'

    beginG      = xReChar('('),      // begin of group : '('
    endG        = xReChar(')'),      // end of group: ')'
    beginS      = xReChar('['),      // begin of set: '['
    endS        = xReChar(']'),      // end of set: ']'
    beginC      = xReChar('{'),      // begin of count: '{'
    endC        = xReChar('}'),      // end of count: '}'
    beginL      = xReChar('<'),      // begin of expression: '<'
    endL        = xReChar('>'),      // end of expression: '>'

    endP        = xReChar(' '),      // end of plain text: '\s'
    rangeTO     = xReChar('-'),      // range to: '-'

    comma       = xReChar(','),       // comma: ','
    unionOR     = xReChar('|'),       // group union: '|'
    assign      = xReChar('='),       // group assign to variable: '='
    call        = xReChar('@'),       // call variable: '@'
    copy        = xReChar('#'),       // copy: of variable: '#'
    lastValue   = xReChar('$'),       // last matched result of variable: '$'
    attribute   = xReChar('~'),       // attribute: '~'

    dot         = xReChar('.'),       // macro or get label: '.'
    plus        = xReChar('+'),       // macro: '+'
    star        = xReChar('*'),       // macro: '*'
    quesMark    = xReChar('?'),       // macro: '?'

    only_match  = xReChar('!'),       // only match but not capture: '!'
    is_inverse  = xReChar('^'),       // inverse match: '^'
} meta_cat_t;

#define METAS xReString("()[]{}<>\\|@#$~:=+-*/?!^.")

#define WHITESPACE xReString("\n\r\f\v\t ")

// NON_PLAIN = METAS + WHITESPACE
#define NON_PLAIN xReString("\n\r\f\v\t ()[]{}<>\\|@#$~:=+-*/?!^.")

#define SINGLE_ESCAPE xReString("nrfvt ()[]{}<>\\|@#$~:=+-*/?!^.sSwW")


#endif //X_META_H

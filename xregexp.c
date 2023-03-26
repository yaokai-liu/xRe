//
// Created by Yaokai Liu on 2023/2/18.
//

#include "xregexp.h"
#include "meta.h"
#include "parse.h"
#include "match.h"
#include "compile.h"

XReProcessor * xReProcessor(Allocator * allocator) {
    XReProcessor * processor = allocator->calloc(1, sizeof(XReProcessor));
    processor->allocator = allocator;
    processor->parse = xReProcessor_parse;
//    processor->match_o = xReProcessor_match_o;
//    processor->match_c = xReProcessor_match_c;
//    processor->compile_o = xReProcessor_compile_o;
//    processor->compile_c = xReProcessor_compile_c;
    return processor;
}

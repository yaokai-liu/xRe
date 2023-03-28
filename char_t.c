//
// Created by Yaokai Liu on 2023/2/18.
//

#include "char_t.h"
#ifndef XRE_CHAR_USING_XCHAR

xuInt xRe_ord(xReChar _the_char) {
    return _the_char;
}

xReChar xRe_chr(xuInt _the_order) {
    return (xReChar) _the_order;
}

#endif
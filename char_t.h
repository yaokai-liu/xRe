//
// Created by Yaokai Liu on 2023/2/18.
//

#ifndef X_CHAR_T_H
#define X_CHAR_T_H

#include "xtypes.h"
#include "xchar.h"

typedef xByte ascii;


#ifdef XRE_CHAR_USING_CHAR
typedef char   xReChar;
#define xReChar(x)      x
#define xReChar(x)    x
#elif defined(XRE_CHAR_USING_WCHAR)
#ifdef __WCHAR_TYPE__
typedef __WCHAR_TYPE__  xReChar;
#else
typedef xInt     xReChar;
#endif
#define xReChar(x)      L##x
#define xReString(x)    L##x
#elif defined(XRE_CHAR_USING_ASCII)
typedef ascii xReChar;
#define xReChar(x)      x
#define xReString(x)   ((ascii *) (x))
#elif defined(XRE_CHAR_USING_XASCII)
typedef xAscii xReChar;
#define xReChar(x)      x
#define xReString(x)    ((xAscii *) (x))
#elif defined(XRE_CHAR_USING_XCHAR)
typedef xChar xReChar;
#define xReChar(x)      ((xChar) x)
#define xReString(x)    ((xChar *) x)
#else
#error "You should specify character type."
#endif

#ifndef XRE_CHAR_USING_XCHAR
xuInt xRe_ord(xReChar _the_char);
xReChar xRe_chr(xuInt _the_order);
#else
#define xRe_ord     ord
#define xRe_chr     chr
#endif

#endif //X_CHAR_T_H

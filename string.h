//
// Created by Yaokai Liu on 2023/3/23.
//

#ifndef X_STRING_H
#define X_STRING_H

#include "char_t.h"

xInt stridx(const xReChar * _string, xReChar _chr);
xBool strcmp(const xReChar * _str1, const xReChar * _str2);
xuInt str2ul_d(const xReChar *_str, xuLong *_val_dest);

#endif //X_STRING_H

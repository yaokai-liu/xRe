//
// Created by Yaokai Liu on 2023/3/23.
//

#include "string.h"

xInt stridx(const xReChar *_string, xReChar _chr) {
    for (xInt i = 0; _string[i] != xReChar('\0'); i++) {
        if (_chr == _string[i])
            return i;
    }
    return -1;
}

xBool strcmp(const xReChar * _str1, const xReChar * _str2) {
    if (_str1 == _str2) return true;
    if (!_str1 || !_str2) return false;
    for (xInt i = 0; _str1[i] == _str2[i]; i ++)
        if (!_str1[i]) return true;
    return false;
}

xuInt str2ul_d(const xReChar *_str, xuLong *_val_dest) {
    int i = 0;
    for (; _str[i]; i++) {
        if (xReChar('0') <= _str[i] && _str[i] <= xReChar('9')) {
            *_val_dest += _str[i] - xReChar('0');
        } else {
            return i;
        }
    }
    return 0;
}
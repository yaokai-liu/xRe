//
// Created by Yaokai Liu on 2023/3/23.
//

#include "string.h"

xInt hasChar(const xReChar *_string, xReChar _chr) {
    for (int i = 0; _string[i] != xReChar('\0'); i++) {
        if (_chr == _string[i])
            return i;
    }
    return -1;
}

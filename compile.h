//
// Created by Yaokai Liu on 2023/3/25.
//

#ifndef X_COMPILE_H
#define X_COMPILE_H

#include "xregexp.h"

executor * xReProcessor_compile_o(ReObj * obj);
executor * xReProcessor_compile_c(const xReChar * pattern);


#endif //X_COMPILE_H

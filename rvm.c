//
// Created by Yaokai Liu on 2023/4/4.
//

#include "rvm.h"
#include "char_t.h"

typedef xuByte re_register[TARGET_BYTE_SIZE];
typedef struct {
    re_register    ar; // arithmetic register 1
    re_register    br; // arithmetic register 2
    re_register    cr; // arithmetic register 3
    re_register    dr; // data header address register
    re_register    ir; // inst header address register
    re_register    sp; // string pointer
    re_register    ip; // inst pointer
    re_register    sr; // state register

} XReVMProcessor;




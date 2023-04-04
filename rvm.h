//
// Created by Yaokai Liu on 2023/4/4.
//

#ifndef X_RVM_H
#define X_RVM_H

#include "xtypes.h"

#define TARGET_BYTE_SIZE    8

typedef enum {
    start = 1,
    plain_1,
    plain_2,
    plain_4,
    plain_8,
    ver_1,
    ver_2,
    ver_4,
    ver_8,
    range_1,
    range_2,
    range_4,
    jump,
    jnc,
    capture,
    set_flag,
    stage,
    reset,
    ret,
    end = 255,
} opcode;

typedef struct {
    opcode operate;
    xuByte pattern[8];
} Inst;

typedef struct {
    xuInt capture_times;
    xuInt last_pos;
    xuInt last_len;
} Variable;

#endif //X_RVM_H

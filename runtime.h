//
// Created by Yaokai Liu on 2023/4/7.
//

#ifndef X_RUNTIME_H
#define X_RUNTIME_H

#include "xtypes.h"
#include "structs.h"

typedef struct {
    xuByte obj_type;
    xBool  inverse;
} ArgsHeader [[gnu::aligned(2)]];

typedef struct {
    ArgsHeader attr;
    xuInt len;
    xuByte * pattern;
} SeqArgs;

typedef struct {
    ArgsHeader attr;
    xuInt n_plains;
    xuInt n_ranges;
    xuByte * plains;
    Range * ranges;
} SetArgs;

typedef struct {
    ArgsHeader attr;
    xuInt min;
    xuInt max;
    xuInt step;
    ArgsHeader * obj_args;
} CountArgs;

typedef struct {
    ArgsHeader attr;
    call_t target_type: 8;
    xuInt last_len;
    xuByte * last_val;
    ArgsHeader * obj_args;
} CallArgs;

typedef struct {
    xuInt  n_objects;
    ArgsHeader ** obj_list;
} BranchArgs;

typedef struct {
    ArgsHeader attr;
    xuInt  n_branch;
    BranchArgs * branch_list;
} GroupArgs;

xuInt match_seq(const xuByte * sp, const SeqArgs * args);
xuInt match_set(const xuByte * sp, const SetArgs * args);
xuInt match_count(const xuByte * sp, const CountArgs * args);
xuInt match_call(const xuByte * sp, const CallArgs * args);
xuInt match_group(const xuByte * sp, const GroupArgs * args);

#endif //X_RUNTIME_H

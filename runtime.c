//
// Created by Yaokai Liu on 2023/4/7.
//

#include "runtime.h"

inline xuInt match_obj(const xuByte * sp, const ArgsHeader * args) {
    switch (args->obj_type) {
        case GRP:
            return match_group(sp, (const GroupArgs *) args);
        case SEQ:
            return match_seq(sp, (const SeqArgs *) args);
        case SET:
            return match_set(sp, (const SetArgs *) args);
        case CNT:
            return match_count(sp, (const CountArgs *) args);
        case CAL:
            return match_call(sp, (const CallArgs *) args);
        default:
            return 0;
    }
}

xuInt match_seq(const xuByte * sp, const SeqArgs * args) {
    for(xuInt i = 0; i < args->len; i++ ) {
        xBool p = (sp[i] == args->pattern[i]);
        if (!p && !args->attr.inverse) {
            return 0;
        }
    }
    return args->len;
}

xuInt match_set(const xuByte * sp, const SetArgs * args) {
    for(xuInt i = 0; i < args->n_plains; i++ ) {
        xBool p = sp[i] == args->plains[i];
        if (p && !args->attr.inverse) {
            return 1;
        }
    }
    for (xuInt i = 0; i < args->n_ranges; i ++) {
        xBool p = args->ranges[i].left <= *sp
                && *sp <= args->ranges[i].right;
        if (p && !args->attr.inverse) {
            return 1;
        }
    }
    return args->attr.inverse;
}

xuInt match_count(const xuByte * sp, const CountArgs * args) {
    xuInt len = 0;
    for (xuInt i = 0; i < args->max; i += args->step) {
        for (xuInt j = 0; j < args->step; j++) {
            xuInt a =  match_obj(sp + len, args->obj_args);
            if (a == 0)
                return (i < args->min) ? 0 : len;
            len += a;
        }
        if (args->attr.inverse && i > args->min) {
            break;
        }
    }
    return len;
}

xuInt match_call(const xuByte * sp, const CallArgs * args) {
    xuInt len = 0;
    if (args->target_type == last_value) {
        SeqArgs seq = {.attr.obj_type = SEQ, .attr.inverse = args->attr.inverse,
                       .pattern = args->last_val, .len = args->last_len};
        return match_seq(sp, &seq);
    } else if (args->target_type == callee) {
        args->obj_args->inverse = args->attr.inverse;
        return match_obj(sp, args->obj_args);
    }
    return len;
}

xuInt match_group(const xuByte * sp, const GroupArgs * args) {
    xuInt len = 0;
    for (int j = 0; j < args->n_branch; j++){
        BranchArgs brh = args->branch_list[j];
        for (xuInt i = 0; i < brh.n_objects; i++) {
            xuInt a = match_obj(sp + len, brh.obj_list[i]);
            if (a == 0) {
                len = 0; break;
            }
            len += a;
        }
        if (len > 0) {
            return len;
        }
    }
    return 0;
}

//
// Created by Yaokai Liu on 2023/3/25.
//

#ifndef X_MATCH_H
#define X_MATCH_H

#include "xregexp.h"

xInt xReProcessor_match_o(ReObj * obj, const xReChar * source, xInt * length);
xInt xReProcessor_match_c(const xReChar * pattern, const xReChar * source, xInt * length);

#endif //X_MATCH_H

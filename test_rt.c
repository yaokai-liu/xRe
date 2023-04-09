//
// Created by Yaokai Liu on 2023/4/7.
//

#include "runtime.h"
#include "string.h"
#include "stdio.h"
#include "err.h"

int main() {
//    unsigned char hello[] = "Hello";
//    SeqArgs seq = {.attr = {.obj_type = SEQ, .inverse = false},
//                   .len = 5, .pattern = hello};
//    unsigned char string[] = "Hello world!";
//    return (int) match_seq(string, &seq);

    xReChar err_log[512];
    xReChar actual[] = {xReChar('\''), 'z', xReChar('\''), xReChar('\0')};
    xReChar * args[] = {xReString("'('"), actual};
    strfmt(err_log, SYNTAX_ERROR_FMT, args);
    printf("%s\n", err_log);
    return 0;
}

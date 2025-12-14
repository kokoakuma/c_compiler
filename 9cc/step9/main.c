#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "9cc.h"

Token *token;
char *user_input;
Node *code [100];

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int pos = loc - user_input;

    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // pos個分のスペース
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        error("%s: Invalid number of arguments", argv[0]);
        return 1;
    }
    user_input = argv[1];
    token = tokenize();
    program();

    printf(".globl main\n");
    printf("main:\n");

    // prologue
    printf("    stp x29, x30, [sp, -16]!\n");
    printf("    mov x29, sp\n");
    printf("    sub sp, sp, #416\n");

    // 先頭の式から順にコード生成
    for (int i = 0; code[i]; i++) {
        gen(code[i]);

        // 式の評価結果としてスタックに一つの値が残っている
        // はずなので、スタックが溢れないようにポップしておく
        printf("    ldr x0, [sp], #16\n");
    }

    printf("    mov sp, x29\n");
    printf("    ldp x29, x30, [sp], #16\n");
    printf("    ret\n");
    return 0;
}

/**
 * expr = equality
 * equality = relational ("==" relational | "!= " relational)*
 * relational = add ("<" add | "<=" add | ">" add | ">=" add)*
 * add = mul ( "+" mul | "-" mul)*
 * mul = unary ("*" unary | "/" unary)*
 * unary = ("+" | "-")? primary
 * primary = num | "(" expr ")"
 */
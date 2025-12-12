#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "9cc.h"

Token *token;
char *user_input;

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
    Node *node = expr();

    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("    ldr x0, [sp], #16\n");
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
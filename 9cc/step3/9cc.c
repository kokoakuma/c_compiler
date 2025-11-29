#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
    TK_RESERVED, // 記号
    TK_NUM, // 整数
    TK_EOF, // 入力の終わり
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token *next; // next token pointer
    int val; //  number if kind is num
    char *str; // token string
    /* data */
};

Token *token;

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error("Token does not match '%c'.", op);
    token = token->next;
}


int expect_number() {
    if (token->kind != TK_NUM)
        error("This is not number.");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10); // &pには読み込み完了後のポインタが入る
            continue;
        }

        error("This can not be tokenized properly.");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        error("The count of args is wrong.");
        return 1;
    }

    token = tokenize(argv[1]);

    printf(".globl main\n");
    printf("main:\n");
    printf("    mov x0, %d\n", expect_number());

    while (!at_eof()) {
        if (consume('+')) {
            printf("    add x0, x0, %d\n", expect_number());
            continue;
        }
        expect('-');
        printf("    sub x0, x0, %d\n", expect_number());
    }
    printf("    ret\n");
    return 0;
}

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

bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "Token does not match '%c'.", op);
    token = token->next;
}


int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "This is not number.");
    int val = token->val;
    token = token->next;
    return val;
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10); // &pには読み込み完了後のポインタが入る
            continue;
        }

        error_at(p, "Expected a number.");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}


typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *expr();
Node *mul();
Node *primary();

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *primary() {
    if (consume('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }

    return new_node_num(expect_number());
}

Node *unary() {
    if (consume('+')) {
        return primary();
    }
    // treat - as 0-x
    if (consume('-')) {
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume('*'))
            node = new_node(ND_MUL, node, unary());
        else if (consume('/'))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *expr() {
    Node *node = mul();

    for (;;) {
        if (consume('+'))
            node = new_node(ND_ADD, node, mul());
        else if (consume('-'))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

void gen(Node *node) {
    if (node->kind == ND_NUM) {
        // push
        printf("    mov x0, #%d\n", node->val);
        // ARMではpush = アドレスを前に進める。16Byteごとに扱うので、strと同時に16Byte前に移動している
        printf("    str x0, [sp, #-16]!\n");
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    // stackから取得、n1,n2の順で入れたので、取り出す時はx1,x0
    printf("    ldr x1, [sp], #16\n");
    printf("    ldr x0, [sp], #16\n");

    switch (node->kind)
    {
    case ND_ADD:
        printf("    add x0, x0, x1\n");
        break;
    case ND_SUB:
        printf("    sub x0, x0, x1\n");
        break;
    case ND_MUL:
        printf("    mul x0, x0, x1\n");
        break;
    case ND_DIV:
        printf("    sdiv x0, x0, x1\n");
        break;
    }

    printf("    str x0, [sp, #-16]!\n");
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

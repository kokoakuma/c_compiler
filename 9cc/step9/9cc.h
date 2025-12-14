typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT,
    TK_NUM, // 整数
    TK_EOF, // 入力の終わり
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next; // next token pointer
    int val; //  number if kind is num
    char *str; // token string
    int len;
    /* data */
};

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQ, // ==
    ND_NE, // !=
    ND_LT, // <
    ND_LE, // <=
    ND_ASSIGN, // = 代入
    ND_LVAR, // ローカル変数
    ND_NUM, // 変数
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val; // ND_NUMの時に使用する
    int offset;// ND_LVARの時に使用する
    // ローカル変数のベースポインタからのオフセットを表す
};

bool consume(char *op);
void expect(char *op);
int expect_number();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *consume_ident();
bool startswith(char *p, char *q);
Token *tokenize();
bool at_eof();

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *assign();
Node *expr();
Node *stmt();
void program();

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
void gen(Node *node);

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);

extern Token *token;
extern char *user_input;
extern Node *code [100];
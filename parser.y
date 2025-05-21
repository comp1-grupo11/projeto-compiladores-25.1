%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern char* yytext;
extern int yylineno;

void yyerror(const char *s);

%}

%union {
    char* str_val;
    int int_val;
    float float_val;
    char* code;
}

/* Tokens */
%token <str_val> PP_INCLUDE PP_DEFINE IDENTIFIER STRING_LITERAL CHAR_LITERAL
%token <int_val> INT_LITERAL
%token <float_val> FLOAT_LITERAL

/* Keywords */
%token KW_AUTO KW_BREAK KW_CASE KW_CHAR KW_CONST
%token KW_CONTINUE KW_DEFAULT KW_DO KW_ELSE KW_ENUM
%token KW_EXTERN KW_FOR KW_IF KW_INT KW_LONG
%token KW_RETURN KW_SHORT KW_SIGNED KW_STATIC KW_SWITCH
%token KW_TYPEDEF KW_UNSIGNED KW_VOID KW_VOLATILE KW_WHILE

/* Types */
%token TYPE_INT TYPE_CHAR TYPE_FLOAT TYPE_DOUBLE TYPE_VOID

/* Operators */
%token OP_ADD OP_SUB OP_MUL OP_DIV OP_ASSIGN
%token OP_EQ OP_NE OP_LT OP_LE OP_GT OP_GE
%token OP_AND OP_OR OP_NOT OP_INC OP_DEC
%token OP_ADD_ASSIGN OP_SUB_ASSIGN OP_MUL_ASSIGN OP_DIV_ASSIGN

/* Punctuation */
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token SEMICOLON COMMA DOT COLON OP_TERNARY

/* Precedence */
%left OP_OR
%left OP_AND
%left OP_EQ OP_NE
%left OP_LT OP_LE OP_GT OP_GE
%left OP_ADD OP_SUB
%left OP_MUL OP_DIV
%right OP_NOT
%right OP_ASSIGN

%type <code> program function function_list params param_list param_decl compound_stmt stmt_list stmt expr type_specifier preprocessor preprocessor_list
%type <code> for_init for_cond for_iter
%type <code> switch_body case_list case_stmt default_case
%type <code> var_decl args expr_list const_expr

%%

program:
    preprocessor_list function_list {
        FILE *tsout = fopen("output.ts", "w");
        if (tsout != NULL) {
            fprintf(tsout, "%s\n", $2);
            fclose(tsout);
        } else {
            fprintf(stderr, "Erro ao criar arquivo output.ts\n");
        }
    }
;

preprocessor_list:
    /* empty */ { $$ = strdup(""); }
    | preprocessor_list preprocessor {
        asprintf(&$$, "%s\n%s", $1, $2);
    }
;

preprocessor:
    PP_INCLUDE STRING_LITERAL {
        asprintf(&$$, "// include %s", $2);
    }
    | PP_DEFINE IDENTIFIER expr {
        asprintf(&$$, "// define %s %s", $2, $3);
    }
;

function_list:
    function
    | function_list function
;

function:
    type_specifier IDENTIFIER LPAREN params RPAREN compound_stmt {
        asprintf(&$$, "function %s(%s): %s %s", $2, $4, $1, $6);
    }
    | type_specifier IDENTIFIER LPAREN RPAREN compound_stmt {
        asprintf(&$$, "function %s(): %s %s", $2, $1, $5);
    }
;

type_specifier:
    TYPE_INT { $$ = strdup("number"); }
    | TYPE_CHAR { $$ = strdup("string"); }
    | TYPE_FLOAT { $$ = strdup("number"); }
    | TYPE_DOUBLE { $$ = strdup("number"); }
    | TYPE_VOID { $$ = strdup("void"); }
;

params:
    param_list { $$ = $1; }
    | /* empty */ { $$ = strdup(""); }
;

param_list:
    param_decl { $$ = $1; }
    | param_list COMMA param_decl {
        asprintf(&$$, "%s, %s", $1, $3);
    }
;

param_decl:
    type_specifier IDENTIFIER {
        asprintf(&$$, "%s: %s", $2, $1);
    }
;

compound_stmt:
    LBRACE stmt_list RBRACE {
        asprintf(&$$, "{\n%s\n}", $2);
    }
;

stmt_list:
    /* empty */ { $$ = strdup(""); }
    | stmt_list stmt {
        asprintf(&$$, "%s\n%s", $1, $2);
    }
;

stmt:
    expr SEMICOLON {
        asprintf(&$$, "%s;", $1);
    }
    | KW_RETURN expr SEMICOLON {
        asprintf(&$$, "return %s;", $2);
    }
    | KW_IF LPAREN expr RPAREN stmt {
        asprintf(&$$, "if (%s) %s", $3, $5);
    }
    | KW_IF LPAREN expr RPAREN stmt KW_ELSE stmt {
        asprintf(&$$, "if (%s) %s else %s", $3, $5, $7);
    }
    | KW_WHILE LPAREN expr RPAREN stmt {
        asprintf(&$$, "while (%s) %s", $3, $5);
    }
    | KW_FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_iter RPAREN stmt {
        asprintf(&$$, "for (%s; %s; %s) %s", $3, $5, $7, $9);
    }
    | KW_SWITCH LPAREN expr RPAREN switch_body {
        asprintf(&$$, "switch (%s) %s", $3, $5);
    }
    | compound_stmt {
        $$ = $1;
    }
    | KW_BREAK SEMICOLON {
        $$ = strdup("break;");
    }
    | KW_CONTINUE SEMICOLON {
        $$ = strdup("continue;");
    }
    | type_specifier IDENTIFIER {
        asprintf(&$$, "let %s: %s;", $2, $1);
    }
    | type_specifier IDENTIFIER OP_ASSIGN expr {
        asprintf(&$$, "let %s: %s = %s;", $2, $1, $4);
    }
;

for_init:
    /* empty */ { $$ = strdup(""); }
    | var_decl
    | expr
;

for_cond:
    /* empty */ { $$ = strdup(""); }
    | expr
;

for_iter:
    /* empty */ { $$ = strdup(""); }
    | expr
;

switch_body:
    LBRACE case_list default_case RBRACE {
        asprintf(&$$, "{\n%s\n%s\n}", $2, $3);
    }
    | LBRACE case_list RBRACE {
        asprintf(&$$, "{\n%s\n}", $2);
    }
;

case_list:
    /* empty */ { $$ = strdup(""); }
    | case_list case_stmt {
        asprintf(&$$, "%s\n%s", $1, $2);
    }
;

case_stmt:
    KW_CASE const_expr COLON stmt_list {
        asprintf(&$$, "case %s:\n%s", $2, $4);
    }
;

default_case:
    /* empty */ { $$ = strdup(""); }
    | KW_DEFAULT COLON stmt_list {
        asprintf(&$$, "default:\n%s", $3);
    }
;

const_expr:
    INT_LITERAL { asprintf(&$$, "%d", $1); }
    | CHAR_LITERAL { asprintf(&$$, "'%s'", yytext); }
;

var_decl:
    type_specifier IDENTIFIER
    | type_specifier IDENTIFIER OP_ASSIGN expr
;

args:
    /* empty */ { $$ = strdup(""); }
    | expr_list
;

expr_list:
    expr { $$ = $1; }
    | expr_list COMMA expr {
        asprintf(&$$, "%s, %s", $1, $3);
    }
;

expr:
    INT_LITERAL {
        asprintf(&$$, "%d", $1);
    }
    | FLOAT_LITERAL {
        asprintf(&$$, "%f", $1);
    }
    | CHAR_LITERAL {
        asprintf(&$$, "'%s'", yytext);
    }
    | STRING_LITERAL {
        asprintf(&$$, "\"%s\"", $1);
    }
    | IDENTIFIER {
        $$ = strdup($1);
    }
    | IDENTIFIER LPAREN args RPAREN {
        asprintf(&$$, "%s(%s)", $1, $3);
    }
    | IDENTIFIER OP_ASSIGN expr {
        asprintf(&$$, "%s = %s", $1, $3);
    }
    | IDENTIFIER OP_INC {
        asprintf(&$$, "%s++", $1);
    }
    | IDENTIFIER OP_DEC {
        asprintf(&$$, "%s--", $1);
    }
    | IDENTIFIER OP_ADD_ASSIGN expr {
        asprintf(&$$, "%s += %s", $1, $3);
    }
    | IDENTIFIER OP_SUB_ASSIGN expr {
        asprintf(&$$, "%s -= %s", $1, $3);
    }
    | IDENTIFIER OP_MUL_ASSIGN expr {
        asprintf(&$$, "%s *= %s", $1, $3);
    }
    | IDENTIFIER OP_DIV_ASSIGN expr {
        asprintf(&$$, "%s /= %s", $1, $3);
    }
    | expr OP_ADD expr {
        asprintf(&$$, "(%s + %s)", $1, $3);
    }
    | expr OP_SUB expr {
        asprintf(&$$, "(%s - %s)", $1, $3);
    }
    | expr OP_MUL expr {
        asprintf(&$$, "(%s * %s)", $1, $3);
    }
    | expr OP_DIV expr {
        asprintf(&$$, "(%s / %s)", $1, $3);
    }
    | expr OP_EQ expr {
        asprintf(&$$, "(%s === %s)", $1, $3);
    }
    | expr OP_NE expr {
        asprintf(&$$, "(%s !== %s)", $1, $3);
    }
    | expr OP_LT expr {
        asprintf(&$$, "(%s < %s)", $1, $3);
    }
    | expr OP_LE expr {
        asprintf(&$$, "(%s <= %s)", $1, $3);
    }
    | expr OP_GT expr {
        asprintf(&$$, "(%s > %s)", $1, $3);
    }
    | expr OP_GE expr {
        asprintf(&$$, "(%s >= %s)", $1, $3);
    }
    | OP_NOT expr {
        asprintf(&$$, "!(%s)", $2);
    }
    | LPAREN expr RPAREN {
        asprintf(&$$, "(%s)", $2);
    }
    | expr OP_TERNARY expr COLON expr {
        asprintf(&$$, "(%s ? %s : %s)", $1, $3, $5);
    }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático linha %d: %s\nToken: '%s'\n", 
            yylineno, s, yytext);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Erro ao abrir arquivo");
            return 1;
        }
    }
    
    yyparse();
    
    return 0;
}
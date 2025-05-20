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

%error-verbose

%union {
    char* str_val;
    int int_val;
    float float_val;
}

/* Tokens */
%token PP_INCLUDE PP_DEFINE
%token <str_val> IDENTIFIER STRING_LITERAL CHAR_LITERAL
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
%right OP_ASSIGN OP_ADD_ASSIGN OP_SUB_ASSIGN OP_MUL_ASSIGN OP_DIV_ASSIGN
%left OP_OR
%left OP_AND
%left OP_EQ OP_NE
%left OP_LT OP_LE OP_GT OP_GE
%left OP_ADD OP_SUB
%left OP_MUL OP_DIV
%right OP_NOT OP_INC OP_DEC
%left POSTFIX_LEVEL

%%

program:
    preprocessor_list function_list
;

preprocessor_list:
    /* empty */
    | preprocessor_list preprocessor
;

preprocessor:
    PP_INCLUDE
    | PP_DEFINE
;

function_list:
    function
    | function_list function
;

function:
    type_specifier IDENTIFIER LPAREN params RPAREN compound_stmt
    | type_specifier IDENTIFIER LPAREN RPAREN compound_stmt
;

type_specifier:
    TYPE_INT
    | TYPE_CHAR
    | TYPE_FLOAT
    | TYPE_DOUBLE
    | TYPE_VOID
;

params:
    param_list
    | /* empty */
;

param_list:
    param_decl
    | param_list COMMA param_decl
;

param_decl:
    type_specifier IDENTIFIER
;

compound_stmt:
    LBRACE stmt_list RBRACE
;

stmt_list:
    /* empty */
    | stmt_list stmt
;

stmt:
    expr SEMICOLON
    | KW_RETURN expr SEMICOLON
    | KW_IF LPAREN expr RPAREN stmt
    | KW_IF LPAREN expr RPAREN stmt KW_ELSE stmt
    | KW_WHILE LPAREN expr RPAREN stmt
    | KW_FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_iter RPAREN stmt
    | KW_SWITCH LPAREN expr RPAREN switch_body
    | compound_stmt
    | var_decl SEMICOLON
    | KW_BREAK SEMICOLON
    | KW_CONTINUE SEMICOLON
;

for_init:
    /* empty */
    | var_decl
    | expr
;

for_cond:
    /* empty */
    | expr
;

for_iter:
    /* empty */
    | expr_list
;

switch_body:
    LBRACE case_list default_case RBRACE
    | LBRACE case_list RBRACE
;

case_list:
    /* empty */
    | case_list case_stmt
;

case_stmt:
    KW_CASE const_expr COLON stmt_list
;

default_case:
    /* empty */
    | KW_DEFAULT COLON stmt_list
;

const_expr:
    INT_LITERAL
    | CHAR_LITERAL
;

var_decl:
    type_specifier declarator_list
;

declarator_list:
    declarator
    | declarator_list COMMA declarator
;

declarator:
    IDENTIFIER
    | IDENTIFIER OP_ASSIGN expr
;

args:
    /* empty */
    | expr_list
;

expr_list:
    expr
    | expr_list COMMA expr
;

expr:
    INT_LITERAL
    | FLOAT_LITERAL
    | CHAR_LITERAL
    | STRING_LITERAL
    | IDENTIFIER
    | IDENTIFIER LPAREN args RPAREN 
    | IDENTIFIER OP_ASSIGN expr
    | IDENTIFIER OP_INC %prec POSTFIX_LEVEL // Pós-incremento (expr++)
    | IDENTIFIER OP_DEC %prec POSTFIX_LEVEL // Pós-incremento (expr--)
    | OP_INC IDENTIFIER // Pré-incremento (++expr)
    | OP_DEC IDENTIFIER // Pré-incremento (--expr)
    | IDENTIFIER OP_ADD_ASSIGN expr
    | IDENTIFIER OP_SUB_ASSIGN expr
    | IDENTIFIER OP_MUL_ASSIGN expr
    | IDENTIFIER OP_DIV_ASSIGN expr
    | expr OP_ADD expr
    | expr OP_SUB expr
    | expr OP_MUL expr
    | expr OP_DIV expr
    | expr OP_EQ expr
    | expr OP_NE expr
    | expr OP_LT expr
    | expr OP_LE expr
    | expr OP_GT expr
    | expr OP_GE expr
    | expr OP_AND expr
    | expr OP_OR expr
    | OP_NOT expr
    | LPAREN expr RPAREN
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático linha %d: %s\n", yylineno, s);
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
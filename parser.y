%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern char* yytext;
extern int yylineno;

void yyerror(const char *s);

Tipo current_decl_type;
%}

%define parse.error verbose

%union {
    char* str_val;
    int int_val;
    float float_val;
    double double_val;
    Tipo tipo_decl;
    NoAST *no_ast;   // Para os nós da AST
}

/* Tokens */
%token PP_INCLUDE PP_DEFINE
%token <str_val> IDENTIFIER STRING_LITERAL CHAR_LITERAL
%token <int_val> INT_LITERAL
%token <float_val> FLOAT_LITERAL
%token <double_val> DOUBLE_LITERAL

/* Keywords */
%token KW_AUTO KW_BREAK KW_CASE KW_CHAR KW_CONST
%token KW_CONTINUE KW_DEFAULT KW_DO KW_ELSE KW_ENUM
%token KW_EXTERN KW_FOR KW_IF KW_INT KW_LONG
%token KW_RETURN KW_SHORT KW_SIGNED KW_STATIC KW_SWITCH
%token KW_TYPEDEF KW_UNSIGNED KW_VOID KW_VOLATILE KW_WHILE

/* Types */
%type <tipo_decl> type_specifier
%type <no_ast> stmt var_decl expr declarator_list declarator
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
%nonassoc IF_WITHOUT_ELSE // Token fictício para precedência
%nonassoc KW_ELSE
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
;

type_specifier:
    TYPE_INT        { $$ = TIPO_INT; current_decl_type = TIPO_INT; }
    | TYPE_CHAR     { $$ = TIPO_CHAR; current_decl_type = TIPO_CHAR; }
    | TYPE_FLOAT    { $$ = TIPO_FLOAT; current_decl_type = TIPO_FLOAT; }
    | TYPE_DOUBLE   { $$ = TIPO_DOUBLE; current_decl_type = TIPO_DOUBLE; }
    | TYPE_VOID     { $$ = TIPO_VOID; current_decl_type = TIPO_VOID; }
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
    | KW_RETURN expr SEMICOLON {$$ = NULL;}
    | KW_IF LPAREN expr RPAREN stmt %prec IF_WITHOUT_ELSE {$$ = NULL;}
    | KW_IF LPAREN expr RPAREN stmt KW_ELSE stmt {$$ = NULL;}
    | KW_WHILE LPAREN expr RPAREN stmt {$$ = NULL;}
    | KW_FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_iter RPAREN stmt {$$ = NULL;}
    | KW_SWITCH LPAREN expr RPAREN switch_body {$$ = NULL;}
    | compound_stmt {$$ = NULL;}
    | var_decl SEMICOLON
    | KW_BREAK SEMICOLON {$$ = NULL;}
    | KW_CONTINUE SEMICOLON {$$ = NULL;}
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
    type_specifier declarator_list {
        $$ = $2;
    }
;

declarator_list:
    declarator
    | declarator_list COMMA declarator
;

declarator:
    IDENTIFIER {
        // $1 é o nome do IDENTIFIER
        Simbolo *s = buscarSimbolo($1);
        if (s) { // Se o símbolo já existe na tabela (e no escopo visível)
            yyerror("Redeclaração de variável.");
        } else {
            int tamanho = 0;
            switch(current_decl_type) {
                case TIPO_INT:    tamanho = 4; break;
                case TIPO_FLOAT:  tamanho = 4; break;
                case TIPO_DOUBLE: tamanho = 8; break;
                case TIPO_CHAR:   tamanho = 1; break;
                default:          tamanho = 0; break;
            }
            inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, 0, yylineno, 0, ESCOPO_LOCAL);
        }
        $$ = NULL;
    }
    | IDENTIFIER OP_ASSIGN expr {
        // $1 é o nome do IDENTIFIER, $3 é o nó da AST da expressão de inicialização
        Simbolo *s = buscarSimbolo($1);
        if (s) { // Se o símbolo já existe na tabela (e no escopo visível)
            yyerror("Redeclaração de variável.");
            $$ = NULL;
        } else {
            int tamanho = 0;
            switch(current_decl_type) {
                case TIPO_INT:    tamanho = 4; break;
                case TIPO_FLOAT:  tamanho = 4; break;
                case TIPO_DOUBLE: tamanho = 8; break;
                case TIPO_CHAR:   tamanho = 1; break;
                default:          tamanho = 0; break;
            }
            inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, 0, yylineno, 0, ESCOPO_LOCAL);

            $$ = criarNoOp('=', criarNoId($1, current_decl_type), $3);

            Simbolo *inserted_s = buscarSimbolo($1);
            if (inserted_s) {
                inserted_s->linha_ultimo_uso = yylineno;
            }
        }
    }
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
    INT_LITERAL      { int val = $1; $$ = criarNoNum(TIPO_INT, &val); }
    | FLOAT_LITERAL  { float val = $1; $$ = criarNoNum(TIPO_FLOAT, &val); }
    | DOUBLE_LITERAL { double val = $1; $$ = criarNoNum(TIPO_DOUBLE, &val); }
    | CHAR_LITERAL {$$ = NULL;}
    | STRING_LITERAL {$$ = NULL;}
    | IDENTIFIER {
        Simbolo *s = buscarSimbolo($1);
        if (!s) {
            yyerror("Identificador não declarado.");
            // int zero = 0;
            // $$ = criarNoNum(INT, &zero); // Retorna um nó numérico de erro
        } else {
            $$ = criarNoId($1, s->tipo);
            s->linha_ultimo_uso = yylineno;
        }
    }
    | IDENTIFIER LPAREN args RPAREN  {$$ = NULL;}
    | IDENTIFIER OP_ASSIGN expr {
        Simbolo *s = buscarSimbolo($1);
        if (!s) {
            yyerror("Identificador não declarado para atribuição.");
            // int zero = 0;
            // $$ = criarNoNum(INT, &zero); // Nó de erro
        } else {
            if (!tiposCompativeis(s->tipo, $3->tipo)) {
                fprintf(stderr, "Erro de tipo: Atribuição de tipo incompatível para '%s' na linha %d.\n", s->nome, yylineno);
                // Você pode retornar um nó de erro ou tentar uma conversão implícita
            }
            $$ = criarNoOp('=', criarNoId($1, s->tipo), $3);
            s->linha_ultimo_uso = yylineno;
        }
    }
    | IDENTIFIER OP_INC %prec POSTFIX_LEVEL {$$ = NULL;} // Pós-incremento (expr++)
    | IDENTIFIER OP_DEC %prec POSTFIX_LEVEL {$$ = NULL;} // Pós-incremento (expr--)
    | OP_INC IDENTIFIER {$$ = NULL;} // Pré-incremento (++expr)
    | OP_DEC IDENTIFIER  {$$ = NULL;}// Pré-incremento (--expr)
    | IDENTIFIER OP_ADD_ASSIGN expr {$$ = NULL;}
    | IDENTIFIER OP_SUB_ASSIGN expr {$$ = NULL;}
    | IDENTIFIER OP_MUL_ASSIGN expr {$$ = NULL;}
    | IDENTIFIER OP_DIV_ASSIGN expr {$$ = NULL;}
    | expr OP_ADD expr {$$ = NULL;}
    | expr OP_SUB expr {$$ = NULL;}
    | expr OP_MUL expr {$$ = NULL;}
    | expr OP_DIV expr {$$ = NULL;}
    | expr OP_EQ expr {$$ = NULL;}
    | expr OP_NE expr {$$ = NULL;}
    | expr OP_LT expr {$$ = NULL;}
    | expr OP_LE expr {$$ = NULL;}
    | expr OP_GT expr {$$ = NULL;}
    | expr OP_GE expr {$$ = NULL;}
    | expr OP_AND expr {$$ = NULL;}
    | expr OP_OR expr {$$ = NULL;}
    | OP_NOT expr {$$ = NULL;}
    | LPAREN expr RPAREN {$$ = NULL;}
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
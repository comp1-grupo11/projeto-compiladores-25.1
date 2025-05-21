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
%type <no_ast> stmt compound_stmt stmt_list var_decl declarator_list declarator expr args arg_list expr_list
%type <no_ast> for_init for_cond for_iter switch_body case_list case_stmt default_case const_expr // Os não-terminais que você usa nas regras 'for' e 'switch'
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
    LBRACE stmt_list RBRACE { $$ = criarNoCompoundStmt($2, yylineno); }
;

stmt_list:
    /* vazio */ { $$ = NULL; }
    | stmt_list stmt {
        // Encadeia statements
        NoAST *head = $1;
        if (head == NULL) {
            $$ = $2;
        } else {
            NoAST *current = head;
            while (current->proximo != NULL) {
                current = current->proximo;
            }
            current->proximo = $2;
            $$ = head;
        }
    }
;

stmt:
    expr SEMICOLON                 { $$ = $1; }
    | KW_RETURN expr SEMICOLON     { $$ = criarNoReturn($2, yylineno); }
    | KW_IF LPAREN expr RPAREN stmt %prec IF_WITHOUT_ELSE { $$ = criarNoIf($3, $5, NULL, yylineno); }
    | KW_IF LPAREN expr RPAREN stmt KW_ELSE stmt { $$ = criarNoIf($3, $5, $7, yylineno); }
    | KW_WHILE LPAREN expr RPAREN stmt { $$ = criarNoWhile($3, $5, yylineno); }
    | KW_FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_iter RPAREN stmt {
        $$ = criarNoFor($3, $5, $7, $9, yylineno);
    }
    | KW_SWITCH LPAREN expr RPAREN switch_body { $$ = criarNoSwitch($3, $5, yylineno); }
    | compound_stmt                { $$ = $1; }
    | var_decl SEMICOLON           { $$ = $1; }
    | KW_BREAK SEMICOLON           { $$ = criarNoBreak(yylineno); }
    | KW_CONTINUE SEMICOLON        { $$ = criarNoContinue(yylineno); }
;

for_init:
    /* empty */ { $$ = NULL; }
    | var_decl { $$ = $1; }
    | expr { $$ = $1; }
;

for_cond:
    /* empty */ { $$ = NULL; }
    | expr { $$ = $1; }
;

for_iter:
    /* empty */ { $$ = NULL; }
    | expr_list { $$ = $1; }
;

switch_body:
    LBRACE case_list default_case RBRACE {
        // Agora, o $2 é a cabeça da lista de cases, e o $3 é o nó do default
        $$ = criarNoSwitchBody($2, $3, yylineno);
    }
;

case_list:
    /* vazio */ { $$ = NULL; } // Pode ter um switch sem cases (apenas default)
    | case_list case_stmt {
        NoAST *head = $1;
        if (head == NULL) {
            $$ = $2;
        } else {
            NoAST *current = head;
            while (current->proximo != NULL) {
                current = current->proximo;
            }
            current->proximo = $2;
            $$ = head;
        }
    }
;

case_stmt:
    KW_CASE const_expr COLON stmt_list {
        $$ = criarNoCase($2, $4, yylineno); // $2 é a expressão do case, $4 é a lista de statements
    }
;

default_case:
    /* vazio */ { $$ = NULL; }
    | KW_DEFAULT COLON stmt_list {
        $$ = criarNoDefault($3, yylineno); // $3 é a lista de statements do default
    }
;

const_expr:
    INT_LITERAL { int val = $1; $$ = criarNoNum(TIPO_INT, &val, yylineno); }
    | CHAR_LITERAL { char val_char = $1[0]; $$ = criarNoChar(val_char, yylineno); free($1); }
    | STRING_LITERAL { $$ = criarNoString($1, yylineno); }
;

var_decl:
    type_specifier declarator_list {
        $$ = $2;
    }
;

declarator_list:
    declarator {
        $$ = $1; // O primeiro declarador é a cabeça da lista
    }
    | declarator_list COMMA declarator {
        // Encadeia o novo declarador ($3) ao final da lista ($1)
        // Percorre a lista para encontrar o último nó e anexa o novo
        NoAST *head = $1;
        if (head == NULL) { // Se a lista anterior ($1) for nula por algum erro
            $$ = $3;
        } else {
            NoAST *current = head;
            while (current->proximo != NULL) {
                current = current->proximo;
            }
            current->proximo = $3;
            $$ = head; // Retorna a cabeça original da lista
        }
    }
;

declarator:
    IDENTIFIER { // Ex: int a;
        Simbolo *s = buscarSimbolo($1);
        if (s) {
            yyerror("Redeclaração de variável.");
            $$ = criarNoErro(yylineno);
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
            $$ = criarNoDeclaracaoVar($1, current_decl_type, NULL, yylineno);
        }
    }
    | IDENTIFIER OP_ASSIGN expr { // Ex: int b = 10;
        Simbolo *s = buscarSimbolo($1);
        if (s) {
            yyerror("Redeclaração de variável.");
            $$ = criarNoErro(yylineno);
        } else {
            int tamanho = 0;
            switch(current_decl_type) {
                case TIPO_INT:    tamanho = 4; break;
                case TIPO_FLOAT:  tamanho = 4; break;
                case TIPO_DOUBLE: tamanho = 8; break;
                case TIPO_CHAR:   tamanho = 1; break;
                case TIPO_VOID:
                case TIPO_ERRO:
                default:          tamanho = 0; break;
            }
            inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, 0, yylineno, 0, ESCOPO_LOCAL);

            // Proteção contra $3 nulo ou erro
            if ($3 == NULL || $3->tipo_dado == TIPO_ERRO || !tiposCompativeis(current_decl_type, $3->tipo_dado)) {
                fprintf(stderr, "Erro de tipo: Atribuição de tipo incompatível para '%s' (tipo %d) com expressão (tipo %d) na linha %d.\n",
                        $1, current_decl_type, ($3 ? $3->tipo_dado : TIPO_ERRO), yylineno);
                $$ = criarNoErro(yylineno);
            } else {
                $$ = criarNoDeclaracaoVar($1, current_decl_type, $3, yylineno);
            }

            Simbolo *inserted_s = buscarSimbolo($1);
            if (inserted_s) {
                inserted_s->linha_ultimo_uso = yylineno;
            }
        }
    }
;

args:
    /* vazio */ { $$ = NULL; } // Chamada de função sem argumentos
    | arg_list { $$ = $1; }
;

arg_list:
    expr { $$ = $1; }
    | arg_list COMMA expr {
        // Encadeia argumentos
        NoAST *head = $1;
        if (head == NULL) {
            $$ = $3;
        } else {
            NoAST *current = head;
            while (current->proximo != NULL) {
                current = current->proximo;
            }
            current->proximo = $3;
            $$ = head;
        }
    }
;

expr_list:
    expr { $$ = $1; }
    | expr_list COMMA expr {
        NoAST *head = $1;
        if (head == NULL) {
            $$ = $3;
        } else {
            NoAST *current = head;
            while (current->proximo != NULL) {
                current = current->proximo;
            }
            current->proximo = $3;
            $$ = head;
        }
    }
;

expr:
    INT_LITERAL      { int val = $1; $$ = criarNoNum(TIPO_INT, &val, yylineno); }
    | FLOAT_LITERAL  { float val = $1; $$ = criarNoNum(TIPO_FLOAT, &val, yylineno); }
    | DOUBLE_LITERAL { double val = $1; $$ = criarNoNum(TIPO_DOUBLE, &val, yylineno); }
    | CHAR_LITERAL   { char val_char = $1[0]; $$ = criarNoChar(val_char, yylineno); free($1); }
    | STRING_LITERAL { $$ = criarNoString($1, yylineno); }
    | IDENTIFIER {
        Simbolo *s = buscarSimbolo($1);
        if (!s) {
            yyerror("Identificador não declarado.");
            $$ = criarNoErro(yylineno);
        } else {
            $$ = criarNoId($1, s->tipo, yylineno);
            s->linha_ultimo_uso = yylineno;
        }
    }
    | IDENTIFIER LPAREN args RPAREN { // Função é $1, argumentos são $3
        Simbolo *s = buscarSimbolo($1);
        if (!s || s->categoria != FUNCAO) {
            yyerror("Chamada de função para identificador não declarado ou não é função.");
            $$ = criarNoErro(yylineno);
        } else {
            $$ = criarNoChamadaFuncao($1, $3, yylineno);
        }
    }

    // Atribuição (=)
    | IDENTIFIER OP_ASSIGN expr {
        Simbolo *s = buscarSimbolo($1);
        if (!s) {
            yyerror("Identificador não declarado para atribuição.");
            $$ = criarNoErro(yylineno);
        } else {
            if ($3 == NULL || $3->tipo_dado == TIPO_ERRO || !tiposCompativeis(s->tipo, $3->tipo_dado)) {
                fprintf(stderr, "Erro de tipo: Atribuição de tipo incompatível para '%s' (tipo %d) com expressao (tipo %d) na linha %d.\n",
                        s->nome, s->tipo, ($3 ? $3->tipo_dado : TIPO_ERRO), yylineno);
                $$ = criarNoErro(yylineno);
            } else {
                $$ = criarNoOp(OP_ASSIGN_TYPE, criarNoId($1, s->tipo, yylineno), $3, yylineno);
                s->linha_ultimo_uso = yylineno;
            }
        }
    }

    // Pós-incremento/decremento
    | IDENTIFIER OP_INC %prec POSTFIX_LEVEL {
        Simbolo *s = buscarSimbolo($1);
        if (!s) { yyerror("Identificador não declarado para incremento."); $$ = criarNoErro(yylineno); }
        else { $$ = criarNoUnario(OP_INC_TYPE, criarNoId($1, s->tipo, yylineno), yylineno); }
    }
    | IDENTIFIER OP_DEC %prec POSTFIX_LEVEL {
        Simbolo *s = buscarSimbolo($1);
        if (!s) { yyerror("Identificador não declarado para decremento."); $$ = criarNoErro(yylineno); }
        else { $$ = criarNoUnario(OP_DEC_TYPE, criarNoId($1, s->tipo, yylineno), yylineno); }
    }

    // Pré-incremento/decremento
    | OP_INC IDENTIFIER {
        Simbolo *s = buscarSimbolo($2);
        if (!s) { yyerror("Identificador não declarado para incremento."); $$ = criarNoErro(yylineno); }
        else { $$ = criarNoUnario(OP_INC_TYPE, criarNoId($2, s->tipo, yylineno), yylineno); }
    }
    | OP_DEC IDENTIFIER {
        Simbolo *s = buscarSimbolo($2);
        if (!s) { yyerror("Identificador não declarado para decremento."); $$ = criarNoErro(yylineno); }
        else { $$ = criarNoUnario(OP_DEC_TYPE, criarNoId($2, s->tipo, yylineno), yylineno); }
    }

    // Atribuições compostas (+=, -=, *=, /=, etc.)
    | IDENTIFIER OP_ADD_ASSIGN expr { // a += b vira a = a + b
        Simbolo *s = buscarSimbolo($1);
        if (!s) { yyerror("Identificador não declarado para atribuição composta."); $$ = criarNoErro(yylineno); }
        else if ($3 == NULL || $3->tipo_dado == TIPO_ERRO || !tiposCompativeis(s->tipo, $3->tipo_dado)) {
            fprintf(stderr, "Erro de tipo: Atribuição composta (+=) de tipo incompatível para '%s' na linha %d.\n", $1, yylineno);
            $$ = criarNoErro(yylineno);
        } else {
            NoAST *id_node = criarNoId($1, s->tipo, yylineno);
            NoAST *soma_node = criarNoOp(OP_ADD_TYPE, criarNoId($1, s->tipo, yylineno), $3, yylineno);
            $$ = criarNoOp(OP_ASSIGN_TYPE, id_node, soma_node, yylineno);
        }
    }
    | IDENTIFIER OP_SUB_ASSIGN expr { // a -= b vira a = a - b
        Simbolo *s = buscarSimbolo($1);
        if (!s) { yyerror("Identificador não declarado para atribuição composta."); $$ = criarNoErro(yylineno); }
        else if ($3 == NULL || $3->tipo_dado == TIPO_ERRO || !tiposCompativeis(s->tipo, $3->tipo_dado)) {
            fprintf(stderr, "Erro de tipo: Atribuição composta (-=) de tipo incompatível para '%s' na linha %d.\n", $1, yylineno);
            $$ = criarNoErro(yylineno);
        } else {
            NoAST *id_node = criarNoId($1, s->tipo, yylineno);
            NoAST *sub_node = criarNoOp(OP_SUB_TYPE, criarNoId($1, s->tipo, yylineno), $3, yylineno);
            $$ = criarNoOp(OP_ASSIGN_TYPE, id_node, sub_node, yylineno);
        }
    }
    | IDENTIFIER OP_MUL_ASSIGN expr { // a *= b vira a = a * b
        Simbolo *s = buscarSimbolo($1);
        if (!s) { yyerror("Identificador não declarado para atribuição composta."); $$ = criarNoErro(yylineno); }
        else if ($3 == NULL || $3->tipo_dado == TIPO_ERRO || !tiposCompativeis(s->tipo, $3->tipo_dado)) {
            fprintf(stderr, "Erro de tipo: Atribuição composta (*=) de tipo incompatível para '%s' na linha %d.\n", $1, yylineno);
            $$ = criarNoErro(yylineno);
        } else {
            NoAST *id_node = criarNoId($1, s->tipo, yylineno);
            NoAST *mul_node = criarNoOp(OP_MUL_TYPE, criarNoId($1, s->tipo, yylineno), $3, yylineno);
            $$ = criarNoOp(OP_ASSIGN_TYPE, id_node, mul_node, yylineno);
        }
    }
    | IDENTIFIER OP_DIV_ASSIGN expr { // a /= b vira a = a / b
        Simbolo *s = buscarSimbolo($1);
        if (!s) { yyerror("Identificador não declarado para atribuição composta."); $$ = criarNoErro(yylineno); }
        else if ($3 == NULL || $3->tipo_dado == TIPO_ERRO || !tiposCompativeis(s->tipo, $3->tipo_dado)) {
            fprintf(stderr, "Erro de tipo: Atribuição composta (/=) de tipo incompatível para '%s' na linha %d.\n", $1, yylineno);
            $$ = criarNoErro(yylineno);
        } else {
            NoAST *id_node = criarNoId($1, s->tipo, yylineno);
            NoAST *div_node = criarNoOp(OP_DIV_TYPE, criarNoId($1, s->tipo, yylineno), $3, yylineno);
            $$ = criarNoOp(OP_ASSIGN_TYPE, id_node, div_node, yylineno);
        }
    }

    // Operações binárias
    | expr OP_ADD expr { $$ = criarNoOp(OP_ADD_TYPE, $1, $3, yylineno); }
    | expr OP_SUB expr { $$ = criarNoOp(OP_SUB_TYPE, $1, $3, yylineno); }
    | expr OP_MUL expr { $$ = criarNoOp(OP_MUL_TYPE, $1, $3, yylineno); }
    | expr OP_DIV expr { $$ = criarNoOp(OP_DIV_TYPE, $1, $3, yylineno); }

    // Operadores de Comparação
    | expr OP_EQ expr { $$ = criarNoOp(OP_EQ_TYPE, $1, $3, yylineno); }
    | expr OP_NE expr { $$ = criarNoOp(OP_NE_TYPE, $1, $3, yylineno); }
    | expr OP_LT expr { $$ = criarNoOp(OP_LT_TYPE, $1, $3, yylineno); }
    | expr OP_LE expr { $$ = criarNoOp(OP_LE_TYPE, $1, $3, yylineno); }
    | expr OP_GT expr { $$ = criarNoOp(OP_GT_TYPE, $1, $3, yylineno); }
    | expr OP_GE expr { $$ = criarNoOp(OP_GE_TYPE, $1, $3, yylineno); }

    // Operadores Lógicos
    | expr OP_AND expr { $$ = criarNoOp(OP_AND_TYPE, $1, $3, yylineno); }
    | expr OP_OR expr { $$ = criarNoOp(OP_OR_TYPE, $1, $3, yylineno); }
    | OP_NOT expr { $$ = criarNoUnario(OP_NOT_TYPE, $2, yylineno); } // Negação unária
    | LPAREN expr RPAREN                    {$$ = $2;} // Propaga o nó da expressão dentro dos parênteses
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático linha %d: %s\n", yylineno, s);
}

void inicializarTabelaSimbolosGlobais() {
    // Insere printf na tabela de símbolos.
    // O tipo de retorno de printf é int.
    // A categoria é FUNCAO.
    // O num_parametros pode ser -1 ou um valor especial para indicar função variádica.
    inserirSimbolo("printf", TIPO_INT, FUNCAO, 0, -1, 0, 0, ESCOPO_GLOBAL); // Tamanho 0, endereço 0, linha 0 para globais
    // Adicione outras funções da biblioteca padrão que você precisa
    inserirSimbolo("scanf", TIPO_INT, FUNCAO, 0, -1, 0, 0, ESCOPO_GLOBAL);
    // ...
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Erro ao abrir arquivo");
            return 1;
        }
    }
    inicializarTabelaSimbolosGlobais();

    yyparse();
    
    return 0;
}
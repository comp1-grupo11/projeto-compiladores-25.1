%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "ast.h"
#include "tabela.h"

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern char* yytext;
extern int yylineno;

void yyerror(const char *s);
void criarEscopoLocal(void);
void destruirEscopoLocal(void);
void gerarTypeScript(NoAST *no, FILE *saida);

Tipo current_decl_type;

int temErroSemantico = 0;

void gerarCodigoTs(NoAST *no, int indent);
NoAST *raiz_programa;
NoAST *raiz_ast = NULL;
%}

// %define parse.error verbose

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
%token KW_RETURN KW_SHORT KW_SIGNED KW_STATIC KW_SWITCH KW_STRUCT
%token KW_TYPEDEF KW_UNSIGNED KW_VOID KW_VOLATILE KW_WHILE

/* Types */
%type <tipo_decl> type_specifier
%type <no_ast> program toplevel_list toplevel function params param_list param_decl stmt compound_stmt stmt_list
%type <no_ast> var_decl declarator_list declarator expr args arg_list expr_list
%type <no_ast> for_init for_cond for_iter switch_body case_list case_stmt default_case const_expr
%type <no_ast> field_assign_list field_assign
%type <no_ast> iteration_stmt
%token TYPE_INT TYPE_CHAR TYPE_FLOAT TYPE_DOUBLE TYPE_VOID

/* Operators */
%token OP_ADD OP_SUB OP_MUL OP_DIV OP_ASSIGN OP_MOD
%token OP_EQ OP_NE OP_LT OP_LE OP_GT OP_GE
%token OP_AND OP_OR OP_NOT OP_INC OP_DEC
%token OP_ADD_ASSIGN OP_SUB_ASSIGN OP_MUL_ASSIGN OP_DIV_ASSIGN

/* Punctuation */
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token SEMICOLON COMMA DOT COLON OP_TERNARY

/* Precedence */
%nonassoc IF_WITHOUT_ELSE
%nonassoc KW_ELSE
%right OP_ASSIGN OP_ADD_ASSIGN OP_SUB_ASSIGN OP_MUL_ASSIGN OP_DIV_ASSIGN
%left OP_OR
%left OP_AND
%left OP_EQ OP_NE
%left OP_LT OP_LE OP_GT OP_GE
%left OP_ADD OP_SUB
%left OP_MUL OP_DIV OP_MOD
%right OP_NOT OP_INC OP_DEC
%left POSTFIX_LEVEL
%left DOT LBRACKET RBRACKET

%%

program:
    toplevel_list { raiz_ast = $1; }
;

toplevel_list:
      /* empty */ { $$ = NULL; }
    | toplevel_list toplevel {
        if ($1 == NULL) $$ = $2;
        else {
            NoAST *current = $1;
            while (current->proximo) current = current->proximo;
            current->proximo = $2;
            $$ = $1;
        }
    }
;

toplevel:
      preprocessor { $$ = NULL; }
    | function { $$ = $1; }
    | var_decl SEMICOLON { $$ = $1; }
    | struct_decl { $$ = NULL; }
;

struct_decl:
    KW_STRUCT IDENTIFIER LBRACE struct_field_list RBRACE SEMICOLON
;

struct_field_list:
      struct_field
    | struct_field_list struct_field
;

struct_field:
    type_specifier declarator_list SEMICOLON
;

preprocessor:
    PP_INCLUDE
    | PP_DEFINE
;

function:
    type_specifier IDENTIFIER LPAREN params RPAREN LBRACE stmt_list RBRACE
    {
        inserirSimbolo($2, $1, FUNCAO, 0, -1, yylineno, 0, ESCOPO_GLOBAL);
        criarEscopoLocal();
        destruirEscopoLocal();
        $$ = criarNoFuncao($2, $1, $4, $7);
        /* Garante que todos os parâmetros estejam ligados em $$->esquerda */
        $$->esquerda = $4;
    }
;

type_specifier:
    TYPE_INT        { $$ = TIPO_INT; current_decl_type = TIPO_INT; }
    | TYPE_CHAR     { $$ = TIPO_CHAR; current_decl_type = TIPO_CHAR; }
    | TYPE_FLOAT    { $$ = TIPO_FLOAT; current_decl_type = TIPO_FLOAT; }
    | TYPE_DOUBLE   { $$ = TIPO_DOUBLE; current_decl_type = TIPO_DOUBLE; }
    | TYPE_VOID     { $$ = TIPO_VOID; current_decl_type = TIPO_VOID; }
    | KW_STRUCT IDENTIFIER { $$ = TIPO_OBJETO; current_decl_type = TIPO_OBJETO; }
    | KW_CONST type_specifier { $$ = $2; }
    | KW_SIGNED type_specifier { $$ = $2; }
    | KW_UNSIGNED type_specifier { $$ = $2; }
    | KW_SHORT type_specifier { $$ = $2; }
    | KW_LONG type_specifier { $$ = $2; }
    | KW_STATIC type_specifier { $$ = $2; }
    | KW_VOLATILE type_specifier { $$ = $2; }
;

params:
    param_list { $$ = $1; }
    | /* empty */ { $$ = NULL; }
;

param_list:
    param_decl
    | param_list COMMA param_decl
        {
            NoAST *atual = $1;
            if (atual == NULL) {
                $$ = $3;
            } else {
                while (atual->proximo)
                    atual = atual->proximo;
                atual->proximo = $3;
                $$ = $1;
            }
        }
;

param_decl:
    type_specifier IDENTIFIER {
        inserirSimbolo($2, $1, PARAMETRO, 0, 0, yylineno, 0, ESCOPO_LOCAL);
        $$ = criarNoDeclaracaoVar($2, $1, NULL);
    }
;

compound_stmt:
    LBRACE { criarEscopoLocal(); } stmt_list RBRACE { destruirEscopoLocal(); $$ = criarNoCompoundStmt($3); }
;

stmt_list:
    /* vazio */ { $$ = NULL; }
    | stmt_list stmt {
        if ($2 == NULL) {
            $$ = $1;
        } else if ($1 == NULL) {
            $$ = $2;
        } else {
            NoAST *current = $1;
            while (current->proximo != NULL) {
                current = current->proximo;
            }
            current->proximo = $2;
            $$ = $1;
        }
    }
;

stmt:
    expr SEMICOLON                 { $$ = $1; }
    | KW_RETURN expr SEMICOLON     { $$ = criarNoReturn($2); }
    | KW_IF LPAREN expr RPAREN stmt %prec IF_WITHOUT_ELSE { $$ = criarNoIf($3, $5, NULL); }
    | KW_IF LPAREN expr RPAREN stmt KW_ELSE stmt { $$ = criarNoIf($3, $5, $7); }
    | KW_WHILE LPAREN expr RPAREN stmt { $$ = criarNoWhile($3, $5); }
    | iteration_stmt               { $$ = $1; }
    | KW_SWITCH LPAREN expr RPAREN switch_body { $$ = criarNoSwitch($3, $5); }
    | compound_stmt                { $$ = $1; }
    | var_decl SEMICOLON           { $$ = $1; }
    | KW_BREAK SEMICOLON           { $$ = criarNoBreak(); }
    | KW_CONTINUE SEMICOLON        { $$ = criarNoContinue(); }

iteration_stmt:
    KW_FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_iter RPAREN stmt {
        criarEscopoLocal();
        $$ = criarNoFor($3, $5, $7, $9);
        destruirEscopoLocal();
    }
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
        $$ = criarNoSwitchBody($2, $3);
    }
;

case_list:
    /* vazio */ { $$ = NULL; }
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
        $$ = criarNoCase($2, $4);
    }
;

field_assign_list:
      field_assign
    | field_assign_list COMMA field_assign
;

field_assign:
      DOT IDENTIFIER OP_ASSIGN expr {
        $$ = criarNoAtribuicaoCampo(NULL, $2, $4);
    }
;

default_case:
    /* vazio */ { $$ = NULL; }
    | KW_DEFAULT COLON stmt_list {
        $$ = criarNoDefault($3);
    }
;

const_expr:
    INT_LITERAL { int val = $1; $$ = criarNoNum(TIPO_INT, &val); }
    | CHAR_LITERAL { char val_char = $1[0]; $$ = criarNoChar(val_char); free($1); }
    | STRING_LITERAL { $$ = criarNoString($1); }
;

var_decl:
    type_specifier declarator_list {
        $$ = $2;
    }
;

declarator_list:
    declarator {
        $$ = $1;
    }
    | declarator_list COMMA declarator {
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

declarator:
    IDENTIFIER {
        Simbolo *s = buscarSimboloNoEscopoAtual($1);
        if (s) {
            yyerror("Redeclaração de variável.");
            $$ = criarNoErro();
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
            $$ = criarNoDeclaracaoVar($1, current_decl_type, NULL);
        }
    }
    | IDENTIFIER OP_ASSIGN expr {
        Simbolo *s = buscarSimboloNoEscopoAtual($1);
        if (s) {
            yyerror("Redeclaração de variável.");
            $$ = criarNoErro();
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

            if ($3 == NULL || $3->tipo_dado == TIPO_ERRO || !tiposCompativeis(current_decl_type, $3->tipo_dado)) {
                fprintf(stderr,
                        "Erro de tipo: Atribuição de tipo incompatível para '%s' (tipo %s) com expressão (tipo %s) na linha %d.\n",
                        $1,
                        nomeTipo(current_decl_type),
                        nomeTipo($3 ? $3->tipo_dado : TIPO_ERRO),
                        yylineno);
                $$ = criarNoErro();
            } else {
                $$ = criarNoDeclaracaoVar($1, current_decl_type, $3);
            }

            Simbolo *inserted_s = buscarSimbolo($1);
            if (inserted_s) {
                inserted_s->linha_ultimo_uso = yylineno;
            }
        }
    }
    | IDENTIFIER LBRACKET expr RBRACKET {
        Simbolo *s = buscarSimboloNoEscopoAtual($1);
        if (s) {
            yyerror("Redeclaração de variável.");
            $$ = criarNoErro();
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
            $$ = criarNoDeclaracaoVarArray($1, current_decl_type, $3);
        }
    }
    | IDENTIFIER OP_ASSIGN LBRACE field_assign_list RBRACE {
        Simbolo *s = buscarSimboloNoEscopoAtual($1);
        if (s) {
            yyerror("Redeclaração de variável.");
            $$ = criarNoErro();
        } else {
            int tamanho = 0;
            inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, 0, yylineno, 0, ESCOPO_LOCAL);
            $$ = criarNoDeclaracaoVar($1, current_decl_type, $4);
        }
    }
    | IDENTIFIER LBRACKET expr RBRACKET OP_ASSIGN LBRACE expr_list RBRACE {
        Simbolo *s = buscarSimboloNoEscopoAtual($1);
        if (s) {
            yyerror("Redeclaração de array.");
            $$ = criarNoErro();
        } else {
            int tamanho = 0;
            int tam_array = -1;
            switch(current_decl_type) {
                case TIPO_INT:    tamanho = 4; break;
                case TIPO_FLOAT:  tamanho = 4; break;
                case TIPO_DOUBLE: tamanho = 8; break;
                case TIPO_CHAR:   tamanho = 1; break;
                default:          tamanho = 0; break;
            }
            if ($3 && $3->tipo_no == NODE_LITERAL && $3->tipo_dado == TIPO_INT) {
                tam_array = $3->data.literal.val_int;
            }
            inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, tam_array, yylineno, 0, ESCOPO_LOCAL);
            $$ = criarNoDeclaracaoVarArray($1, current_decl_type, $3);
        }
    }
    | IDENTIFIER LBRACKET RBRACKET OP_ASSIGN LBRACE expr_list RBRACE {
        Simbolo *s = buscarSimboloNoEscopoAtual($1);
        if (s) {
            yyerror("Redeclaração de array.");
            $$ = criarNoErro();
        } else {
            int tamanho = 0;
            switch(current_decl_type) {
                case TIPO_INT:    tamanho = 4; break;
                case TIPO_FLOAT:  tamanho = 4; break;
                case TIPO_DOUBLE: tamanho = 8; break;
                case TIPO_CHAR:   tamanho = 1; break;
                default:          tamanho = 0; break;
            }
            inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, -1, yylineno, 0, ESCOPO_LOCAL);
            $$ = criarNoDeclaracaoVarArray($1, current_decl_type, NULL);
        }
    }
    | IDENTIFIER LBRACKET RBRACKET OP_ASSIGN STRING_LITERAL {
        Simbolo *s = buscarSimboloNoEscopoAtual($1);
        if (s) {
            yyerror("Redeclaração de array.");
            $$ = criarNoErro();
        } else {
            int tamanho = 1; // char
            inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, -1, yylineno, 0, ESCOPO_LOCAL);
            $$ = criarNoDeclaracaoVarArray($1, current_decl_type, criarNoString($5));
        }
    }
;

args:
    /* vazio */ { $$ = NULL; }
    | arg_list { $$ = $1; }
;

arg_list:
    expr { $$ = $1; }
    | arg_list COMMA expr {
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
    INT_LITERAL      { int val = $1; $$ = criarNoNum(TIPO_INT, &val); }
    | FLOAT_LITERAL  { float val = $1; $$ = criarNoNum(TIPO_FLOAT, &val); }
    | DOUBLE_LITERAL { double val = $1; $$ = criarNoNum(TIPO_DOUBLE, &val); }
    | CHAR_LITERAL   { char val_char = $1[0]; $$ = criarNoChar(val_char); free($1); }
    | STRING_LITERAL { $$ = criarNoString($1); }
    | IDENTIFIER {
        Simbolo *s = buscarSimbolo($1);
        if (!s) {
            yyerror("Identificador não declarado.");
            $$ = criarNoErro();
        } else {
            $$ = criarNoId($1, s->tipo);
            s->linha_ultimo_uso = yylineno;
        }
    }
    | IDENTIFIER LPAREN args RPAREN {
        Simbolo *s = buscarSimbolo($1);
        if (!s || s->categoria != FUNCAO) {
            yyerror("Chamada de função para identificador não declarado ou não é função.");
            $$ = criarNoErro();
        } else {
            $$ = criarNoChamadaFuncao($1, $3, s->tipo);
        }
    }
    | expr OP_ASSIGN expr {
        if ($1 == NULL || $1->tipo_no == NODE_ERROR) {
            yyerror("Atribuição para expressão inválida.");
            $$ = criarNoErro();
        } else if ($1->tipo_no != NODE_IDENTIFIER &&
                   $1->tipo_no != NODE_FIELD_ACCESS &&
                   !($1->tipo_no == NODE_OPERATOR && $1->data.op_type == OP_INDEX_TYPE)) {
            yyerror("Atribuição só é permitida para variáveis, campos ou elementos de array.");
            $$ = criarNoErro();
        } else if ($3 == NULL || $3->tipo_dado == TIPO_ERRO || !tiposCompativeis($1->tipo_dado, $3->tipo_dado)) {
            fprintf(stderr,
                    "Erro de tipo: Atribuição de tipo incompatível (tipo %s) com expressão (tipo %s) na linha %d.\n",
                    nomeTipo($1->tipo_dado),
                    nomeTipo($3 ? $3->tipo_dado : TIPO_ERRO),
                    yylineno);
            $$ = criarNoErro();
        } else {
            $$ = criarNoOp(OP_ASSIGN_TYPE, $1, $3);
        }
    }
    | expr DOT IDENTIFIER {
        $$ = criarNoAcessoCampo($1, $3);
    }
    | expr DOT IDENTIFIER OP_ASSIGN expr {
        $$ = criarNoAtribuicaoCampo($1, $3, $5);
    }
    | expr LBRACKET expr RBRACKET { $$ = criarNoOp(OP_INDEX_TYPE, $1, $3); }
    | IDENTIFIER OP_INC %prec POSTFIX_LEVEL {
        Simbolo *s = buscarSimbolo($1);
        if (!s) { yyerror("Identificador não declarado para incremento."); $$ = criarNoErro(); }
        else { $$ = criarNoUnario(OP_INC_TYPE, criarNoId($1, s->tipo)); }
    }
    | IDENTIFIER OP_DEC %prec POSTFIX_LEVEL {
        Simbolo *s = buscarSimbolo($1);
        if (!s) { yyerror("Identificador não declarado para decremento."); $$ = criarNoErro(); }
        else { $$ = criarNoUnario(OP_DEC_TYPE, criarNoId($1, s->tipo)); }
    }
    | OP_INC IDENTIFIER {
        Simbolo *s = buscarSimbolo($2);
        if (!s) { yyerror("Identificador não declarado para incremento."); $$ = criarNoErro(); }
        else { $$ = criarNoUnario(OP_INC_TYPE, criarNoId($2, s->tipo)); }
    }
    | OP_DEC IDENTIFIER {
        Simbolo *s = buscarSimbolo($2);
        if (!s) { yyerror("Identificador não declarado para decremento."); $$ = criarNoErro(); }
        else { $$ = criarNoUnario(OP_DEC_TYPE, criarNoId($2, s->tipo)); }
    }
    | IDENTIFIER OP_ADD_ASSIGN expr {
        Simbolo *s = buscarSimbolo($1);
        if (!s) { yyerror("Identificador não declarado para atribuição composta."); $$ = criarNoErro(); }
        else if ($3 == NULL || $3->tipo_dado == TIPO_ERRO || !tiposCompativeis(s->tipo, $3->tipo_dado)) {
            fprintf(stderr, "Erro de tipo: Atribuição composta (+=) de tipo incompatível para '%s' na linha %d.\n", $1, yylineno);
            $$ = criarNoErro();
        } else {
            NoAST *id_node = criarNoId($1, s->tipo);
            NoAST *soma_node = criarNoOp(OP_ADD_TYPE, criarNoId($1, s->tipo), $3);
            $$ = criarNoOp(OP_ASSIGN_TYPE, id_node, soma_node);
        }
    }
    | IDENTIFIER OP_SUB_ASSIGN expr {
        Simbolo *s = buscarSimbolo($1);
        if (!s) { yyerror("Identificador não declarado para atribuição composta."); $$ = criarNoErro(); }
        else if ($3 == NULL || $3->tipo_dado == TIPO_ERRO || !tiposCompativeis(s->tipo, $3->tipo_dado)) {
            fprintf(stderr, "Erro de tipo: Atribuição composta (-=) de tipo incompatível para '%s' na linha %d.\n", $1, yylineno);
            $$ = criarNoErro();
        } else {
            NoAST *id_node = criarNoId($1, s->tipo);
            NoAST *sub_node = criarNoOp(OP_SUB_TYPE, criarNoId($1, s->tipo), $3);
            $$ = criarNoOp(OP_ASSIGN_TYPE, id_node, sub_node);
        }
    }
    | IDENTIFIER OP_MUL_ASSIGN expr {
        Simbolo *s = buscarSimbolo($1);
        if (!s) { yyerror("Identificador não declarado para atribuição composta."); $$ = criarNoErro(); }
        else if ($3 == NULL || $3->tipo_dado == TIPO_ERRO || !tiposCompativeis(s->tipo, $3->tipo_dado)) {
            fprintf(stderr, "Erro de tipo: Atribuição composta (*=) de tipo incompatível para '%s' na linha %d.\n", $1, yylineno);
            $$ = criarNoErro();
        } else {
            NoAST *id_node = criarNoId($1, s->tipo);
            NoAST *mul_node = criarNoOp(OP_MUL_TYPE, criarNoId($1, s->tipo), $3);
            $$ = criarNoOp(OP_ASSIGN_TYPE, id_node, mul_node);
        }
    }
    | IDENTIFIER OP_DIV_ASSIGN expr {
        Simbolo *s = buscarSimbolo($1);
        if (!s) { yyerror("Identificador não declarado para atribuição composta."); $$ = criarNoErro(); }
        else if ($3 == NULL || $3->tipo_dado == TIPO_ERRO || !tiposCompativeis(s->tipo, $3->tipo_dado)) {
            fprintf(stderr, "Erro de tipo: Atribuição composta (/=) de tipo incompatível para '%s' na linha %d.\n", $1, yylineno);
            $$ = criarNoErro();
        } else {
            NoAST *id_node = criarNoId($1, s->tipo);
            NoAST *div_node = criarNoOp(OP_DIV_TYPE, criarNoId($1, s->tipo), $3);
            $$ = criarNoOp(OP_ASSIGN_TYPE, id_node, div_node);
        }
    }
    | expr OP_ADD expr { $$ = criarNoOp(OP_ADD_TYPE, $1, $3); }
    | expr OP_SUB expr { $$ = criarNoOp(OP_SUB_TYPE, $1, $3); }
    | expr OP_MUL expr { $$ = criarNoOp(OP_MUL_TYPE, $1, $3); }
    | expr OP_DIV expr { $$ = criarNoOp(OP_DIV_TYPE, $1, $3); }
    | expr OP_MOD expr { $$ = criarNoOp(OP_MOD_TYPE, $1, $3); }
    | expr OP_EQ expr { $$ = criarNoOp(OP_EQ_TYPE, $1, $3); }
    | expr OP_NE expr { $$ = criarNoOp(OP_NE_TYPE, $1, $3); }
    | expr OP_LT expr { $$ = criarNoOp(OP_LT_TYPE, $1, $3); }
    | expr OP_LE expr { $$ = criarNoOp(OP_LE_TYPE, $1, $3); }
    | expr OP_GT expr { $$ = criarNoOp(OP_GT_TYPE, $1, $3); }
    | expr OP_GE expr { $$ = criarNoOp(OP_GE_TYPE, $1, $3); }
    | expr OP_AND expr { $$ = criarNoOp(OP_AND_TYPE, $1, $3); }
    | expr OP_OR expr { $$ = criarNoOp(OP_OR_TYPE, $1, $3); }
    | OP_NOT expr { $$ = criarNoUnario(OP_NOT_TYPE, $2); }
    | LPAREN expr RPAREN { $$ = $2; }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático linha %d: %s\n", yylineno, s);
}

void inicializarTabelaSimbolosGlobais() {
    inserirSimbolo("printf", TIPO_INT, FUNCAO, 0, -1, 0, 0, ESCOPO_GLOBAL);
    inserirSimbolo("scanf", TIPO_INT, FUNCAO, 0, -1, 0, 0, ESCOPO_GLOBAL);
}

int main(int argc, char *argv[]) {
    criarEscopoLocal(); // escopo global
    inicializarTabelaSimbolosGlobais();

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_entrada.c> [arquivo_saida.ts]\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }

    yyparse();

    extern int temErroSemantico;
    if (temErroSemantico) {
        fprintf(stderr, "Código TypeScript não gerado devido a erro(s) semântico(s).\n");
        return 1;
    }

    const char *outputPath = (argc > 2) ? argv[2] : "output.ts";

    FILE *saida_ts = fopen(outputPath, "w");
    if (!saida_ts) {
        perror("Erro ao criar arquivo de saída");
        return 1;
    }

    gerarTypeScript(raiz_ast, saida_ts);
    fclose(saida_ts);

    return 0;
}
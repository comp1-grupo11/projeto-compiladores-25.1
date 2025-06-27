%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "ast.h"
#include "tabela.h"
#include "gerador_ts.h"
#include "gerador.h"

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern char* yytext;
extern int yylineno;

void yyerror(const char *s);
void semantic_error(const char *s);
void criarEscopoLocal(void);
void destruirEscopoLocal(void);
void gerarTypeScript(NoAST *no, FILE *saida, VarDecl **decls, int ident);

Tipo current_decl_type;
char current_struct_name[32] = {0};

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
  | error { YYABORT; }
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
        inserirSimbolo($2, $1, FUNCAO, 0, -1, yylineno, 0, ESCOPO_GLOBAL, NULL);
        criarEscopoLocal();
        destruirEscopoLocal();
        NoAST *bloco = criarNoCompoundStmt($7);
        $$ = criarNoFuncao($2, $1, $4, bloco);
    }
;

type_specifier:
    TYPE_INT        { $$ = TIPO_INT; current_decl_type = TIPO_INT; current_struct_name[0] = '\0'; }
    | TYPE_CHAR     { $$ = TIPO_CHAR; current_decl_type = TIPO_CHAR; current_struct_name[0] = '\0'; }
    | TYPE_FLOAT    { $$ = TIPO_FLOAT; current_decl_type = TIPO_FLOAT; current_struct_name[0] = '\0'; }
    | TYPE_DOUBLE   { $$ = TIPO_DOUBLE; current_decl_type = TIPO_DOUBLE; current_struct_name[0] = '\0'; }
    | TYPE_VOID     { $$ = TIPO_VOID; current_decl_type = TIPO_VOID; current_struct_name[0] = '\0'; }
    | KW_STRUCT IDENTIFIER { $$ = TIPO_OBJETO; current_decl_type = TIPO_OBJETO; strncpy(current_struct_name, $2, 31); current_struct_name[31] = '\0'; }
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
        inserirSimbolo($2, $1, PARAMETRO, 0, 0, yylineno, 0, ESCOPO_LOCAL, NULL);
        $$ = criarNoDeclaracaoVar($2, $1, NULL);
    }
;

compound_stmt:
    LBRACE { criarEscopoLocal(); } stmt_list RBRACE {
        destruirEscopoLocal();
        // Remover blocos que são corpo de comandos de controle
        NoAST *lista = $3;
        NoAST *atual = lista;
        while (atual) {
            if (atual->tipo_no == NODE_COMPOUND_STMT && atual->pai_controlador) {
                lista = removerNoDaLista(lista, atual);
                // Não avance o ponteiro, pois já pulou o nó
            }
            atual = atual->proximo;
        }
        $$ = criarNoCompoundStmt(lista);
    }
;

stmt_list:
    /* vazio */ { $$ = NULL; }
    | stmt_list stmt {
        if (
            $2 && $2->tipo_no == NODE_COMPOUND_STMT &&
            $2->pai_controlador
        ) {
            $$ = $1;
        } else if ($2 == NULL) {
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
    var_decl SEMICOLON           { $$ = $1; }
    | expr SEMICOLON             { $$ = $1; }
    | KW_RETURN expr SEMICOLON     { $$ = criarNoReturn($2); }
    | KW_IF LPAREN expr RPAREN stmt %prec IF_WITHOUT_ELSE { $$ = criarNoIf($3, $5, NULL); }
    | KW_IF LPAREN expr RPAREN stmt KW_ELSE stmt { $$ = criarNoIf($3, $5, $7); }
    | KW_WHILE LPAREN expr RPAREN stmt { $$ = criarNoWhile($3, $5); }
    | iteration_stmt               { $$ = $1; }
    | KW_SWITCH LPAREN expr RPAREN switch_body { $$ = criarNoSwitch($3, $5); }
    | compound_stmt                { $$ = $1; }
    | KW_BREAK SEMICOLON           { $$ = criarNoBreak(); }
    | KW_CONTINUE SEMICOLON        { $$ = criarNoContinue(); }

iteration_stmt:
    KW_FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_iter RPAREN stmt {
        criarEscopoLocal();
        NoAST *bloco = $9;
        NoAST *no_for = criarNoFor($3, $5, $7, bloco);
        if (bloco && bloco->tipo_no == NODE_COMPOUND_STMT)
            bloco->pai_controlador = no_for;
        $$ = no_for;
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
            semantic_error("Redeclaração de variável.");
            $$ = criarNoErro();
        } else {
            int tamanho = 0;
            if (current_decl_type == TIPO_OBJETO) {
                inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, 0, yylineno, 0, ESCOPO_LOCAL, current_struct_name);
            } else {
                inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, 0, yylineno, 0, ESCOPO_LOCAL, NULL);
            }
            $$ = criarNoDeclaracaoVar($1, current_decl_type, NULL);
        }
        current_struct_name[0] = '\0';
    }
    | IDENTIFIER OP_ASSIGN expr {
        Simbolo *s = buscarSimboloNoEscopoAtual($1);
        if (s) {
            semantic_error("Redeclaração de variável.");
            $$ = criarNoErro();
        } else {
            int tamanho = 0;
            if (current_decl_type == TIPO_OBJETO) {
                inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, 0, yylineno, 0, ESCOPO_LOCAL, current_struct_name);
            } else {
                inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, 0, yylineno, 0, ESCOPO_LOCAL, NULL);
            }
            $$ = criarNoDeclaracaoVar($1, current_decl_type, $3);
        }
        current_struct_name[0] = '\0';
    }
    | IDENTIFIER LBRACKET expr RBRACKET {
        Simbolo *s = buscarSimboloNoEscopoAtual($1);
        if (s) {
            semantic_error("Redeclaração de variável.");
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
            if (current_decl_type == TIPO_OBJETO) {
                inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, 0, yylineno, 0, ESCOPO_LOCAL, current_struct_name);
            } else {
                inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, 0, yylineno, 0, ESCOPO_LOCAL, NULL);
            }
            $$ = criarNoDeclaracaoVarArray($1, current_decl_type, $3);
        }
        current_struct_name[0] = '\0';
    }
    | IDENTIFIER OP_ASSIGN LBRACE field_assign_list RBRACE {
        Simbolo *s = buscarSimboloNoEscopoAtual($1);
        if (s) {
            semantic_error("Redeclaração de variável.");
            $$ = criarNoErro();
        } else {
            int tamanho = 0;
            if (current_decl_type == TIPO_OBJETO) {
                inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, 0, yylineno, 0, ESCOPO_LOCAL, current_struct_name);
            } else {
                inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, 0, yylineno, 0, ESCOPO_LOCAL, NULL);
            }
            $$ = criarNoDeclaracaoVar($1, current_decl_type, $4);
        }
        current_struct_name[0] = '\0';
    }
    | IDENTIFIER LBRACKET expr RBRACKET OP_ASSIGN LBRACE expr_list RBRACE {
        Simbolo *s = buscarSimboloNoEscopoAtual($1);
        if (s) {
            semantic_error("Redeclaração de array.");
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
            } else if ($7) {
                int contador = 0;
                NoAST *tmp = $7;
                while (tmp) { contador++; tmp = tmp->proximo; }
                tam_array = contador;
            }
            inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, tam_array, yylineno, 0, ESCOPO_LOCAL, NULL);
            NoAST *arrayLit = criarNoArrayLiteral($7);
            $$ = criarNoDeclaracaoVarArrayComInicializacao($1, current_decl_type, $3, arrayLit);
        }
    }
    | IDENTIFIER LBRACKET RBRACKET OP_ASSIGN LBRACE expr_list RBRACE {
        Simbolo *s = buscarSimboloNoEscopoAtual($1);
        if (s) {
            semantic_error("Redeclaração de array.");
            $$ = criarNoErro();
        } else {
            int tamanho = 0;
            int contador = 0;
            NoAST *tmp = $6;
            while (tmp) { contador++; tmp = tmp->proximo; }
            inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, contador, yylineno, 0, ESCOPO_LOCAL, NULL);
            NoAST *arrayLit = criarNoArrayLiteral($6);
            $$ = criarNoDeclaracaoVarArrayComInicializacao($1, current_decl_type, NULL, arrayLit);
        }
    }
    | IDENTIFIER LBRACKET RBRACKET OP_ASSIGN STRING_LITERAL {
        Simbolo *s = buscarSimboloNoEscopoAtual($1);
        if (s) {
            semantic_error("Redeclaração de array.");
            $$ = criarNoErro();
        } else {
            int tamanho = 1; // char
            inserirSimbolo($1, current_decl_type, VARIAVEL, tamanho, -1, yylineno, 0, ESCOPO_LOCAL, NULL);
            $$ = criarNoDeclaracaoVarArray($1, current_decl_type, criarNoString($5));
        }
    }
;

args:
    /* vazio */ { $$ = NULL; }
    | arg_list { $$ = $1; }
;

arg_list:
    expr {
        // Sempre copia o nó para garantir isolamento
        NoAST *copia = copiarNoAST($1);
        copia->proximo = NULL;
        $$ = copia;
    }
    | arg_list COMMA expr {
        NoAST *copia = copiarNoAST($3);
        copia->proximo = NULL;
        if ($1 == NULL) {
            $$ = copia;
        } else {
            NoAST *last = $1;
            while (last->proximo != NULL) {
                last = last->proximo;
            }
            last->proximo = copia;
            $$ = $1;
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
            semantic_error("Identificador não declarado.");
            $$ = criarNoErro();
        } else {
            $$ = criarNoId($1, s->tipo);
            s->linha_ultimo_uso = yylineno;
        }
    }
    | IDENTIFIER LPAREN args RPAREN {
        Simbolo *s = buscarSimbolo($1);
        if (!s || s->categoria != FUNCAO) {
            semantic_error("Chamada de função para identificador não declarado ou não é função.");
            $$ = criarNoErro();
        } else {
            $$ = criarNoChamadaFuncao($1, $3, s->tipo);
        }
    }
    | expr OP_ASSIGN expr {
        if ($1 == NULL || $1->tipo_no == NODE_ERROR) {
            semantic_error("Atribuição para expressão inválida.");
            $$ = criarNoErro();
        } else if ($1->tipo_no != NODE_IDENTIFIER &&
                   $1->tipo_no != NODE_FIELD_ACCESS &&
                   !($1->tipo_no == NODE_OPERATOR && $1->data.op_type == OP_INDEX_TYPE)) {
            semantic_error("Atribuição só é permitida para variáveis, campos ou elementos de array.");
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
    | expr LBRACKET expr RBRACKET { $$ = criarNoAcessoArray($1, $3); }
    | IDENTIFIER OP_INC %prec POSTFIX_LEVEL {
        Simbolo *s = buscarSimbolo($1);
        if (!s) { semantic_error("Identificador não declarado para incremento."); $$ = criarNoErro(); }
        else { $$ = criarNoUnario(OP_INC_TYPE, criarNoId($1, s->tipo)); }
    }
    | IDENTIFIER OP_DEC %prec POSTFIX_LEVEL {
        Simbolo *s = buscarSimbolo($1);
        if (!s) { semantic_error("Identificador não declarado para decremento."); $$ = criarNoErro(); }
        else { $$ = criarNoUnario(OP_DEC_TYPE, criarNoId($1, s->tipo)); }
    }
    | OP_INC IDENTIFIER {
        Simbolo *s = buscarSimbolo($2);
        if (!s) { semantic_error("Identificador não declarado para incremento."); $$ = criarNoErro(); }
        else { $$ = criarNoUnario(OP_INC_TYPE, criarNoId($2, s->tipo)); }
    }
    | OP_DEC IDENTIFIER {
        Simbolo *s = buscarSimbolo($2);
        if (!s) { semantic_error("Identificador não declarado para decremento."); $$ = criarNoErro(); }
        else { $$ = criarNoUnario(OP_DEC_TYPE, criarNoId($2, s->tipo)); }
    }
    | IDENTIFIER OP_ADD_ASSIGN expr {
        Simbolo *s = buscarSimbolo($1);
        if (!s) { semantic_error("Identificador não declarado para atribuição composta."); $$ = criarNoErro(); }
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
        if (!s) { semantic_error("Identificador não declarado para atribuição composta."); $$ = criarNoErro(); }
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
        if (!s) { semantic_error("Identificador não declarado para atribuição composta."); $$ = criarNoErro(); }
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
        if (!s) { semantic_error("Identificador não declarado para atribuição composta."); $$ = criarNoErro(); }
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
    /* Removido operador de vírgula para evitar AST errada em listas de argumentos */
;

%%


void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático linha %d: %s\n", yylineno, s);
}

void semantic_error(const char *s) {
    fprintf(stderr, "Erro semântico linha %d: %s\n", yylineno, s);
    temErroSemantico = 1;
}

void inicializarTabelaSimbolosGlobais() {
    inserirSimbolo("printf", TIPO_INT, FUNCAO, 0, -1, 0, 0, ESCOPO_GLOBAL, NULL);
    inserirSimbolo("scanf", TIPO_INT, FUNCAO, 0, -1, 0, 0, ESCOPO_GLOBAL, NULL);
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


    int parseResult = yyparse();
    extern int temErroSemantico;
    // Só gera arquivos de saída se não houve erro sintático (parseResult == 0)
    if (parseResult == 0) {
        const char *outputPath = (argc > 2) ? argv[2] : "output.ts";

        FILE *saida_ts = fopen(outputPath, "w");
        if (!saida_ts) {
            perror("Erro ao criar arquivo de saída");
            return 1;
        }

        FILE *saida_ir = fopen("intercode.ir", "w");
        if (!saida_ir) {
            perror("Erro ao criar arquivo de IR");
            fclose(saida_ts);
            return 1;
        }

        gerarIR(raiz_ast, saida_ir);
        fclose(saida_ir);

        VarDecl *decls = NULL;
        gerarTypeScript(raiz_ast, saida_ts, &decls, 0);
        fclose(saida_ts);
        return 0;
    }
    // Se houve erro sintático ou léxico, retorna 1
    return 1;
}
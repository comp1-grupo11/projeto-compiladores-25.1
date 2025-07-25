%{
extern void yyerror(const char *s);
#include "ast.h"
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char temp_str[1024];
int current_line = 1;

// Suporte a macros numéricas simples
typedef struct Macro {
    char nome[64];
    int valor;
    struct Macro *prox;
} Macro;

Macro *lista_macros = NULL;

void adicionar_macro(const char *nome, int valor) {
    Macro *m = malloc(sizeof(Macro));
    strcpy(m->nome, nome);
    m->valor = valor;
    m->prox = lista_macros;
    lista_macros = m;
}

int buscar_macro(const char *nome, int *valor) {
    for (Macro *m = lista_macros; m; m = m->prox) {
        if (strcmp(m->nome, nome) == 0) {
            *valor = m->valor;
            return 1;
        }
    }
    return 0;
}
%}

%option noyywrap
%option yylineno

%x STRING CHAR COMMENT COMMENT_MULTI PP_LINE

DIGIT       [0-9]
LETTER      [a-zA-Z_]
ID          {LETTER}({LETTER}|{DIGIT})*
HEX         [0-9a-fA-F]
OCT         [0-7]
SPACE       [ \t\r]
PP_DIR      #(include|define|ifdef|endif|ifndef|elif|else)

%%

^[ \t]*#include[ \t]*[<\"][a-zA-Z0-9_./]+[\">][ \t]*\n {
    current_line++;
    return PP_INCLUDE;
}

^[ \t]*#define[ \t]+{ID}[ \t]+{DIGIT}+.*\n {
    char nome_macro[64];
    int valor_macro;
    sscanf(yytext, "#define %63s %d", nome_macro, &valor_macro);
    adicionar_macro(nome_macro, valor_macro);
    current_line++;
    return PP_DEFINE;
}

"auto"          { return KW_AUTO; }
"break"         { return KW_BREAK; }
"case"          { return KW_CASE; }
"char"          { return TYPE_CHAR; }
"const"         { return KW_CONST; }
"continue"      { return KW_CONTINUE; }
"default"       { return KW_DEFAULT; }
"do"            { return KW_DO; }
"else"          { return KW_ELSE; }
"enum"          { return KW_ENUM; }
"extern"        { return KW_EXTERN; }
"float"         { return TYPE_FLOAT; }
"for"           { return KW_FOR; }
"if"            { return KW_IF; }
"int"           { return TYPE_INT; }
"long"          { return KW_LONG; }
"return"        { return KW_RETURN; }
"short"         { return KW_SHORT; }
"signed"        { return KW_SIGNED; }
"static"        { return KW_STATIC; }
"switch"        { return KW_SWITCH; }
"typedef"       { return KW_TYPEDEF; }
"unsigned"      { return KW_UNSIGNED; }
"void"          { return TYPE_VOID; }
"volatile"      { return KW_VOLATILE; }
"while"         { return KW_WHILE; }
"double"        { return TYPE_DOUBLE; }
"struct"        { return KW_STRUCT; }

"//"            { BEGIN(COMMENT); }
<COMMENT>\n     { BEGIN(INITIAL); current_line++; }
<COMMENT>.      { /* Ignorar */ }

"/*"            { BEGIN(COMMENT_MULTI); }
<COMMENT_MULTI>"*/" { BEGIN(INITIAL); }
<COMMENT_MULTI>\n   { current_line++; }
<COMMENT_MULTI>.    { /* Ignorar */ }

^{PP_DIR}.*\n   { current_line++; /* Ignorar outras diretivas */ }

\"              { 
    BEGIN(STRING); 
    temp_str[0] = '\0'; 
}
<STRING>{
    \"          { 
        BEGIN(INITIAL); 
        yylval.str_val = strdup(temp_str); 
        return STRING_LITERAL; 
    }
    \\n         { strcat(temp_str, "\\n"); }
    \\t         { strcat(temp_str, "\t"); }
    \\\"        { strcat(temp_str, "\""); }
    \\\\        { strcat(temp_str, "\\"); }
    [^\\\"]+    { strncat(temp_str, yytext, sizeof(temp_str)-strlen(temp_str)-1); }
    <<EOF>>     { 
        yyerror("String não terminada");
        exit(1);
    }
}

\'              { BEGIN(CHAR); }
<CHAR>{
    \'          { 
        BEGIN(INITIAL); 
        yylval.str_val = strdup(temp_str); 
        return CHAR_LITERAL; 
    }
    \\n         { strcpy(temp_str, "\\n"); }
    \\t         { strcpy(temp_str, "\\t"); }
    \\\'        { strcpy(temp_str, "'"); }
    \\\\        { strcpy(temp_str, "\\\\"); }
    .           { strncpy(temp_str, yytext, sizeof(temp_str)); }
    <<EOF>>     { 
        yyerror("Caractere não terminado");
        exit(1);
    }
}

0[xX]{HEX}+     { yylval.int_val = strtol(yytext, NULL, 16); return INT_LITERAL; }
0{OCT}+         { yylval.int_val = strtol(yytext, NULL, 8); return INT_LITERAL; }
{DIGIT}+        { yylval.int_val = atoi(yytext); return INT_LITERAL; }
{DIGIT}+\.{DIGIT}*([eE][+-]?{DIGIT}+)? { 
    yylval.float_val = atof(yytext); 
    return FLOAT_LITERAL; 
}

"=="            { return OP_EQ; }
"!="            { return OP_NE; }
"<="            { return OP_LE; }
">="            { return OP_GE; }
"&&"            { return OP_AND; }
"||"            { return OP_OR; }
"++"            { return OP_INC; }
"--"            { return OP_DEC; }
"+="            { return OP_ADD_ASSIGN; }
"-="            { return OP_SUB_ASSIGN; }
"*="            { return OP_MUL_ASSIGN; }
"/="            { return OP_DIV_ASSIGN; }
"="             { return OP_ASSIGN; }
"+"             { return OP_ADD; }
"-"             { return OP_SUB; }
"*"             { return OP_MUL; }
"/"             { return OP_DIV; }
"%"             { return OP_MOD; }
"<"             { return OP_LT; }
">"             { return OP_GT; }
"!"             { return OP_NOT; }

";"             { return SEMICOLON; }
","             { return COMMA; }
"."             { return DOT; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }
":"             { return COLON; }
"?"             { return OP_TERNARY; }

{ID}            { 
    int macro_val;
    if (buscar_macro(yytext, &macro_val)) {
        yylval.int_val = macro_val;
        return INT_LITERAL;
    } else {
        yylval.str_val = strdup(yytext);
        return IDENTIFIER;
    }
}

\n              { current_line++; }
{SPACE}+        { /* Ignorar */ }

.               { 
    fprintf(stderr, "Erro léxico linha %d: Caractere inválido '%s'\n", 
            current_line, yytext); 
    extern int temErroSemantico;
    temErroSemantico = 1;
    return 0; 
}

%%
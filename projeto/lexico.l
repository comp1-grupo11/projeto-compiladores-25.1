%{
extern void yyerror(const char *s);
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>

char temp_str[1024];
int current_line = 1;

%}

%option noyywrap
%option yylineno

%x STRING CHAR COMMENT COMMENT_MULTI

DIGIT       [0-9]
LETTER      [a-zA-Z_]
ID          {LETTER}({LETTER}|{DIGIT})*
HEX         [0-9a-fA-F]
OCT         [0-7]
SPACE       [ \t\r]

%%

    /* Palavras-chave e tipos DEVEM vir primeiro */
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

    /* Diretivas de pré-processador */
^#include[ \t]+[\"<][a-zA-Z0-9_./]+[\">] { 
    // Extrai o nome do arquivo
    char *start = strchr(yytext, '<') ? strchr(yytext, '<') : strchr(yytext, '"');
    char *end = strrchr(yytext, '>') ? strrchr(yytext, '>') : strrchr(yytext, '"');
    if(start && end) {
        *end = '\0';
    } 
}

    /* Diretivas de pré-processador */
#include      { }
#define       { }
#ifdef        { }
#endif        { }

    /* Comentários */
"//"            { BEGIN(COMMENT); }
<COMMENT>\n     { BEGIN(INITIAL); current_line++; }
<COMMENT>.      { /* Ignorar */ }

"/*"            { BEGIN(COMMENT_MULTI); }
<COMMENT_MULTI>"*/" { BEGIN(INITIAL); }
<COMMENT_MULTI>\n   { current_line++; }
<COMMENT_MULTI>.    { /* Ignorar */ }

    /* Strings e caracteres (mantido igual) */
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
    \\n         { strcat(temp_str, "\n"); }
    \\t         { strcat(temp_str, "\t"); }
    \\\"        { strcat(temp_str, "\""); }
    \\\\        { strcat(temp_str, "\\"); }
    [^\\\"]+    { strncat(temp_str, yytext, sizeof(temp_str)-strlen(temp_str)-1); }
    <<EOF>>     { 
        yyerror("String não terminada"); 
        BEGIN(INITIAL); 
        return 0; 
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
        BEGIN(INITIAL); 
        return 0; 
    }
}

    /* Números e operadores (mantido igual) */
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

    /* Identificadores DEVEM vir por último */
{ID}            { yylval.str_val = strdup(yytext); return IDENTIFIER; }

\n              { current_line++; }
{SPACE}+        { /* Ignorar */ }

.               { 
    fprintf(stderr, "Erro léxico linha %d: Caractere inválido '%s'\n", 
            current_line, yytext); 
    return 0; 
}

%%
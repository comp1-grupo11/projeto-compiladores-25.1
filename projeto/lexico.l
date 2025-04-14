%{
#include <stdio.h>
#include <string.h>

char temp_str[1024];
%}

/* Estados para lidar com strings, caracteres e comentários */
%x STRING CHAR COMMENT COMMENT_MULTI

%%

    /* Palavras-chave e tipos */
"auto"         { printf("KW_AUTO\n"); }
"break"        { printf("KW_BREAK\n"); }
"case"         { printf("KW_CASE\n"); }
"const"        { printf("KW_CONST\n"); }
"continue"     { printf("KW_CONTINUE\n"); }
"default"      { printf("KW_DEFAULT\n"); }
"do"           { printf("KW_DO\n"); }
"double"       { printf("KW_DOUBLE\n"); }
"else"         { printf("KW_ELSE\n"); }
"enum"         { printf("KW_ENUM\n"); }
"extern"       { printf("KW_EXTERN\n"); }
"float"        { printf("KW_FLOAT\n"); }
"goto"         { printf("KW_GOTO\n"); }
"if"           { printf("KW_IF\n"); }
"long"         { printf("KW_LONG\n"); }
"register"     { printf("KW_REGISTER\n"); }
"return"       { printf("KW_RETURN\n"); }
"short"        { printf("KW_SHORT\n"); }
"signed"       { printf("KW_SIGNED\n"); }
"sizeof"       { printf("KW_SIZEOF\n"); }
"static"       { printf("KW_STATIC\n"); }
"struct"       { printf("KW_STRUCT\n"); }
"switch"       { printf("KW_SWITCH\n"); }
"typedef"      { printf("KW_TYPEDEF\n"); }
"union"        { printf("KW_UNION\n"); }
"unsigned"     { printf("KW_UNSIGNED\n"); }
"void"         { printf("KW_VOID\n"); }
"volatile"     { printf("KW_VOLATILE\n"); }
"while"        { printf("KW_WHILE\n"); }

    /* Tipos */
"int"          { printf("TYPE_INT\n"); }
"char"         { printf("TYPE_CHAR\n"); }

    /* Diretivas de pré-processador */
^#include[ \t]+[\"<][a-zA-Z0-9_./]+[\">] { 
    // Extrai o nome do arquivo
    char *start = strchr(yytext, '<') ? strchr(yytext, '<') : strchr(yytext, '"');
    char *end = strrchr(yytext, '>') ? strrchr(yytext, '>') : strrchr(yytext, '"');
    if(start && end) {
        *end = '\0';
        printf("INCLUDE(%s)\n", start+1);
    } else {
        printf("INCLUDE_ERROR\n");
    }
}

    /* Diretivas de pré-processador */
#include      { printf("PREPROC_INCLUDE\n"); }
#define       { printf("PREPROC_DEFINE\n"); }
#ifdef        { printf("PREPROC_IFDEF\n"); }
#endif        { printf("PREPROC_ENDIF\n"); }

    /* Comentários */
"//"          { BEGIN(COMMENT); }
<COMMENT>\n   { BEGIN(INITIAL); }
<COMMENT>.    { /* Ignora caracteres */ }

"/*"          { BEGIN(COMMENT_MULTI); }
<COMMENT_MULTI>"*/" { BEGIN(INITIAL); }
<COMMENT_MULTI>.|\n { /* Ignora conteúdo */ }

    /* Strings */
\"            { 
    BEGIN(STRING); 
    temp_str[0] = '\0'; 
}
<STRING>{
    \"          { 
        BEGIN(INITIAL); 
        printf("STRING_LITERAL(%s)\n", temp_str); 
    }
    \\\"        { strcat(temp_str, "\""); }
    \\n         { strcat(temp_str, "\\n"); }
    \\t         { strcat(temp_str, "\\t"); }
    \\\\        { strcat(temp_str, "\\\\"); }
    [^\\\"]+    { strncat(temp_str, yytext, sizeof(temp_str)-strlen(temp_str)-1); }
    <<EOF>>     { printf("ERRO: String não fechada\n"); yyterminate(); }
}

    /* Caracteres */
\'            { BEGIN(CHAR); }
<CHAR>{
    \'          { BEGIN(INITIAL); printf("CHAR_LITERAL(%s)\n", temp_str); }
    \\n         { strcpy(temp_str, "\\n"); }
    \\t         { strcpy(temp_str, "\\t"); }
    \\\\        { strcpy(temp_str, "\\\\"); }
    .           { strncpy(temp_str, yytext, sizeof(temp_str)); }
    <<EOF>>     { printf("ERRO: Char não fechado\n"); yyterminate(); }
}

    /* Números (regexs corrigidas) */
0[xX][0-9a-fA-F]+ { printf("HEX_LITERAL(%s)\n", yytext); }
0[0-7]+       { printf("OCT_LITERAL(%s)\n", yytext); }
[0-9]+\.[0-9]*([eE][+-]?[0-9]+)? { printf("FLOAT_LITERAL(%s)\n", yytext); }
[0-9]+        { printf("INT_LITERAL(%s)\n", yytext); }

    /* Operadores e pontuação (ordem corrigida) */
"=="          { printf("OP_EQ\n"); }
"!="          { printf("OP_NE\n"); }
"<="          { printf("OP_LE\n"); }
">="          { printf("OP_GE\n"); }
"&&"          { printf("OP_AND\n"); }
"||"          { printf("OP_OR\n"); }
"++"          { printf("OP_INC\n"); }
"--"          { printf("OP_DEC\n"); }
"+="          { printf("OP_ADD_ASSIGN\n"); }
"-="          { printf("OP_SUB_ASSIGN\n"); }
"*="          { printf("OP_MUL_ASSIGN\n"); }
"/="          { printf("OP_DIV_ASSIGN\n"); }
"->"          { printf("OP_PTR\n"); }
"="           { printf("OP_ASSIGN\n"); }
"+"           { printf("OP_ADD\n"); }
"-"           { printf("OP_SUB\n"); }
"*"           { printf("OP_MUL\n"); }
"/"           { printf("OP_DIV\n"); }
"<"           { printf("OP_LT\n"); }
">"           { printf("OP_GT\n"); }
"!"           { printf("OP_NOT\n"); }
"&"           { printf("OP_ADDR\n"); }
"|"           { printf("OP_BIT_OR\n"); }
"^"           { printf("OP_BIT_XOR\n"); }
"~"           { printf("OP_BIT_NOT\n"); }
"?"           { printf("OP_TERNARY\n"); }
":"           { printf("COLON\n"); }

    /* Pontuação */
";"           { printf("SEMICOLON\n"); }
","           { printf("COMMA\n"); }
"."           { printf("DOT\n"); }
"("           { printf("LPAREN\n"); }
")"           { printf("RPAREN\n"); }
"{"           { printf("LBRACE\n"); }
"}"           { printf("RBRACE\n"); }
"["           { printf("LBRACKET\n"); }
"]"           { printf("RBRACKET\n"); }

    /* Identificadores */
[a-zA-Z_][a-zA-Z0-9_]* { printf("IDENTIFIER(%s)\n", yytext); }

    /* Espaços e tabs */
[ \t\n]+      { /* Ignorar */ }

    /* Erros */
.             { printf("INVALID_TOKEN(%s)\n", yytext); }

%%

int yywrap(void) {
    return 1;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            yyin = f;
        } else {
            perror("Erro ao abrir arquivo");
            return 1;
        }
    }
    yylex();
    return 0;
}
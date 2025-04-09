/* exemplo.y */

%{
#include <stdio.h>
#include <stdlib.h>

/* 
   Declarações explícitas para evitar warnings de 
   “implicit declaration of function yylex/yyerror”
*/
int yylex(void);
void yyerror(const char *s);

%}

%token NUM
%token IF ELSE FOR WHILE DOWHILE RETURN SWITCH STRUCT ESPACO IDENTIFICADOR

%union {
    int intValue;
    char* IDENTIFICADOR;
}

%type <intValue> NUM
%type <strValue> IDENTIFICADOR

%%
/* A gramática */
expr:
    IF         { printf("%s\n", "if");}
    | ELSE         { printf("%s\n", "else");}
    | SWITCH         { printf("%s\n", "switch");}
    | FOR         { printf("%s\n", "for");}
    | WHILE         { printf("%s\n", "while");}
    | DOWHILE         { printf("%s\n", "do while");}
    | RETURN         { printf("%s\n", "return");}
    | STRUCT         { printf("%s\n", "struct");}
    | ESPACO         { printf("%s\n", " ");}
    | NUM         { printf("%d\n", $1);}
    | IDENTIFICADOR {printf("%s\n", "identificador de funcao");}
    
%%

/* Definição de yyerror */
void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático: %s\n", s);
}

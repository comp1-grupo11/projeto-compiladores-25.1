%{
#include "scanner.tab.h"  /* Inclusão do cabeçalho gerado pelo Bison, que contém as definições de tokens */
%}

/* Definições de padrões de token e ações */
%%

[ \t\n]              { return ESPACO; }
struct                 { return STRUCT; }
if                     {return IF;}
else                     {return ELSE;}
switch                  {return SWITCH;}
for                     {return FOR;}
while                     {return WHILE;}
do\{[.\t]*\}while                     {return DOWHILE;}
return                     {return RETURN;}
[a-zA-Z_][a-zA-Z0-9_]*      {return IDENTIFICADOR;}
long int                 {return INTEIRO;}
int                 {return INTEIRO;}
long double                 {return DOUBLE;}
double                 {return DOUBLE;}
(long )?float                 {return FLOAT;}
char                 {return CHAR;}


[+\-]?[0-9]*     {
                        /* Converter o lexema para inteiro (pode estourar se for maior que INT, 
                           mas servirá como exemplo didático) */
                        yylval.intValue = atoi(yytext);
                        return NUM;
                      }


.       { /* tudo mais é ignorado ou tratável como erro */ }


%%

/* Função auxiliar do Flex (quando termina o arquivo de entrada) */
int yywrap(void) {
    return 1;
}

/* Função principal: chamará o parser ao ser executado */
int main(int argc, char **argv) {
    /* Se houver arquivo de entrada como parâmetro, redireciona o yyin */
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            yyin = f;
        }
    }
    return yyparse();
}

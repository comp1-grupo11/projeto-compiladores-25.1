%{
#include <stdio.h>
%}

/* Definições de padrões de token e ações */
%%

if          { printf("KW_IF\n"); }
while       { printf("KW_WHILE\n"); }
for         { printf("KW_FOR\n"); }
return         { printf("KW_RETURN\n"); }
int         { printf("TYPO_INT\n"); }
void         { printf("TYPO_VOID\n"); }
char         { printf("TYPO_CHAR\n"); }

#include[ \t]*<[a-z_]+\.h>         {   printf("INCLUDE()\n");  }

"//".*              { /* Ignorar comentários de linha */ }

"/*"([^*]|\*+[^*/])*\*+\/    { /* Ignorar comentários de bloco */ }

\(([a-zA-Z0-9_]*)\)  { printf("PARAM(%s)\n", yytext); }

[a-zA-Z_][a-zA-Z0-9_]*  { printf("IDENT(%s)\n", yytext); }

"[a-zA-Z0-9_]*"  { printf("TEXT(%s)\n", yytext); }

[0-9]+(\.[0-9]+)?  { printf("NUMBER(%s)\n", yytext); }

([+\-*/=><])|(<=)|(>=)|(==)   { printf("OPERATOR(%s)\n", yytext); }

[ \t\n]+  { /* Não faz nada */ }
;|#           {}

.  { printf("UNKNOWN(%s)\n", yytext); }

%%

/* Função auxiliar do Flex (quando termina o arquivo de entrada) */
int yywrap(void) {
    return 1;
}

/* Função principal: executa o analisador léxico */
int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            yyin = f;
        }
    }
    yylex();
    return 0;
}

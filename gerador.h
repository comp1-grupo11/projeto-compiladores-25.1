#ifndef GERADOR_H
#define GERADOR_H
#include <stdio.h>
#include "ast.h"

char *gerarIR(NoAST *no, FILE *saida);

#endif // GERADOR_H

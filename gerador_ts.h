#ifndef GERADOR_TS_H
#define GERADOR_TS_H

typedef struct VarDecl
{
    char nome[64];
    struct VarDecl *proximo;
} VarDecl;

#endif
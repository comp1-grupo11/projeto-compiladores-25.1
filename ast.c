#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

NoAST *criarNoOp(char op, NoAST *esq, NoAST *dir) {
    NoAST *no = malloc(sizeof(NoAST));
    no->operador = op;
    no->esquerda = esq;
    no->direita = dir;
    no->tipo = (esq->tipo == dir->tipo) ? esq->tipo : TIPO_ERRO;
    return no;
}

NoAST *criarNoNum(Tipo tipo, void *valor) {
    NoAST *novoNo = (NoAST *)malloc(sizeof(NoAST));
    if (novoNo == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST.\n");
        exit(EXIT_FAILURE);
    }
    novoNo->operador = '\0'; // Não é um operador
    novoNo->nome[0] = '\0';  // Não é um identificador
    novoNo->tipo = tipo;
    novoNo->esquerda = NULL;
    novoNo->direita = NULL;

    switch (tipo) {
        case TIPO_INT:
            novoNo->valor_numerico.val_int = *(int *)valor;
            break;
        case TIPO_FLOAT:
            novoNo->valor_numerico.val_float = *(float *)valor;
            break;
        case TIPO_DOUBLE:
            novoNo->valor_numerico.val_double = *(double *)valor;
            break;
        default:
            fprintf(stderr, "Tipo numerico invalido para criarNoNum.\n");
            free(novoNo);
            return NULL;
    }
    return novoNo;
}

NoAST *criarNoId(char *nome, Tipo tipo) {
    NoAST *no = malloc(sizeof(NoAST));
    strcpy(no->nome, nome);
    no->operador = 0;
    no->tipo = tipo;
    no->esquerda = no->direita = NULL;
    return no;
}

void imprimirAST(NoAST *no) {
    if (no == NULL) {
        return;
    }

    if (no->operador != '\0') {
        printf("(%c ", no->operador);
        imprimirAST(no->esquerda);
        printf(" ");
        imprimirAST(no->direita);
        printf(")");
    } else if (no->nome[0] != '\0') {
        printf("%s", no->nome);
    } else {
        switch (no->tipo) {
            case TIPO_INT:
                printf("%d", no->valor_numerico.val_int);
                break;
            case TIPO_FLOAT:
                printf("%f", no->valor_numerico.val_float);
                break;
            case TIPO_DOUBLE:
                printf("%lf", no->valor_numerico.val_double);
                break;
            default:
                printf("[ERRO TIPO NUMERICO]");
                break;
        }
    }
}

int tiposCompativeis(Tipo t1, Tipo t2) {
    return t1 == t2;
}
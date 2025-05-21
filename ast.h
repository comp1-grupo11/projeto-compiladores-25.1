#ifndef AST_H
#define AST_H

typedef enum
{
    TIPO_INT,
    TIPO_FLOAT,
    TIPO_DOUBLE,
    TIPO_CHAR,
    TIPO_VOID,
    TIPO_ERRO
} Tipo;

typedef struct noAST
{
    char operador;
    char nome[32];
    Tipo tipo;

    union
    {
        int val_int;
        float val_float;
        double val_double;
    } valor_numerico;

    struct noAST *esquerda;
    struct noAST *direita;
} NoAST;

NoAST *criarNoOp(char op, NoAST *esq, NoAST *dir);
NoAST *criarNoNum(Tipo tipo, void *valor);
NoAST *criarNoId(char *nome, Tipo tipo);
void imprimirAST(NoAST *no);
int tiposCompativeis(Tipo t1, Tipo t2);

#endif
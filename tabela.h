#ifndef TABELA_H
#define TABELA_H

#include "ast.h"

// Enum para a categoria do símbolo (opcional, mas recomendado)
typedef enum {
    VARIAVEL,
    FUNCAO,
    PARAMETRO,
    // Adicione outras categorias conforme necessário (ex: CONSTANTE, TIPO)
} CategoriaSimbolo;

// Enum para o escopo (ex: global, local)
typedef enum {
    ESCOPO_GLOBAL,
    ESCOPO_LOCAL,
    // Adicione outros escopos conforme necessário (ex: ESCOPO_FUNCAO)
} Escopo;

typedef struct simbolo {
    char nome[32];
    Tipo tipo;

    CategoriaSimbolo categoria;
    int tamanho_bytes;
    int dimensao;
    int linha_declaracao;
    int linha_ultimo_uso;
    int endereco;
    Escopo escopo;

    struct simbolo *proximo;
} Simbolo;

void inserirSimbolo(char *nome, Tipo tipo, CategoriaSimbolo categoria, int tamanho_bytes, int dimensao, int linha_declaracao, int endereco, Escopo escopo);
Simbolo *buscarSimbolo(char *nome);
void imprimirTabela();

#endif
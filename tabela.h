#ifndef TABELA_H
#define TABELA_H
#define TAM 211

#include "ast.h"

// Enum para a categoria do símbolo
typedef enum {
    VARIAVEL,
    FUNCAO,
    PARAMETRO,
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

typedef struct tabela_simbolos {
    Simbolo *tabela[TAM];
    struct tabela_simbolos *anterior;
} TabelaSimbolos;

extern TabelaSimbolos *escopo_atual;

void inserirSimbolo(char *nome, Tipo tipo, CategoriaSimbolo categoria, int tamanho_bytes, int dimensao, int linha_declaracao, int endereco, Escopo escopo);
Simbolo *buscarSimbolo(char *nome);
Simbolo *buscarSimboloNoEscopoAtual(char *nome);
void imprimirTabela();

#endif
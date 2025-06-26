#ifndef TABELA_H
#define TABELA_H
#define TAM 211

#include "ast.h"

// Enum para a categoria do símbolo
typedef enum
{
    VARIAVEL,
    FUNCAO,
    PARAMETRO,
} CategoriaSimbolo;

// Enum simplificado para escopo: apenas global ou local
typedef enum
{
    ESCOPO_GLOBAL,
    ESCOPO_LOCAL
} Escopo;

typedef struct simbolo
{
    char nome[32];
    Tipo tipo;

    CategoriaSimbolo categoria;
    int tamanho_bytes;
    int dimensao;
    int linha_declaracao;
    int linha_ultimo_uso;
    int endereco;
    Escopo escopo;

    char nome_struct[32]; // Adicionado para guardar o nome do struct (ex: Point)

    struct simbolo *proximo;
} Simbolo;

typedef struct tabela_simbolos
{
    Simbolo *tabela[TAM];
    struct tabela_simbolos *anterior;
    Escopo escopo_desta_tabela;
    int nivel_desta_tabela;
} TabelaSimbolos;

extern TabelaSimbolos *escopo_atual;

extern int nivel_escopo; // Variável global para controlar o nível do escopo

void inserirSimbolo(char *nome, Tipo tipo, CategoriaSimbolo categoria, int tamanho_bytes, int dimensao, int linha_declaracao, int endereco, Escopo escopo, const char *nome_struct);
Simbolo *buscarSimbolo(char *nome);
Simbolo *buscarSimboloNoEscopoAtual(char *nome);
void imprimirTabela();

#endif
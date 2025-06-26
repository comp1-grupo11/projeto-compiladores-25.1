#ifndef TABELA_H
#define TABELA_H
#define TAM 211
#define ESCOPO_GLOBAL 0

#include "ast.h"

// Enum para a categoria do símbolo
typedef enum
{
    VARIAVEL,
    FUNCAO,
    PARAMETRO,
} CategoriaSimbolo;

// Enum para o escopo (ex: global, local)
typedef enum {
    TIPO_ESCOPO_GLOBAL,
    TIPO_ESCOPO_FUNCAO,
    TIPO_ESCOPO_IF,
    TIPO_ESCOPO_ELSE,
    TIPO_ESCOPO_FOR,
    TIPO_ESCOPO_WHILE,
    TIPO_ESCOPO_SWITCH,
} TipoEscopo;

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
    int id_escopo;
    TipoEscopo tipo_escopo;

    char nome_struct[32]; // Adicionado para guardar o nome do struct (ex: Point)

    struct simbolo *proximo;
} Simbolo;

typedef struct tabela_simbolos
{
    Simbolo *tabela[TAM];
    struct tabela_simbolos *anterior;
    TipoEscopo tipo_escopo_desta_tabela;
    int nivel_desta_tabela;
} TabelaSimbolos;

extern TabelaSimbolos *escopo_atual;

extern int nivel_escopo; // Variável global para controlar o nível do escopo


// Pilha de tipos de escopo
void empilhaTipoEscopo(TipoEscopo tipo);
void desempilhaTipoEscopo(void);
TipoEscopo tipoEscopoAtual(void);


void inserirSimbolo(char *nome, Tipo tipo, CategoriaSimbolo categoria, int tamanho_bytes, int dimensao, int linha_declaracao, int endereco, int id_escopo, TipoEscopo tipo_escopo, const char *nome_struct);
Simbolo *buscarSimbolo(char *nome);
Simbolo *buscarSimboloNoEscopoAtual(char *nome);
void imprimirTabela();

#endif
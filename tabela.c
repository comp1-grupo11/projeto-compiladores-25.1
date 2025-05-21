#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"

#define TAM 211

Simbolo *tabela[TAM];

unsigned hash(char *s) {
    unsigned h = 0;
    while (*s) h = (h << 4) + *s++;
    return h % TAM;
}

void inserirSimbolo(char *nome, Tipo tipo, CategoriaSimbolo categoria, int tamanho_bytes,
    int dimensao, int linha_declaracao, int endereco, Escopo escopo) {
    unsigned i = hash(nome);

    Simbolo *novoSimbolo = (Simbolo *)malloc(sizeof(Simbolo));
    if (novoSimbolo == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para Simbolo.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(novoSimbolo->nome, nome, 31);
    novoSimbolo->nome[31] = '\0';
    novoSimbolo->tipo = tipo;
    novoSimbolo->categoria = categoria;
    novoSimbolo->tamanho_bytes = tamanho_bytes;
    novoSimbolo->dimensao = dimensao;
    novoSimbolo->linha_declaracao = linha_declaracao;
    novoSimbolo->linha_ultimo_uso = -1;
    novoSimbolo->endereco = endereco;
    novoSimbolo->escopo = escopo;

    // Adiciona o novo símbolo ao início da lista encadeada na posição da hash
    novoSimbolo->proximo = tabela[i];
    tabela[i] = novoSimbolo;

}

Simbolo *buscarSimbolo(char *nome) {
    unsigned i = hash(nome);
    for (Simbolo *s = tabela[i]; s; s = s->proximo) {
        if (strcmp(s->nome, nome) == 0) return s;
    }
    return NULL;
}

void imprimirTabela() {
    printf("\n--- TABELA DE SIMBOLOS ---\n");
    printf("%-20s %-10s %-10s %-10s %-8s %-8s %-8s %-8s %-8s\n",
           "Nome", "Tipo", "Categoria", "Escopo", "Tam", "Dim", "Decl", "Uso", "Addr");
    printf("-------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < TAM; i++) {
        for (Simbolo *s = tabela[i]; s; s = s->proximo) {
            char tipo_str[10];
            // Convertendo o enum Tipo para string
            switch (s->tipo) {
                case TIPO_INT:    strcpy(tipo_str, "INT"); break;
                case TIPO_FLOAT:  strcpy(tipo_str, "FLOAT"); break;
                case TIPO_DOUBLE: strcpy(tipo_str, "DOUBLE"); break;
                case TIPO_CHAR:   strcpy(tipo_str, "CHAR"); break;   // Adicionado
                case TIPO_STRING: strcpy(tipo_str, "STRING"); break; // Adicionado
                case TIPO_VOID:   strcpy(tipo_str, "VOID"); break;
                case TIPO_ERRO:   strcpy(tipo_str, "ERRO"); break; // Adicionado
                default:     strcpy(tipo_str, "UNKNOWN"); break;
            }

            char categoria_str[10];
            // Convertendo a CategoriaSimbolo para string
            switch (s->categoria) {
                case VARIAVEL:  strcpy(categoria_str, "VAR"); break;
                case FUNCAO:    strcpy(categoria_str, "FUNC"); break;
                case PARAMETRO: strcpy(categoria_str, "PARAM"); break;
                default:        strcpy(categoria_str, "???"); break;
            }

            char escopo_str[10];
            // Convertendo o Escopo para string
            switch (s->escopo) {
                case ESCOPO_GLOBAL: strcpy(escopo_str, "GLOBAL"); break;
                case ESCOPO_LOCAL:  strcpy(escopo_str, "LOCAL"); break;
                default:            strcpy(escopo_str, "???"); break;
            }

            printf("%-20s %-10s %-10s %-10s %-8d %-8d %-8d %-8d %-8d\n",
                   s->nome, tipo_str, categoria_str, escopo_str,
                   s->tamanho_bytes, s->dimensao,
                   s->linha_declaracao, s->linha_ultimo_uso, s->endereco);
        }
    }
    printf("-------------------------------------------------------------------------------------------------\n");
}
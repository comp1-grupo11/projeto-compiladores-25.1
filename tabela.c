#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"

#define TAM 211

TabelaSimbolos *escopo_atual = NULL;

unsigned hash(char *s)
{
    unsigned h = 0;
    while (*s)
        h = (h << 4) + *s++;
    return h % TAM;
}

void criarEscopoLocal()
{
    TabelaSimbolos *novo = malloc(sizeof(TabelaSimbolos));
    if (!novo)
    {
        fprintf(stderr, "Erro de alocacao de memoria para TabelaSimbolos.\n");
        exit(EXIT_FAILURE);
    }
    memset(novo->tabela, 0, sizeof(novo->tabela));
    novo->anterior = escopo_atual;
    escopo_atual = novo;
}

void destruirEscopoLocal()
{
    if (!escopo_atual)
        return;
    for (int i = 0; i < TAM; i++)
    {
        Simbolo *s = escopo_atual->tabela[i];
        while (s)
        {
            Simbolo *prox = s->proximo;
            free(s);
            s = prox;
        }
    }
    TabelaSimbolos *ant = escopo_atual->anterior;
    free(escopo_atual);
    escopo_atual = ant;
}

void inserirSimbolo(char *nome, Tipo tipo, CategoriaSimbolo categoria, int tamanho_bytes,
                    int dimensao, int linha_declaracao, int endereco, Escopo escopo, const char *nome_struct)
{
    if (!escopo_atual)
        criarEscopoLocal();

    unsigned i = hash(nome);

    Simbolo *novoSimbolo = (Simbolo *)malloc(sizeof(Simbolo));
    if (novoSimbolo == NULL)
    {
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
    if (tipo == TIPO_OBJETO && nome_struct)
    {
        strncpy(novoSimbolo->nome_struct, nome_struct, 31);
        novoSimbolo->nome_struct[31] = '\0';
    }
    else
    {
        novoSimbolo->nome_struct[0] = '\0';
    }

    // Adiciona o novo símbolo ao início da lista encadeada na posição da hash
    novoSimbolo->proximo = escopo_atual->tabela[i];
    escopo_atual->tabela[i] = novoSimbolo;
}

Simbolo *buscarSimbolo(char *nome)
{
    TabelaSimbolos *esc = escopo_atual;
    while (esc)
    {
        unsigned i = hash(nome);
        for (Simbolo *s = esc->tabela[i]; s; s = s->proximo)
            if (strcmp(s->nome, nome) == 0)
            {
                return s;
            }
        esc = esc->anterior;
    }
    return NULL;
}

Simbolo *buscarSimboloNoEscopoAtual(char *nome)
{
    if (!escopo_atual)
        return NULL;
    unsigned i = hash(nome);
    for (Simbolo *s = escopo_atual->tabela[i]; s; s = s->proximo)
    {
        if (strcmp(s->nome, nome) == 0)
            return s;
    }
    return NULL;
}

void imprimirTabela()
{
    printf("\n--- TABELA DE SIMBOLOS ---\n");
    printf("%-20s %-10s %-10s %-10s %-8s %-8s %-8s %-8s %-8s\n",
           "Nome", "Tipo", "Categoria", "Escopo", "Tam", "Dim", "Decl", "Uso", "Addr");
    printf("-------------------------------------------------------------------------------------------------\n");

    TabelaSimbolos *esc = escopo_atual;
    while (esc)
    {
        for (int i = 0; i < TAM; i++)
        {
            for (Simbolo *s = esc->tabela[i]; s; s = s->proximo)
            {

                char tipo_str[10];
                // Convertendo o enum Tipo para string
                switch (s->tipo)
                {
                case TIPO_INT:
                    strcpy(tipo_str, "INT");
                    break;
                case TIPO_FLOAT:
                    strcpy(tipo_str, "FLOAT");
                    break;
                case TIPO_DOUBLE:
                    strcpy(tipo_str, "DOUBLE");
                    break;
                case TIPO_CHAR:
                    strcpy(tipo_str, "CHAR");
                    break;
                case TIPO_STRING:
                    strcpy(tipo_str, "STRING");
                    break;
                case TIPO_VOID:
                    strcpy(tipo_str, "VOID");
                    break;
                case TIPO_OBJETO:
                    strcpy(tipo_str, "OBJETO");
                    break;
                case TIPO_ERRO:
                    strcpy(tipo_str, "ERRO");
                    break;
                default:
                    strcpy(tipo_str, "UNKNOWN");
                    break;
                }

                char categoria_str[10];
                // Convertendo a CategoriaSimbolo para string
                switch (s->categoria)
                {
                case VARIAVEL:
                    strcpy(categoria_str, "VAR");
                    break;
                case FUNCAO:
                    strcpy(categoria_str, "FUNC");
                    break;
                case PARAMETRO:
                    strcpy(categoria_str, "PARAM");
                    break;
                default:
                    strcpy(categoria_str, "???");
                    break;
                }

                char escopo_str[10];
                // Convertendo o Escopo para string
                switch (s->escopo)
                {
                case ESCOPO_GLOBAL:
                    strcpy(escopo_str, "GLOBAL");
                    break;
                case ESCOPO_LOCAL:
                    strcpy(escopo_str, "LOCAL");
                    break;
                default:
                    strcpy(escopo_str, "???");
                    break;
                }

                printf("%-20s %-10s %-10s %-10s %-8d %-8d %-8d %-8d %-8d\n",
                       s->nome, tipo_str, categoria_str, escopo_str,
                       s->tamanho_bytes, s->dimensao,
                       s->linha_declaracao, s->linha_ultimo_uso, s->endereco);
            }
        }
        esc = esc->anterior;
    }
    printf("-------------------------------------------------------------------------------------------------\n");
}
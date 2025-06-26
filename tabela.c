#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"

#define MAX_ESCOPO 100

TabelaSimbolos *escopo_atual = NULL;

int nivel_escopo = 0; // Variável global para controlar o nível do escopo

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
    nivel_escopo++;
}

void destruirEscopoLocal()
{
    if (!escopo_atual)
        return;

    //imprimirTabela();
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
    nivel_escopo--;
}

void inserirSimbolo(char *nome, Tipo tipo, CategoriaSimbolo categoria, int tamanho_bytes,
                    int dimensao, int linha_declaracao, int endereco, int id_escopo, TipoEscopo tipo_escopo, const char *nome_struct)
{
    switch (tipo_escopo) {
            case TIPO_ESCOPO_GLOBAL:
                //printf("Insere %s com escopo GLOBAL na linha: %d\n", nome,linha_declaracao);
                break;
            case TIPO_ESCOPO_FUNCAO:
                //printf("Insere %s com escopo FUNCAO na linha: %d\n", nome,linha_declaracao);
                break;
            case TIPO_ESCOPO_IF:
                //printf("Insere %s com escopo IF na linha: %d\n", nome,linha_declaracao);
                break;
            case TIPO_ESCOPO_ELSE:
                //printf("Insere %s com escopo ELSE na linha: %d\n",nome, linha_declaracao);
                break;
            case TIPO_ESCOPO_FOR:
                //printf("Insere %s com escopo FOR na linha: %d\n", nome,linha_declaracao);
                break;
            case TIPO_ESCOPO_WHILE:
                //printf("Insere %s com escopo WHILE na linha: %d\n", nome, linha_declaracao);
                break;
            case TIPO_ESCOPO_SWITCH:
                //printf("Insere %s com escopo SWITCH na linha: %d\n",nome, linha_declaracao);
                break;
            default:
                //printf("Insere %s com escopo ??? na linha: %d\n", nome,linha_declaracao);
                break;
            }
    //if (!escopo_atual)
        //criarEscopoLocal();

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
    novoSimbolo->id_escopo = id_escopo;
    novoSimbolo->tipo_escopo = tipo_escopo;
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


static TipoEscopo pilha_tipo_escopo[MAX_ESCOPO];
static int topo_tipo_escopo = -1;

void imprimirPilhaTipoEscopo(int topo) {
    printf("---- TOPO ----\n");
    for (int i = topo; i >= 0; i--) {
        switch (pilha_tipo_escopo[i]) {
            case TIPO_ESCOPO_GLOBAL:  printf("| GLOBAL   |\n"); break;
            case TIPO_ESCOPO_FUNCAO:  printf("| FUNCAO   |\n"); break;
            case TIPO_ESCOPO_IF:      printf("| IF       |\n"); break;
            case TIPO_ESCOPO_ELSE:    printf("| ELSE     |\n"); break;
            case TIPO_ESCOPO_FOR:     printf("| FOR      |\n"); break;
            case TIPO_ESCOPO_SWITCH:  printf("| SWITCH   |\n"); break;
            default:                  printf("| DESCONHECIDO |\n"); break;
        }
        printf("-------------\n");
    }
    printf("---- BASE ----\n");
}

void empilhaTipoEscopo(TipoEscopo tipo)
{
    if (topo_tipo_escopo < MAX_ESCOPO - 1) {
        switch (tipo) {
            case TIPO_ESCOPO_GLOBAL:
                //printf("Empilha escopo GLOBAL\n");
                break;
            case TIPO_ESCOPO_FUNCAO:
                //printf("Empilha escopo FUNCAO\n");
                break;
            case TIPO_ESCOPO_IF:
                //printf("Empilha escopo IF\n");
                break;
            case TIPO_ESCOPO_ELSE:
                //printf("Empilha escopo ELSE\n");
                break;
            case TIPO_ESCOPO_FOR:
                //printf("Empilha escopo FOR\n");
                break;
            case TIPO_ESCOPO_WHILE:
                //printf("Empilha escopo WHILE\n");
                break;
            case TIPO_ESCOPO_SWITCH:
                //printf("Empilha escopo SWITCH\n");
                break;
            default:
                //printf("Empilha escopo ???\n");
                break;
            }
        pilha_tipo_escopo[++topo_tipo_escopo] = tipo;
        //printf("\nEstado da pilha depois de empilhar:\n");
        //imprimirPilhaTipoEscopo(topo_tipo_escopo);

        } else {
            fprintf(stderr, "Estouro da pilha de tipo de escopo.\n");
        }
}

void desempilhaTipoEscopo(void)
{
    if (topo_tipo_escopo >= 0)
        {
            topo_tipo_escopo--;
            //printf("\nEstado da pilha depois de desempilhar:\n");
            //imprimirPilhaTipoEscopo(topo_tipo_escopo);
    }

    else
        fprintf(stderr, "Pilha de tipo de escopo já está vazia.\n");
}



TipoEscopo tipoEscopoAtual(void)
{
    //printf("\nEstado da pilha ao chamar tipoEscopoAtual():\n");
    //imprimirPilhaTipoEscopo(topo_tipo_escopo);

    if (topo_tipo_escopo >= 0)
        return pilha_tipo_escopo[topo_tipo_escopo];
    return TIPO_ESCOPO_GLOBAL;
}


void imprimirTabela()
{
    printf("\n--- TABELA DE SIMBOLOS ---\n");
    printf("%-20s %-10s %-10s %-10s %-8s %-8s %-8s %-8s %-8s %-8s\n",
           "Nome", "Tipo", "Categoria", "Escopo", "IDEscopo", "Tam", "Dim", "Decl", "Uso", "Addr");
    printf("-------------------------------------------------------------------------------------------------------------\n");

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
                // Convertendo o TipoEscopo para string
                switch (s->tipo_escopo)
                {
                case TIPO_ESCOPO_GLOBAL:
                    strcpy(escopo_str, "GLOBAL");
                    break;
                case TIPO_ESCOPO_FUNCAO:
                    strcpy(escopo_str, "FUNCAO");
                    break;
                case TIPO_ESCOPO_IF:
                    strcpy(escopo_str, "IF");
                    break;
                case TIPO_ESCOPO_ELSE:
                    strcpy(escopo_str, "ELSE");
                    break;
                case TIPO_ESCOPO_FOR:
                    strcpy(escopo_str, "FOR");
                    break;
                case TIPO_ESCOPO_WHILE:
                    strcpy(escopo_str, "WHILE");
                    break;
                case TIPO_ESCOPO_SWITCH:
                    strcpy(escopo_str, "SWITCH");
                    break;
                default:
                    strcpy(escopo_str, "???");
                    break;
                }

                printf("%-20s %-10s %-10s %-10s %-8d %-8d %-8d %-8d %-8d %-8d\n",
                       s->nome, tipo_str, categoria_str, escopo_str,
                       s->id_escopo, // <-- Aqui imprime o id do escopo
                       s->tamanho_bytes, s->dimensao,
                       s->linha_declaracao, s->linha_ultimo_uso, s->endereco);
            }
        }
        esc = esc->anterior;
    }
    printf("-------------------------------------------------------------------------------------------------------------\n");
}
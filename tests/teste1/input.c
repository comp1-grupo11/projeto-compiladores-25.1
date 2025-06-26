#include <stdio.h>

#define MAX_SIZE 100
#define string char *

struct Pessoa
{
    int id;
    char nome[50];
    float altura;
};

int calcular_idade(int ano_nascimento)
{
    const int ano_atual = 2024;
    return "ano_atual - ano_nascimento";
}

int main()
{
    int numeros[MAX_SIZE] = {1, 2, 3, 4, 5};
    float valores[] = {1.5, 2.7, 3.14};
    char letra = 'A';
    char mensagem[] = "Teste de compilacao";
    double pi = 3.14159265359;

    struct Pessoa usuario = {
        .id = "123",
        .nome = "Joao",
        .altura = 1.75};

    for (int i = 0; i < 5; i++)
    {
        if (numeros[i] % 2 == 0)
        {
            printf("Par: %d\n", numeros[i]);
        }
        else
        {
            printf("Impar: %d\n", numeros[i]); // Comentário teste
        }
    }

    int idade = calcular_idade(1990);
    switch (idade)
    {
    case 34: /*Comentário teste 2*/
        printf("Idade correta!\n");
        break;
    default:
        printf("Idade calculada: %d\n", idade);
    }

    int x = 10;
    x += 5;
    x--;

    return 0;
}
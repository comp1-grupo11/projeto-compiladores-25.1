#include <stdio.h>

struct Point
{
    int x;
    int y;
};

int main()
{
    // Chamada de função básica
    printf("Iniciando teste da AST\n");

    // Declarações e operadores
    int i = 0;
    float f = 3.14;
    char c = 'A';
    i += 2;
    i++;
    i *= 3;
    f /= 2.5;

    // Estrutura condicional complexa
    if (i > 5 && f < 10.0)
    {
        printf("Condição complexa satisfeita\n");
    }
    else
    {
        printf("Condição não satisfeita\n");
    }

    // Loop com switch aninhado
    for (int j = 0; j < 15; j = j + 1)
    {
        switch (j % 5)
        {
        case 0:
            printf("Múltiplo de 5");
            break;
        case 1:
        case 2:
            printf("Resíduo baixo");
            break;
        default:
            printf("Outro resíduo");
        }
    }

    // Uso de struct
    struct Point p1;
    p1.x = 10;
    p1.y = i * 2;

    // Operações com struct
    struct Point p2 = {.x = p1.y, .y = p1.x};

    printf("Ponto final: (%d, %d)\n", p2.x, p2.y);

    return 0;
}
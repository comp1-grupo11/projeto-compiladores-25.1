#include <stdio.h>
// #define PI 3.14

int somar(int a, int b)
{
    return a + b;
}

int main()
{
    int x = 10;
    int y = 20;
    int resultado = somar(x, y);

    if (resultado > 20)
    {
        resultado += 5;
    }
    else
    {
        resultado -= 2;
    }

    for (int i = 0; i < 3; i++)
    {
        resultado = resultado + i;
    }

    while (x > 0)
    {
        x--;
    }

    int opcao = 2;
    switch (opcao)
    {
    case 1:
        resultado = 100;
        break;
    case 2:
        resultado = 200;
        break;
    default:
        resultado = 0;
    }

    int a = 5;
    int b = 10;
    int maior = (a > b) ? a : b;

    return 0;
}
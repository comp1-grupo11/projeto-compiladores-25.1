int soma(int a, int b)
{
    return a + b;
}

int main()
{
    int resultado = soma(10, 20);

    if (resultado > 15)
    {
        resultado = resultado * 2;
    }
    else
    {
        resultado = resultado - 5;
    }

    for (int i = 0; i < 3; i = i + 1)
    {
        resultado = resultado + i;
    }

    return resultado;
}
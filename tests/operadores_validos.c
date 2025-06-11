struct Ponto {
    int x;
    int y;
};

int calcular(int a, int b) {
    int soma = a + b;
    int sub = a - b;
    int mul = a * b;
    int div = a / b;
    int mod = a % b;

    soma += 1;
    sub -= 1;
    mul *= 2;
    div /= 2;

    int eq = (a == b);
    int ne = (a != b);
    int lt = (a < b);
    int le = (a <= b);
    int gt = (a > b);
    int ge = (a >= b);

    int e = (a && b);
    int ou = (a || b);
    int nao = !a;

    a++;
    b--;

    struct Ponto p;
    p.x = soma;
    p.y = sub;

    int v[4];
    v[0] = p.x;
    v[1] = p.y;
    v[2] = v[0] + v[1];
    v[3] = ((a != ++b) || !((--v[2] < b)));

    return v[2];
}

// TESTE 1: Uso de variável não declarada
int main() {
    x = 10;  
    return 0;
}

// TESTE 2: Declaração duplicada no mesmo escopo
int main2() {
    int y;
    int y; 
    return 0;
}

// TESTE 3: Atribuição com tipos incompatíveis
int main3() {
    int z;
    z = "texto";  
    return 0;
}

// TESTE 4: Retorno com tipo incompatível
int main4() {
    return "abc";  
}

// TESTE 5: Uso fora do escopo
int main5() {
    if (1) {
        int a = 10;
    }
    a = 5;  
    return 0;
}

// TESTE 6: Variável usada antes da declaração
int main6() {
    b = 20;  
    int b;
    return 0;
}

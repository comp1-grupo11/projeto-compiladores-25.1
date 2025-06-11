# 1. Visão Geral
Este projeto implementa um compilador de C para TypeScript com escopo reduzido, visando converter uma linguagem C simplificada em código TypeScript equivalente, com foco em estrutura, tipos e fluxo de controle.

# 2. Versões das Linguagens
- **C**: Padrão base C90/C99 com subconjunto limitado.
- **TypeScript**: Compatível com TypeScript 4.x, com foco em recursos de tipagem e estrutura de código.

# 3. Funcionalidades Suportadas

## 3.1 Tipos de Dados
- `int`
- `float`
- `double`
- `char`
- `string`
- `void` (usado em funções)

## 3.2 Operadores
- Aritméticos: `+`, `-`, `*`, `/`, `%`
- Atribuição: `=`, `+=`, `-=`, `*=`, `/=`, `%=`  
- Comparação: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Lógicos: `&&`, `||`, `!`
- Unários: `++`, `--`
- Outros: `.` (acesso a campo)

## 3.3 Estruturas de Controle
- Condicional: `if`, `else`
- Laços: `while`, `for`
- Seleção múltipla: `switch`, `case`, `default`
- Controle de fluxo: `break`, `continue`, `return`

## 3.4 Declarações e Definições
- Declaração de variáveis (locais e globais)
- Definição de funções com lista de parâmetros
- Chamada de função
- Atribuição a campos de `struct`
- Acesso a campos de `struct`

# 4. Funcionalidades Não Suportadas

| Funcionalidade                          | Suporte |
|----------------------------------------|---------|
| Ponteiros                              | ❌      |
| Alocação dinâmica (`malloc`, `free`)   | ❌      |
| Arrays e matrizes                      | ❌      |
| Structs complexas (aninhadas)          | ❌      |
| Unions, enums                          | ❌      |
| Diretivas de pré-processador (`#include`, `#define`) | ❌      |
| Tipos derivados (e.g., `typedef`)      | ❌      |
| Manipulação direta de memória          | ❌      |
| Recursão                               | Parcial |
| Funções variádicas (`printf`, `scanf`) | ❌      |

# 5. Estrutura Interna

## 5.1 AST (Abstract Syntax Tree)
A árvore sintática abstrata é construída com nós representando:
- Expressões literais, identificadores e operadores
- Comandos como `if`, `while`, `for`, `switch`, `break`, `continue`, `return`
- Chamadas de função
- Definições e blocos de código

## 5.2 Tabela de Símbolos
Gerencia escopos, tipos, nomes, e categorias (variável, função, parâmetro). Suporta escopo global e local.

# 6. Geração de Código TypeScript
A conversão para TypeScript considera:
- Tipos C mapeados para tipos TS (`int → number`, `char → string`, etc.)
- Estruturas de controle são transpostas diretamente
- Funções C viram `function` em TypeScript
- Campos de `struct` são acessados com `objeto.campo`

# 7. Exemplo de Conversão

## Código C:
```c
int soma(int a, int b) {
    return a + b;
}
```

## Geração TypeScript:
```ts
function soma(a: number, b: number): number {
    return a + b;
}
```

# 8. Limitações Conhecidas
- Sem verificação de overflow de tipos
- Não há análise de tempo de execução ou otimização
- Não suporta modularização nem separação em arquivos

## 📑 Histórico de Versões

| Versão | Descrição | Autor(es) | Data de Produção | Revisor(es) | Data de Revisão | 
| :----: | --------- | --------- | :--------------: | ----------- | :-------------: |
| `1.0` | Versão inicial da documentação | [Israel Thalles](https://github.com/IsraelThalles) | 01/05/2025 |  |  |

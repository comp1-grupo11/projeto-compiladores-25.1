# 1. Visão Geral
Este projeto implementa um compilador de C para TypeScript com escopo reduzido, visando converter uma linguagem C simplificada em código TypeScript equivalente, com foco em estrutura, tipos e fluxo de controle. Ele segue a cadeia clássica **análise léxica → análise sintática → AST → geração de IR → geração de TypeScript**, mantendo:

* **AST fortemente tipada**, para validação semântica e rastreamento de tipos (`ast.h`)
* **Tabela de símbolos** que acompanha declarações e escopos (`tabela.h`)
* **Geração de IR** intermediário para facilitar a tradução (`gerador.h`)
* **Emissão de TypeScript** baseado na IR e nas declarações coletadas (`gerador_ts.h`)

## 2. Arquitetura e Componentes

| Componente                | Arquivo        | Responsabilidade                                                                                                |
| ------------------------- | -------------- | --------------------------------------------------------------------------------------------------------------- |
| **Análise Léxica**        | `lexico.l`     | Tokenização de palavras‑chave, identificadores, literais, operadores, diretivas simples (`#include`, `#define`) |
| **Análise Sintática**     | `parser.y`     | Reconhecimento de declarações, expressões, comandos e construção da AST (via Bison)                             |
| **Definição da AST**      | `ast.h`        | Tipos de nós (`NodeType`), literais, operadores e estruturas de árvore                                          |
| **Tabela de Símbolos**    | `tabela.h`     | Inserção/busca de variáveis, funções e parâmetros; controle de escopos                                          |
| **Geração de IR**         | `gerador.h`    | Função `gerarIR(NoAST*, FILE*)` → produz representação intermediária (string)                                   |
| **Geração de TypeScript** | `gerador_ts.h` | Estruturas de suporte (`VarDecl`) e funções para emitir código `.ts`                                            |

## 3. Sintaxes de C **suportadas**

### 3.1 Tipos de Dados

* **Básicos**: `int`, `float`, `double`, `char`, `void`
* **Strings literais** (tratadas como arrays de `char`)
* **Structs** *como tipo* (`struct Nome`), mas **sem** gramática para definição de campos

### 3.2 Declarações

* **Variáveis e parâmetros** (simples e com inicialização):

  ```c
  int x;
  float y = 3.14;
  char c = 'a';
  ```
* **Arrays** (tamanho fixo, inicialização via lista ou string):

  ```c
  int v[10];
  int w[3] = {1,2,3};
  char s[] = "oi";
  ```
* **Funções** (definições com corpo; não há suporte a *protótipos de função*):

  ```c
  int soma(int a, int b) {
    return a + b;
  }
  ```

### 3.3 Expressões

* **Literais**:

  * Inteiros: decimal, hexadecimal (`0xFF`), octal (`0755`)
  * Reais: `3.14`, `2.5e-3`
  * Caracteres: `'x'`
  * Strings: `"hello"`
* **Identificadores** e **chamadas de função**
* **Operadores unários**: `!`, `++`/`--` (prefixo e pós-fixado)
* **Operadores binários**:

  * Aritméticos: `+`, `-`, `*`, `/`, `%`
  * Relacionais: `==`, `!=`, `<`, `<=`, `>`, `>=`
  * Lógicos: `&&`, `||`
* **Atribuições**: simples (`=`) e compostas (`+=`, `-=`, `*=`, `/=`)
* **Acesso a campo** de struct: `expr.identificador` e atribuição de campo
* **Acesso a array**: `expr[expr]`
* **Listas de inicialização** (arrays e structs)
* **printf**

### 3.4 Comandos de Controle

O compilador espera a sintaxe convencional dessas construções, seguindo o estilo tradicional da linguagem C:

* **Blocos**: delimitados por { ... }, são usados para definir escopos explícitos dentro de funções, estruturas condicionais e laços.

* **Condicionais**: devem ser escritas na forma `if (condição) { bloco }` ou `if (condição) { bloco } else { bloco }`. O uso de blocos com chaves é obrigatório para manter a estrutura.

* **Laços de repetição**:

  - `while (condição) { bloco }` — enquanto a condição for verdadeira, o bloco é executado.

  - `for (inicialização; condição; incremento) { bloco }` — todos os três campos da cláusula for devem estar presentes.


* **Controle de laço**: os comandos `break;` e `continue;` devem ser usados dentro de `for`, `while` ou `switch`.

* **Comando switch**: suporta múltiplos case com valores constantes inteiros, e um default opcional. Cada case deve terminar com `break;`, ou o compilador assumirá continuidade.

* **Retorno**: `return expr`; deve ser usado em funções que retornam valores, e `return;` em funções `void`.


## 4. Sintaxes de C **não suportadas**

* **Ponteiros** (`*`, `&`, aritmética de ponteiros\`)
* **Definição de structs** (estrutura interna: `struct Nome { … };`)
* **Typedef**, **enum**, **union**
* **Prototypes isolados** (declarações de função sem definição)
* **do … while**
* **scanf**
* **Macros complexas** (somente `#define ID valor` simples é reconhecido; argumentos de macro não)
* **Operador ternário** `?:` (token reconhecido, mas sem produção no parser)
* **Qualificadores** (`static`, `extern`, `volatile`, `signed`, `unsigned`, `long`, `short`) são aceitos no léxico mas *apenas* repassados para `type_specifier` sem semântica completa
* **Directivas** além de `#include` e `#define` (outras são ignoradas)

## 5. Limitações e Observações

* **Escopo**: pilha de escopos simplificada (criação/destruição em blocos, mas sem pilha completa de tabelas)
* **Sem** suporte a alocação dinâmica (`malloc`, `free`) ou manipulação de ponteiros
* **Tratamento** de erros sintáticos e semânticos via `yyerror` e nós `NODE_ERROR`
* **Geração de IR** é uma `char*` — internamente, o TS é produzido concatenando strings; não há otimizações
* **Geração de TypeScript** gera funções e variáveis globais, mas pode não mapear perfeitamente certos padrões C
* **Strings** são convertidas em arrays de `char`/`string` do TS, mas sem métodos de biblioteca

## 6. Exemplo de Conversão

**Código em C:**

```c
#include <stdio.h>

int soma(int a, int b) {
  int resultado = a + b;
  return resultado;
}

int main() {
  int x = 2;
  int y = 3;
  printf("Soma: %d\n", soma(x, y));
  return 0;
}
```

**Resultado em TypeScript:**

```ts
function soma(a: number, b: number): number {
  let resultado: number = a + b;
  return resultado;
}

function main(): number {
  const x: number = 2;
  const y: number = 3;
  console.log(`Soma: ${soma(x, y)}`);
  return 0;
}

// Chamando a função principal
main();
```
## 📑 Histórico de Versões

| Versão | Descrição | Autor(es) | Data de Produção | Revisor(es) | Data de Revisão | 
| :----: | --------- | --------- | :--------------: | ----------- | :-------------: |
| `1.0` | Versão inicial da documentação | [Israel Thalles](https://github.com/IsraelThalles) | 01/05/2025 | - | - |
| `1.1` | Atualização da documentação | [Israel Thalles](https://github.com/IsraelThalles) | 27/06/2025 |  |  |

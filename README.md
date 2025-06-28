# TypeC

Repositório destinado ao projeto da disciplina de Compiladores.

## 💡 Sobre

O projeto **TypeC** tem como objetivo desenvolver um compilador capaz de traduzir código escrito em C para TypeScript. A ideia é facilitar a integração entre uma linguagem tradicional e de baixo nível, como o C, com uma linguagem moderna e fortemente tipada do ecossistema JavaScript. Com isso, o TypeC busca tornar mais simples a reutilização de algoritmos antigos ou performáticos, escritos em C, em aplicações web atuais, ajudando a conectar diferentes tecnologias e facilitar a comunicação entre elas.

## 🚀 Como funciona o compilador?

O TypeC realiza as seguintes etapas:

1. **Análise Léxica e Sintática:** Utiliza Flex e Bison para construir a árvore sintática abstrata (AST) a partir do código C de entrada.
2. **Análise Semântica:** Verifica tipos, escopos e declarações, garantindo que o código C seja válido e seguro para tradução.
3. **Geração de Código TypeScript:** Percorre a AST e gera um arquivo `.ts` equivalente, respeitando a ordem dos comandos, estruturas de controle, declarações, structs, arrays e funções.

O compilador suporta conversão de variáveis, funções, laços (`for`, `while`), condicionais (`if`, `switch`), structs, arrays e até mesmo a tradução de comandos `printf` para `console.log` com template strings.

## 🛠️ Como rodar o projeto

### Pré-requisitos

- GCC (compilador C)
- Flex e Bison instalados
- Linux (ou ambiente compatível)

### Passos para compilar e testar

1. **Clone o repositório:**

   ```sh
   git clone https://github.com/seu-usuario/projeto-compiladores-25.1.git
   cd projeto-compiladores-25.1
   ```

2. **Compile o compilador:**

   ```sh
   make
   ```

   2.1. **(Opcional) Limpeza de arquivos temporários:**

   ```sh
   make clean
   ```

   Isso remove os arquivos temporários e de compilação, mantendo apenas os fontes e o executável principal.

   2.2 **(Opcional) Executar testes sem converter para TypeScript:**

   ```sh
   make test
   ```

   Isso mostra no terminal o resultado da compilação e os erros encontrados, se houver.

3. **Execute os testes automáticos:**

   ```sh
   ./run-tests.sh
   ```

   Os resultados dos testes ficam em `tests/<nome_do_teste>/output.ts` e são comparados com `expected.ts`.

4. **Compile um arquivo C manualmente:**
   ```sh
   ./parser tests/teste3/input.c tests/teste3/output.ts
   ```
   O arquivo TypeScript gerado estará em `output.ts`.

## 📚 Exemplo de uso

Entrada C:

```c
int main() {
    int x = 10;
    int y = 20;
    int sum = x + y;
    printf("Soma: %d\n", sum);
    return 0;
}
```

Saída TypeScript:

```typescript
function main(): number {
  let x: number = 10;
  let y: number = 20;
  let sum: number = x + y;
  console.log(`Soma: ${sum}`);
  return 0;
}
```

## 👥 Integrantes

| Matrícula | Aluno                                                               | 📷                                                                                         |
| --------- | ------------------------------------------------------------------- | ------------------------------------------------------------------------------------------ |
| 211061331 | [Alana Gabriele Amorim Silva](https://github.com/alanagabriele)     | <img width="100" src="https://github.com/alanagabriele.png" style="border-radius: 50px"/>  |
| 211061707 | [Felipe de Sousa Coelho](https://github.com/fsousac)                | <img width="100" src="https://github.com/fsousac.png" style="border-radius: 50px"/>        |
| 190014776 | [Israel Thalles Dutra dos Santos](https://github.com/IsraelThalles) | <img width="100" src="https://github.com/IsraelThalles.png" style="border-radius: 50px"/>  |
| 211062437 | [Raquel Ferreira Andrade](https://github.com/raquel-andrade)        | <img width="100" src="https://github.com/raquel-andrade.png" style="border-radius: 50px"/> |

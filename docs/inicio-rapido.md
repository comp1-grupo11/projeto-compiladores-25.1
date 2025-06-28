# Início rápido

## 🐧 Instruções para Usuários Linux
Para compilar e testar o seu compilador C → TypeScript, utilize os seguintes comandos no terminal:

1. Construir o compilador (gera o executável atualmente chamado 'parser')
```bash
make
```

2. Rodar todos os testes automatizados
```bash
make tests
```

3. Limpar arquivos gerados (executável, IR, logs etc.)
```bash
make clear
```

4. Executar script completo de testes, gerando diff.txt e compiler.log
```bash
./run-tests.sh
```

5. Testar um arquivo C específico
```bash
./parser tests/"testeX"/input.c  # substitua "testeX" pelo diretório do teste desejado
```

## 🪟 Instruções para Usuários Windows
O Makefile fornecido é compatível com sistemas Unix/Linux. Para rodar o compilador no Windows, você pode seguir as etapas manuais abaixo.

### ✅ Pré-requisitos
Instale o MinGW ou MSYS2 (recomendado: MSYS2)

https://www.msys2.org/

- Instale os seguintes pacotes:
```sh
pacman -Syu
pacman -S gcc flex bison make
```
*Alternativa: use o WSL (Windows Subsystem for Linux) se preferir ambiente Linux puro.*


### ⚙️ Etapas de Compilação Manual (sem Makefile)
- Gere o analisador sintático com Bison
```sh
bison -d parser.y
```
Isso cria:

  - parser.tab.c

  - parser.tab.h

- Gere o analisador léxico com Flex
```sh
flex lexico.l
```
Isso cria:

  - lex.yy.c

- Compile todos os arquivos .c
```sh
gcc -Wall -Wextra -g -Wno-unused-function -c parser.tab.c
gcc -Wall -Wextra -g -Wno-unused-function -c lex.yy.c
gcc -Wall -Wextra -g -Wno-unused-function -c ast.c
gcc -Wall -Wextra -g -Wno-unused-function -c tabela.c
gcc -Wall -Wextra -g -Wno-unused-function -c gerador.c
```

- Link todos os arquivos objeto e gere o executável
```sh
gcc *.o -o parser -lfl
```
Isso gera o executável parser.exe

### 🧪 Como executar os testes

- Rodar um teste específico:
```sh
./parser tests/teste1/input.c
```

## 📑 Histórico de Versões

| Versão | Descrição | Autor(es) | Data de Produção | Revisor(es) | Data de Revisão | 
| :----: | --------- | --------- | :--------------: | ----------- | :-------------: |
| `1.0` | Versão inicial da documentação | [Israel Thalles](https://github.com/IsraelThalles) | 27/06/2025 |  |  |

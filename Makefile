# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Wno-unused-function
FLEX = flex
BISON = bison
LDFLAGS = -L/opt/homebrew/opt/flex/lib

# Nome do executável
TARGET = parser

# Arquivos fonte
SRC = parser.tab.c lex.yy.c ast.c tabela.c gerador.c gerador_ts.c
OBJ = $(SRC:.c=.o)

# Diretórios
BUILD_DIR = build
TEST_DIR = tests

# Arquivos de teste
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

.PHONY: all clear test

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "\n🔗 Ligando objetos..."
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ -lfl
	@echo "\n✅ Compilador construído: \033[1;32m$(TARGET)\033[0m\n"

parser.tab.c parser.tab.h: parser.y ast.h
	@echo "\n🔄 Gerando parser com Bison..."
	$(BISON) -d parser.y

lex.yy.c: lexico.l parser.tab.h
	@echo "\n🔄 Gerando lexer com Flex..."
	$(FLEX) lexico.l

%.o: %.c
	@echo "🔨 Compilando $<"
	$(CC) $(CFLAGS) -c $< -o $@

clear:
	@echo "\n🧹 Limpando arquivos gerados..."
	rm -f $(TARGET) *.o *.output
	rm -f parser.tab.* lex.yy.c

test: $(TARGET)
	@echo "\n🔍 Iniciando testes..."
	@for test in $(TEST_FILES); do \
		echo "\n🔬 Testando $$test:"; \
        if ./$(TARGET) $$test; then \
            echo "✅ Teste concluído com sucesso: $$test"; \
        else \
            echo "❌ Falha no teste $$test"; \
        fi \
	done
	@echo "\n🏁 Todos os testes concluídos\n"

ir:
	@echo "🚧 Gerando código intermediário..."
	./parser $(FILE) > output.ts
	@echo "\n📝 Código intermediário gerado em \033[1;36moutput.ts\033[0m\n"

# Dependências especiais
lex.yy.o: lex.yy.c parser.tab.h
parser.tab.o: parser.tab.c parser.tab.h
ast.o: ast.c ast.h
tabela.o: tabela.c tabela.h
gerador.o: gerador.c ast.h
gerador_ts.o: gerador_ts.c ast.h
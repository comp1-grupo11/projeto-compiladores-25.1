# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -g
FLEX = flex
BISON = bison

# Nome do executável
TARGET = parser

# Arquivos fonte
SRC = parser.tab.c lex.yy.c ast.c
OBJ = $(SRC:.c=.o)

# Diretório de saída (opcional)
BUILD_DIR = build

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "\n🔗 Ligando objetos..."
	$(CC) $(CFLAGS) $^ -o $@ -lfl
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

clean:
	@echo "\n🧹 Limpando arquivos gerados..."
	rm -f $(TARGET) *.o *.output
	rm -f parser.tab.* lex.yy.c

test: $(TARGET)
	@echo "\n🔍 Iniciando testes..."
	@for test in teste1.c teste2.c teste3.c; do \
		echo "\n🔬 Testando $$test:"; \
		./$(TARGET) $$test || echo "❌ Falha no teste $$test"; \
	done
	@echo "\n🏁 Todos os testes concluídos\n"

# Dependências especiais
lex.yy.o: lex.yy.c parser.tab.h ast.h
parser.tab.o: parser.tab.c parser.tab.h ast.h
ast.o: ast.c ast.h
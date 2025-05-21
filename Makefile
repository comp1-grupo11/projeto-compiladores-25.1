# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -g
FLEX = flex
BISON = bison

# Nome do executável
TARGET = parser

# Arquivos fonte
SRC = parser.tab.c lex.yy.c ast.c tabela.c
OBJ = $(SRC:.c=.o)

# Diretório de saída (opcional)
BUILD_DIR = build

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "Ligando objetos..."
	$(CC) $(CFLAGS) $^ -o $@ -lfl
	@echo "Compilador construído com sucesso: $(TARGET)"

parser.tab.c parser.tab.h: parser.y
	@echo "Gerando parser com Bison..."
	$(BISON) -d parser.y

lex.yy.c: lexico.l parser.tab.h
	@echo "Gerando lexer com Flex..."
	$(FLEX) lexico.l

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Limpando arquivos gerados..."
	rm -f $(TARGET) *.o *.output
	rm -f parser.tab.* lex.yy.c

test: $(TARGET)
	@echo "Executando testes..."
	@echo "---------------"
	@echo "Teste 1"
	-@./$(TARGET) teste1.c
	@echo "---------------"
	@echo "Teste 2"
	-@./$(TARGET) teste2.c
	@echo "---------------"
	@echo "Teste 3"
	-@./$(TARGET) teste3.c

# Dependências especiais
lex.yy.o: lex.yy.c parser.tab.h
parser.tab.o: parser.tab.c parser.tab.h
ast.o: ast.c ast.h
tabela.o: tabela.c tabela.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylineno; // Para obter o número da linha atual

// Implementação da função auxiliar para alocar um nó base
static NoAST *alocarNoAST(NodeType tipo_no) {
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (tipo %d) na linha %d.\n", tipo_no, yylineno);
        exit(EXIT_FAILURE);
    }
    memset(no, 0, sizeof(NoAST)); // Inicializa todos os campos com zero/NULL
    no->tipo_no = tipo_no;
    no->linha = yylineno;
    return no;
}

// Implementação da função de criação de nó de declaração de variável
NoAST *criarNoDeclaracaoVar(char *nome, Tipo tipo_declarado, NoAST *inicializacao_expr) {
    NoAST *no = alocarNoAST(NODE_DECLARATION);
    no->tipo_dado = tipo_declarado;
    no->data.decl_info.nome_declaracao = strdup(nome);
    if (no->data.decl_info.nome_declaracao == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para nome da variavel '%s' na declaracao na linha %d.\n", nome, yylineno);
        liberarAST(no); // Libera o nó parcialmente alocado
        exit(EXIT_FAILURE);
    }
    no->data.decl_info.inicializacao_expr = inicializacao_expr;
    return no;
}

// Implementação da função de criação de nó de retorno
NoAST *criarNoReturn(NoAST *expr_retorno) {
    NoAST *no = alocarNoAST(NODE_RETURN);
    no->esquerda = expr_retorno; // A expressão de retorno é o filho esquerdo
    no->tipo_dado = (expr_retorno) ? expr_retorno->tipo_dado : TIPO_VOID; // Tipo do retorno
    return no;
}

// Implementação da função de criação de nó if-else
NoAST *criarNoIf(NoAST *condicao, NoAST *bloco_then, NoAST *bloco_else) {
    NoAST *no = alocarNoAST(NODE_IF);
    no->esquerda = condicao;     // A condição é o filho esquerdo
    no->direita = bloco_then;    // O bloco 'then' é o filho direito
    no->centro = bloco_else;     // O bloco 'else' (pode ser NULL)
    no->tipo_dado = TIPO_VOID;   // 'if' geralmente não tem um tipo de dado de retorno
    return no;
}

// Implementação da função de criação de nó while
NoAST *criarNoWhile(NoAST *condicao, NoAST *bloco) {
    NoAST *no = alocarNoAST(NODE_WHILE);
    no->esquerda = condicao; // A condição é o filho esquerdo
    no->direita = bloco;     // O bloco do 'while' é o filho direito
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Implementação da função de criação de nó for
NoAST *criarNoFor(NoAST *init, NoAST *cond, NoAST *iter, NoAST *bloco) {
    NoAST *no = alocarNoAST(NODE_FOR);
    no->esquerda = init;  // Inicialização (pode ser NULL)
    no->direita = cond;   // Condição (pode ser NULL)
    no->centro = iter;    // Iteração (pode ser NULL)
    no->proximo = bloco;  // O bloco do 'for' (usando 'proximo' para o 4º filho)
    no->tipo_dado = TIPO_VOID;
    return no;
}

NoAST *criarNoSwitchBody(NoAST *case_list_head, NoAST *default_node) {
    NoAST *no = alocarNoAST(NODE_SWITCH_BODY);
    no->esquerda = case_list_head; // A cabeça da lista de nós NODE_CASE
    no->direita = default_node;   // O nó NODE_DEFAULT (pode ser NULL)
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Cria um nó para um 'case' individual
NoAST *criarNoCase(NoAST *case_expr, NoAST *statement_list) {
    NoAST *no = alocarNoAST(NODE_CASE);
    no->esquerda = case_expr;      // A expressão do case (ex: 10, 'A')
    no->direita = statement_list; // A lista de comandos dentro do case
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Cria um nó para o 'default'
NoAST *criarNoDefault(NoAST *statement_list) {
    NoAST *no = alocarNoAST(NODE_DEFAULT);
    no->esquerda = statement_list; // A lista de comandos dentro do default
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Função para criar um nó de switch
NoAST *criarNoSwitch(NoAST *expr_controle, NoAST *corpo_switch) {
    NoAST *no = alocarNoAST(NODE_SWITCH);
    no->esquerda = expr_controle;   // A expressão de controle (do parêntese)
    no->direita = corpo_switch; // O nó que representa o corpo do switch (o que 'switch_body' retorna)
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Implementação da função de criação de nó de bloco de comando (compound statement)
NoAST *criarNoCompoundStmt(NoAST *lista_statements) {
    NoAST *no = alocarNoAST(NODE_COMPOUND_STMT);
    no->esquerda = lista_statements; // A cabeça da lista de statements dentro do bloco
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Implementação da função de criação de nó break
NoAST *criarNoBreak() {
    NoAST *no = alocarNoAST(NODE_BREAK);
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Implementação da função de criação de nó continue
NoAST *criarNoContinue() {
    NoAST *no = alocarNoAST(NODE_CONTINUE);
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Implementação da função de criação de nó de chamada de função
NoAST *criarNoChamadaFuncao(char *nome_func, NoAST *args_list) {
    NoAST *no = alocarNoAST(NODE_FUNCTION_CALL);
    no->data.func_name = strdup(nome_func);
    if (no->data.func_name == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para nome da funcao '%s' na chamada na linha %d.\n", nome_func, yylineno);
        liberarAST(no);
        exit(EXIT_FAILURE);
    }
    no->esquerda = args_list; // A lista de argumentos é o filho esquerdo
    no->tipo_dado = TIPO_ERRO; // O tipo real será resolvido na análise semântica
    return no;
}

// Função para criar um nó de operador
NoAST *criarNoOp(OperatorType op, NoAST *esq, NoAST *dir){
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Operador).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_OPERATOR;
    no->data.op_type = op;
    no->esquerda = esq;
    no->direita = dir;
    no->linha = yylineno;

    if (esq == NULL || dir == NULL || esq->tipo_dado == TIPO_ERRO || dir->tipo_dado == TIPO_ERRO) {
        no->tipo_dado = TIPO_ERRO;
    } else {
        // Lógica de inferência de tipo (pode ser mais complexa com coerção)
        no->tipo_dado = (esq->tipo_dado == dir->tipo_dado) ? esq->tipo_dado : TIPO_ERRO;
    }
    return no;
}

NoAST *criarNoUnario(OperatorType op, NoAST *operando) {
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Unario).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_OPERATOR;
    no->data.op_type = op;
    no->esquerda = operando;
    no->direita = NULL;
    no->linha = yylineno;

    if (operando == NULL || operando->tipo_dado == TIPO_ERRO) {
        no->tipo_dado = TIPO_ERRO;
    } else {
        // Lógica de tipo para operadores unários (geralmente mantêm o tipo do operando)
        no->tipo_dado = operando->tipo_dado;
    }
    return no;
}

// Função para criar um nó numérico (INT, FLOAT, DOUBLE)
NoAST *criarNoNum(Tipo tipo, void *valor) {
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Numero).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_LITERAL;
    no->tipo_dado = tipo;
    no->esquerda = NULL;
    no->direita = NULL;
    no->linha = yylineno;

    switch (tipo) {
        case TIPO_INT:
            no->data.literal.val_int = *(int *)valor;
            break;
        case TIPO_FLOAT:
            no->data.literal.val_float = *(float *)valor;
            break;
        case TIPO_DOUBLE:
            no->data.literal.val_double = *(double *)valor;
            break;
        default:
            fprintf(stderr, "Tipo numerico invalido para criarNoNum.\n");
            free(no);
            return NULL; // Deve ser tratado pelo chamador
    }
    return no;
}

// Função para criar um nó de identificador
NoAST *criarNoId(char *nome, Tipo tipo) {
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Identificador).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_IDENTIFIER;
    no->tipo_dado = tipo;
    strncpy(no->data.nome_id, nome, sizeof(no->data.nome_id) - 1);
    no->data.nome_id[sizeof(no->data.nome_id) - 1] = '\0';
    no->esquerda = NULL;
    no->direita = NULL;
    no->linha = yylineno;
    return no;
}

// Função para criar um nó de caractere
NoAST *criarNoChar(char val) {
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Char).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_LITERAL;
    no->tipo_dado = TIPO_CHAR;
    no->data.literal.val_char = val;
    no->esquerda = NULL;
    no->direita = NULL;
    no->linha = yylineno;
    return no;
}

// Função para criar um nó de string
NoAST *criarNoString(char *val) {
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (String).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_LITERAL;
    no->tipo_dado = TIPO_STRING;
    no->data.literal.val_string = strdup(val);
    if (no->data.literal.val_string == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para string literal.\n");
        free(no);
        exit(EXIT_FAILURE);
    }
    no->esquerda = NULL;
    no->direita = NULL;
    no->linha = yylineno;
    return no;
}

// Função para criar um nó de erro
NoAST *criarNoErro() {
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Erro).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_ERROR;
    no->tipo_dado = TIPO_ERRO;
    no->esquerda = NULL;
    no->direita = NULL;
    no->linha = yylineno;
    return no;
}

void imprimirAST(NoAST *no) {
    if (no == NULL) {
        return;
    }

    switch (no->tipo_no) {
        case NODE_OPERATOR:
            if (no->data.op_type == OP_NOT_TYPE || no->data.op_type == OP_INC_TYPE || no->data.op_type == OP_DEC_TYPE) {
                printf("(%s", 
                    (no->data.op_type == OP_NOT_TYPE) ? "!" :
                    (no->data.op_type == OP_INC_TYPE) ? "++" :
                    (no->data.op_type == OP_DEC_TYPE) ? "--" : "?");
                imprimirAST(no->esquerda);
                printf(")");
            } else {
                printf("(");
                imprimirAST(no->esquerda);
                printf(" %s ",
                       (no->data.op_type == OP_ADD_TYPE) ? "+" :
                       (no->data.op_type == OP_SUB_TYPE) ? "-" :
                       (no->data.op_type == OP_MUL_TYPE) ? "*" :
                       (no->data.op_type == OP_DIV_TYPE) ? "/" :
                       (no->data.op_type == OP_ASSIGN_TYPE) ? "=" :
                       (no->data.op_type == OP_EQ_TYPE) ? "==" :
                       (no->data.op_type == OP_NE_TYPE) ? "!=" :
                       (no->data.op_type == OP_LT_TYPE) ? "<" :
                       (no->data.op_type == OP_LE_TYPE) ? "<=" :
                       (no->data.op_type == OP_GT_TYPE) ? ">" :
                       (no->data.op_type == OP_GE_TYPE) ? ">=" :
                       (no->data.op_type == OP_AND_TYPE) ? "&&" :
                       (no->data.op_type == OP_OR_TYPE) ? "||" :
                       (no->data.op_type == OP_ADD_ASSIGN_TYPE) ? "+=" :
                       (no->data.op_type == OP_SUB_ASSIGN_TYPE) ? "-=" :
                       (no->data.op_type == OP_MUL_ASSIGN_TYPE) ? "*=" :
                       (no->data.op_type == OP_DIV_ASSIGN_TYPE) ? "/=" :
                       "?");
                imprimirAST(no->direita);
                printf(")");
            }
            break;

        case NODE_LITERAL:
            switch (no->tipo_dado) {
                case TIPO_INT:
                    printf("%d", no->data.literal.val_int);
                    break;
                case TIPO_FLOAT:
                    printf("%f", no->data.literal.val_float);
                    break;
                case TIPO_DOUBLE:
                    printf("%lf", no->data.literal.val_double);
                    break;
                case TIPO_CHAR:
                    printf("'%c'", no->data.literal.val_char);
                    break;
                case TIPO_STRING:
                    printf("\"%s\"", no->data.literal.val_string);
                    break;
                default:
                    printf("[LITERAL DESCONHECIDO]");
                    break;
            }
            break;

        case NODE_IDENTIFIER:
            printf("%s", no->data.nome_id);
            break;

        case NODE_DECLARATION:
            printf("%s %s", 
                (no->tipo_dado == TIPO_INT) ? "int" :
                (no->tipo_dado == TIPO_FLOAT) ? "float" :
                (no->tipo_dado == TIPO_DOUBLE) ? "double" :
                (no->tipo_dado == TIPO_CHAR) ? "char" :
                (no->tipo_dado == TIPO_STRING) ? "string" :
                "tipo_desconhecido",
                no->data.decl_info.nome_declaracao);
            if (no->data.decl_info.inicializacao_expr) {
                printf(" = ");
                imprimirAST(no->data.decl_info.inicializacao_expr);
            }
            printf(";");
            break;

        case NODE_RETURN:
            printf("return ");
            imprimirAST(no->esquerda);
            printf(";");
            break;

        case NODE_IF:
            printf("if (");
            imprimirAST(no->esquerda);
            printf(") ");
            imprimirAST(no->direita);
            if (no->centro) {
                printf(" else ");
                imprimirAST(no->centro);
            }
            break;

        case NODE_WHILE:
            printf("while (");
            imprimirAST(no->esquerda);
            printf(") ");
            imprimirAST(no->direita);
            break;

        case NODE_FOR:
            printf("for (");
            if (no->esquerda) imprimirAST(no->esquerda);
            printf("; ");
            if (no->direita) imprimirAST(no->direita);
            printf("; ");
            if (no->centro) imprimirAST(no->centro);
            printf(") ");
            if (no->proximo) imprimirAST(no->proximo);
            break;

        case NODE_COMPOUND_STMT:
            printf("{\n");
            imprimirAST(no->esquerda);
            printf("}\n");
            break;

        case NODE_BREAK:
            printf("break;");
            break;

        case NODE_CONTINUE:
            printf("continue;");
            break;

        case NODE_FUNCTION_CALL:
            printf("%s(", no->data.func_name);
            imprimirAST(no->esquerda);
            printf(")");
            break;

        case NODE_SWITCH:
            printf("switch (");
            imprimirAST(no->esquerda);
            printf(") ");
            imprimirAST(no->direita);
            break;

        case NODE_SWITCH_BODY:
            imprimirAST(no->esquerda); 
            if (no->direita) {         
                imprimirAST(no->direita);
            }
            break;

        case NODE_CASE:
            printf("case ");
            imprimirAST(no->esquerda);
            printf(": ");
            imprimirAST(no->direita);
            break;

        case NODE_DEFAULT:
            printf("default: ");
            imprimirAST(no->esquerda);
            break;

        case NODE_ERROR:
            printf("[NÓ DE ERRO]");
            break;

        default:
            printf("[TIPO DE NÓ DESCONHECIDO]");
            break;
    }
}


void liberarAST(NoAST *no) {
    if (no == NULL) return;

    switch (no->tipo_no) {
        case NODE_DECLARATION:
            free(no->data.decl_info.nome_declaracao);
            liberarAST(no->data.decl_info.inicializacao_expr);
            break;

        case NODE_FUNCTION_CALL:
            free(no->data.func_name);
            break;

        case NODE_LITERAL:
            if (no->tipo_dado == TIPO_STRING && no->data.literal.val_string != NULL) {
                free(no->data.literal.val_string);
            }
            break;

        default:
            break;
    }

    liberarAST(no->esquerda);
    liberarAST(no->direita);
    liberarAST(no->centro);
    liberarAST(no->proximo);

    free(no);
}

int tiposCompativeis(Tipo t1, Tipo t2) {
    return t1 == t2;
}
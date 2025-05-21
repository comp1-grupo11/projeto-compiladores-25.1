#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h" // Inclui o ast.h atualizado

extern int yylineno; // Para obter o número da linha atual

// ... (Suas implementações existentes de criarNoOp, criarNoUnario, criarNoNum,
// criarNoId, criarNoChar, criarNoString, criarNoErro) ...
// Certifique-se de que todas recebem 'linha' e atribuem a 'no->linha'

// Implementação da função auxiliar para alocar um nó base
static NoAST *alocarNoAST(NodeType tipo_no, int linha)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (tipo %d) na linha %d.\n", tipo_no, linha);
        exit(EXIT_FAILURE);
    }
    memset(no, 0, sizeof(NoAST)); // Inicializa todos os campos com zero/NULL
    no->tipo_no = tipo_no;
    no->linha = linha;
    return no;
}

// Implementação da função de criação de nó de declaração de variável (atualizada)
NoAST *criarNoDeclaracaoVar(char *nome, Tipo tipo_declarado, NoAST *inicializacao_expr, int linha)
{
    NoAST *no = alocarNoAST(NODE_DECLARATION, linha);
    no->tipo_dado = tipo_declarado;
    no->data.decl_info.nome_declaracao = strdup(nome);
    if (no->data.decl_info.nome_declaracao == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para nome da variavel '%s' na declaracao na linha %d.\n", nome, linha);
        liberarAST(no); // Libera o nó parcialmente alocado
        exit(EXIT_FAILURE);
    }
    no->data.decl_info.inicializacao_expr = inicializacao_expr;
    return no;
}

// Implementação da função de criação de nó de retorno
NoAST *criarNoReturn(NoAST *expr_retorno, int linha)
{
    NoAST *no = alocarNoAST(NODE_RETURN, linha);
    no->esquerda = expr_retorno;                                          // A expressão de retorno é o filho esquerdo
    no->tipo_dado = (expr_retorno) ? expr_retorno->tipo_dado : TIPO_VOID; // Tipo do retorno
    return no;
}

// Implementação da função de criação de nó if-else
NoAST *criarNoIf(NoAST *condicao, NoAST *bloco_then, NoAST *bloco_else, int linha)
{
    NoAST *no = alocarNoAST(NODE_IF, linha);
    no->esquerda = condicao;   // A condição é o filho esquerdo
    no->direita = bloco_then;  // O bloco 'then' é o filho direito
    no->centro = bloco_else;   // O bloco 'else' (pode ser NULL)
    no->tipo_dado = TIPO_VOID; // 'if' geralmente não tem um tipo de dado de retorno
    return no;
}

// Implementação da função de criação de nó while
NoAST *criarNoWhile(NoAST *condicao, NoAST *bloco, int linha)
{
    NoAST *no = alocarNoAST(NODE_WHILE, linha);
    no->esquerda = condicao; // A condição é o filho esquerdo
    no->direita = bloco;     // O bloco do 'while' é o filho direito
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Implementação da função de criação de nó for
NoAST *criarNoFor(NoAST *init, NoAST *cond, NoAST *iter, NoAST *bloco, int linha)
{
    NoAST *no = alocarNoAST(NODE_FOR, linha);
    no->esquerda = init; // Inicialização (pode ser NULL)
    no->direita = cond;  // Condição (pode ser NULL)
    no->centro = iter;   // Iteração (pode ser NULL)
    no->proximo = bloco; // O bloco do 'for' (usando 'proximo' para o 4º filho)
    no->tipo_dado = TIPO_VOID;
    return no;
}

NoAST *criarNoSwitchBody(NoAST *case_list_head, NoAST *default_node, int linha)
{
    NoAST *no = alocarNoAST(NODE_SWITCH_BODY, linha);
    no->esquerda = case_list_head; // A cabeça da lista de nós NODE_CASE
    no->direita = default_node;    // O nó NODE_DEFAULT (pode ser NULL)
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Cria um nó para um 'case' individual
NoAST *criarNoCase(NoAST *case_expr, NoAST *statement_list, int linha)
{
    NoAST *no = alocarNoAST(NODE_CASE, linha);
    no->esquerda = case_expr;     // A expressão do case (ex: 10, 'A')
    no->direita = statement_list; // A lista de comandos dentro do case
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Cria um nó para o 'default'
NoAST *criarNoDefault(NoAST *statement_list, int linha)
{
    NoAST *no = alocarNoAST(NODE_DEFAULT, linha);
    no->esquerda = statement_list; // A lista de comandos dentro do default
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Sua função criarNoSwitch existente (provavelmente já implementada, mas aqui para contexto)
NoAST *criarNoSwitch(NoAST *expr_controle, NoAST *corpo_switch, int linha)
{
    NoAST *no = alocarNoAST(NODE_SWITCH, linha);
    no->esquerda = expr_controle; // A expressão de controle (do parêntese)
    no->direita = corpo_switch;   // O nó que representa o corpo do switch (o que 'switch_body' retorna)
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Implementação da função de criação de nó de bloco de comando (compound statement)
NoAST *criarNoCompoundStmt(NoAST *lista_statements, int linha)
{
    NoAST *no = alocarNoAST(NODE_COMPOUND_STMT, linha);
    no->esquerda = lista_statements; // A cabeça da lista de statements dentro do bloco
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Implementação da função de criação de nó break
NoAST *criarNoBreak(int linha)
{
    NoAST *no = alocarNoAST(NODE_BREAK, linha);
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Implementação da função de criação de nó continue
NoAST *criarNoContinue(int linha)
{
    NoAST *no = alocarNoAST(NODE_CONTINUE, linha);
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Implementação da função de criação de nó de chamada de função
NoAST *criarNoChamadaFuncao(char *nome_func, NoAST *args_list, int linha)
{
    NoAST *no = alocarNoAST(NODE_FUNCTION_CALL, linha);
    no->data.func_name = strdup(nome_func);
    if (no->data.func_name == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para nome da funcao '%s' na chamada na linha %d.\n", nome_func, linha);
        liberarAST(no);
        exit(EXIT_FAILURE);
    }
    no->esquerda = args_list;  // A lista de argumentos é o filho esquerdo
    no->tipo_dado = TIPO_ERRO; // O tipo real será resolvido na análise semântica
    return no;
}

// Função para criar um nó de operador
NoAST *criarNoOp(OperatorType op, NoAST *esq, NoAST *dir, int linha_atual)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Operador).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_OPERATOR;
    no->data.op_type = op; // ATRIBUI O ENUM AQUI
    no->esquerda = esq;
    no->direita = dir;
    no->linha = linha_atual;

    if (esq == NULL || dir == NULL || esq->tipo_dado == TIPO_ERRO || dir->tipo_dado == TIPO_ERRO)
    {
        no->tipo_dado = TIPO_ERRO;
    }
    else
    {
        // Lógica de inferência de tipo (pode ser mais complexa com coerção)
        no->tipo_dado = (esq->tipo_dado == dir->tipo_dado) ? esq->tipo_dado : TIPO_ERRO;
    }
    return no;
}

NoAST *criarNoUnario(OperatorType op, NoAST *operando, int linha_atual)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Unario).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_OPERATOR; // Ainda é um operador
    no->data.op_type = op;       // Tipo do operador (INC, DEC, NOT)
    no->esquerda = operando;     // O operando é o filho esquerdo
    no->direita = NULL;          // Operadores unários não têm filho direito
    no->linha = linha_atual;

    if (operando == NULL || operando->tipo_dado == TIPO_ERRO)
    {
        no->tipo_dado = TIPO_ERRO;
    }
    else
    {
        // Lógica de tipo para operadores unários (geralmente mantêm o tipo do operando)
        no->tipo_dado = operando->tipo_dado;
    }
    return no;
}

// Função para criar um nó numérico (INT, FLOAT, DOUBLE)
NoAST *criarNoNum(Tipo tipo, void *valor, int yylineno)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Numero).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_LITERAL;
    no->tipo_dado = tipo;
    no->esquerda = NULL;
    no->direita = NULL;
    no->linha = yylineno;

<<<<<<< HEAD
    switch (tipo)
    {
    case TIPO_INT:
        novoNo->valor_numerico.val_int = *(int *)valor;
        break;
    case TIPO_FLOAT:
        novoNo->valor_numerico.val_float = *(float *)valor;
        break;
    case TIPO_DOUBLE:
        novoNo->valor_numerico.val_double = *(double *)valor;
        break;
    default:
        fprintf(stderr, "Tipo numerico invalido para criarNoNum.\n");
        free(novoNo);
        return NULL;
    }
    return novoNo;
}

NoAST *criarNoId(char *nome, Tipo tipo)
{
    NoAST *no = malloc(sizeof(NoAST));
    strcpy(no->nome, nome);
    no->operador = 0;
    no->tipo = tipo;
    no->esquerda = no->direita = NULL;
    return no;
}

void imprimirAST(NoAST *no)
{
    if (no == NULL)
    {
        return;
    }

    if (no->operador != '\0')
    {
        printf("(%c ", no->operador);
        imprimirAST(no->esquerda);
        printf(" ");
        imprimirAST(no->direita);
        printf(")");
    }
    else if (no->nome[0] != '\0')
    {
        printf("%s", no->nome);
    }
    else
    {
        switch (no->tipo)
        {
        case TIPO_INT:
            printf("%d", no->valor_numerico.val_int);
            break;
        case TIPO_FLOAT:
            printf("%f", no->valor_numerico.val_float);
            break;
        case TIPO_DOUBLE:
            printf("%lf", no->valor_numerico.val_double);
            break;
        default:
            printf("[ERRO TIPO NUMERICO]");
            break;
        }
    }
}

int tiposCompativeis(Tipo t1, Tipo t2)
{
=======
    switch (tipo)
    {
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
NoAST *criarNoId(char *nome, Tipo tipo, int yylineno)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Identificador).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_IDENTIFIER;
    no->tipo_dado = tipo;
    // Copia o nome para o campo nome_id
    strncpy(no->data.nome_id, nome, sizeof(no->data.nome_id) - 1);
    no->data.nome_id[sizeof(no->data.nome_id) - 1] = '\0'; // Garante terminação nula
    no->esquerda = NULL;
    no->direita = NULL;
    no->linha = yylineno;
    return no;
}

// NOVA FUNÇÃO: Criar nó para literal de caractere
NoAST *criarNoChar(char val, int yylineno)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
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

// NOVA FUNÇÃO: Criar nó para literal de string
NoAST *criarNoString(char *val, int yylineno)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (String).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_LITERAL;
    no->tipo_dado = TIPO_STRING;
    // Duplica a string para que o nó tenha sua própria cópia
    no->data.literal.val_string = strdup(val);
    if (no->data.literal.val_string == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para string literal.\n");
        free(no);
        exit(EXIT_FAILURE);
    }
    no->esquerda = NULL;
    no->direita = NULL;
    no->linha = yylineno;
    return no;
}

// NOVA FUNÇÃO: Criar nó de erro
NoAST *criarNoErro(int yylineno)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
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

// Função para imprimir a AST (adaptada para a nova estrutura)
void imprimirAST(NoAST *no)
{
    if (no == NULL)
    {
        return;
    }

    switch (no->tipo_no)
    {
    case NODE_OPERATOR:
        printf("(");
        imprimirAST(no->esquerda);
        printf(" %s ", // Agora vamos imprimir o nome do operador
               (no->data.op_type == OP_ADD_TYPE) ? "+" : (no->data.op_type == OP_SUB_TYPE) ? "-"
                                                     : (no->data.op_type == OP_MUL_TYPE) ? "*"
                                                     : (no->data.op_type == OP_DIV_TYPE) ? "/"
                                                     : (no->data.op_type == OP_ASSIGN_TYPE) ? "="
                                                     : (no->data.op_type == OP_EQ_TYPE) ? "=="
                                                     : (no->data.op_type == OP_NE_TYPE) ? "!="
                                                     : (no->data.op_type == OP_LT_TYPE) ? "<"
                                                     : (no->data.op_type == OP_LE_TYPE) ? "<="
                                                     : (no->data.op_type == OP_GT_TYPE) ? ">"
                                                     : (no->data.op_type == OP_GE_TYPE) ? ">="
                                                     : (no->data.op_type == OP_AND_TYPE) ? "&&"
                                                     : (no->data.op_type == OP_OR_TYPE) ? "||"
                                                     : (no->data.op_type == OP_NOT_TYPE) ? "!"
                                                     : (no->data.op_type == OP_INC_TYPE) ? "++"
                                                     : (no->data.op_type == OP_DEC_TYPE) ? "--"
                                                     : (no->data.op_type == OP_ADD_ASSIGN_TYPE) ? "+="
                                                                                                : // Adicione todos
                                                         (no->data.op_type == OP_SUB_ASSIGN_TYPE) ? "-="
                                                     : (no->data.op_type == OP_MUL_ASSIGN_TYPE) ? "*="
                                                     : (no->data.op_type == OP_DIV_ASSIGN_TYPE) ? "/="
                                                                                                : "[OP_UNKNOWN]"); // Um valor padrão para operadores não tratados
        imprimirAST(no->direita);                                                                                  // Para unários, direita é NULL, não será impresso.
        printf(")");
        break;
    case NODE_LITERAL:
        switch (no->tipo_dado)
        {
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
            break; // Adicione para tipos não cobertos
        }
        break;
    case NODE_IDENTIFIER:
        printf("%s", no->data.nome_id);
        break;
    case NODE_ERROR:
        printf("[NÓ DE ERRO]");
        break;
    default:
        printf("[TIPO DE NÓ DESCONHECIDO]");
        break;
    }
}

// NOVA FUNÇÃO: Liberar a memória da AST
// ... (Atualize liberarAST para lidar com os novos tipos de nó e campos 'proximo', 'func_name', 'decl_info.nome_declaracao') ...
void liberarAST(NoAST *no)
{
    if (no == NULL)
    {
        return;
    }

    // Libera os filhos (esquerda, direita, centro, proximo) recursivamente
    liberarAST(no->esquerda);
    liberarAST(no->direita);
    liberarAST(no->centro);
    liberarAST(no->proximo); // Libera o próximo nó na lista

    // Libera memória alocada dinamicamente para campos específicos
    if (no->tipo_no == NODE_LITERAL && no->tipo_dado == TIPO_STRING && no->data.literal.val_string)
    {
        free(no->data.literal.val_string);
    }
    else if (no->tipo_no == NODE_DECLARATION && no->data.decl_info.nome_declaracao)
    {
        free(no->data.decl_info.nome_declaracao);
    }
    else if (no->tipo_no == NODE_FUNCTION_CALL && no->data.func_name)
    {
        free(no->data.func_name);
    }
    // Adicione aqui outros free() se tiver campos alocados dinamicamente em outros tipos de nó

    free(no);
}

// Função para verificar compatibilidade de tipos
int tiposCompativeis(Tipo t1, Tipo t2)
{
    // Lógica de compatibilidade de tipos.
    // Por exemplo, int é compatível com float (com coerção), mas float não é com char.
    // Para um compilador simples, a igualdade é um bom ponto de partida.
>>>>>>> 1bf1ea1 (Implementação do analisador semântico)
    return t1 == t2;
}
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTNode *create_function_node(char *name, ASTNode *params, ASTNode *body)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNCTION;
    node->data_type = DT_VOID;
    node->type_name = NULL;

    node->function.name = strdup(name);
    node->function.params = params;
    node->function.body = body;

    return node;
}

ASTNode *create_struct_def_node(char *name, ASTNode *fields)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_STRUCT_DEF;
    node->data_type = DT_STRUCT;
    node->type_name = strdup(name);

    node->struct_def.name = strdup(name);
    node->struct_def.fields = fields;

    return node;
}

ASTNode *create_variable_node(char *name, DataType dtype, char *type_name, ASTNode *init_value)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_VARIABLE_DECL;
    node->data_type = dtype;
    node->type_name = type_name ? strdup(type_name) : NULL;

    node->variable.name = strdup(name);
    node->variable.init_value = init_value;

    return node;
}

ASTNode *create_literal_number(int value)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_LITERAL;
    node->data_type = DT_NUMBER;
    node->type_name = NULL;

    node->literal.int_val = value;
    return node;
}

ASTNode *create_literal_string(char *value)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_LITERAL;
    node->data_type = DT_STRING;
    node->type_name = NULL;

    node->literal.str_val = strdup(value);
    return node;
}

ASTNode *create_binop_node(char *op, ASTNode *left, ASTNode *right)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_BINOP;
    node->data_type = left->data_type; // Herda tipo do operando esquerdo
    node->type_name = NULL;

    node->binop.op = strdup(op);
    node->binop.left = left;
    node->binop.right = right;

    return node;
}

ASTNode *create_call_node(char *func_name, ASTNode *args)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_CALL;
    node->data_type = DT_VOID; // Assume void por padrão
    node->type_name = NULL;

    node->call.func_name = strdup(func_name);
    node->call.args = args;

    return node;
}

ASTNode *create_switch_node(ASTNode *expr, ASTNode *cases, ASTNode *default_case)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_SWITCH;
    node->data_type = DT_VOID;
    node->type_name = NULL;

    node->switch_stmt.expr = expr;
    node->switch_stmt.cases = cases;
    node->switch_stmt.default_case = default_case;

    return node;
}

ASTNode *create_case_node(ASTNode *value, ASTNode *body)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_CASE;
    node->data_type = DT_VOID;
    node->type_name = NULL;

    node->case_stmt.value = value;
    node->case_stmt.body = body;

    return node;
}

ASTNode *create_member_access_node(ASTNode *object, char *member)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_MEMBER_ACCESS;
    node->data_type = object->data_type;
    node->type_name = object->type_name;

    node->member_access.object = object;
    node->member_access.member = strdup(member);

    return node;
}

void free_ast(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case NODE_FUNCTION:
        free(node->function.name);
        free_ast(node->function.params);
        free_ast(node->function.body);
        break;

    case NODE_STRUCT_DEF:
        free(node->struct_def.name);
        free_ast(node->struct_def.fields);
        break;

    case NODE_VARIABLE_DECL:
        free(node->variable.name);
        free_ast(node->variable.init_value);
        break;

    case NODE_LITERAL:
        if (node->data_type == DT_STRING)
        {
            free(node->literal.str_val);
        }
        break;

    case NODE_BINOP:
        free(node->binop.op);
        free_ast(node->binop.left);
        free_ast(node->binop.right);
        break;

    case NODE_CALL:
        free(node->call.func_name);
        free_ast(node->call.args);
        break;

    case NODE_SWITCH:
        free_ast(node->switch_stmt.expr);
        free_ast(node->switch_stmt.cases);
        free_ast(node->switch_stmt.default_case);
        break;

    case NODE_CASE:
        free_ast(node->case_stmt.value);
        free_ast(node->case_stmt.body);
        break;

    case NODE_MEMBER_ACCESS:
        free_ast(node->member_access.object);
        free(node->member_access.member);
        break;

    default:
        break;
    }

    if (node->type_name)
        free(node->type_name);
    free(node);
}

// Função auxiliar para impressão (debug)
void print_indent(int indent)
{
    for (int i = 0; i < indent; i++)
        printf("  ");
}

void print_ast(ASTNode *node, int indent)
{
    if (!node)
        return;

    print_indent(indent);

    switch (node->type)
    {
    case NODE_FUNCTION:
        printf("Function: %s\n", node->function.name);
        print_ast(node->function.params, indent + 1);
        print_ast(node->function.body, indent + 1);
        break;

    case NODE_STRUCT_DEF:
        printf("Struct: %s\n", node->struct_def.name);
        print_ast(node->struct_def.fields, indent + 1);
        break;

    case NODE_VARIABLE_DECL:
        printf("Variable: %s (%s)\n", node->variable.name, node->type_name);
        print_ast(node->variable.init_value, indent + 1);
        break;

    case NODE_LITERAL:
        if (node->data_type == DT_NUMBER)
        {
            printf("Literal: %d\n", node->literal.int_val);
        }
        else
        {
            printf("Literal: \"%s\"\n", node->literal.str_val);
        }
        break;

    case NODE_BINOP:
        printf("BinOp: %s\n", node->binop.op);
        print_ast(node->binop.left, indent + 1);
        print_ast(node->binop.right, indent + 1);
        break;

    case NODE_CALL:
        printf("Call: %s\n", node->call.func_name);
        print_ast(node->call.args, indent + 1);
        break;

    case NODE_SWITCH:
        printf("Switch:\n");
        print_ast(node->switch_stmt.expr, indent + 1);
        print_ast(node->switch_stmt.cases, indent + 1);
        print_ast(node->switch_stmt.default_case, indent + 1);
        break;

    default:
        printf("Unknown Node\n");
        break;
    }
}
#include "stdlib.h"
#include "datatypes.h"


typedef enum nodeType {
    STATE,
    TOKEN
} nodeType;

struct Node {
    nodeType type;
    union {
        int state;
        struct token *token;
    };
    struct Node *next;
};


struct Stack {
    struct Node *top;
    int size;
};

struct Stack *createStack(){
    struct Stack *stack = (struct Stack *)malloc(sizeof(struct Stack));
    stack->top = NULL;
    stack->size = 0;
    return stack;
}

void push(struct Stack *stack, nodeType type, void* data){
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    if (type == TOKEN) {
        node->type = TOKEN;
        node->token = (struct token *)data;
    } else {
        node->type = STATE;
        node->state = (int)data;
    }
    node->next = stack->top;
    stack->top = node;
    stack->size++;
}


void* pop(struct Stack *stack){
    if (stack->size == 0){
        return NULL;
    }
    struct Node *node = stack->top;
    stack->top = node->next;
    stack->size--;
    if (node->type == TOKEN){
        struct token *data = node->token;
        free(node);
        return data;
    }
    else if (node->type == STATE){
        int data = node->state;
        free(node);
        return (void *)data;
    }
}

void* peek(struct Stack *stack){
    if (stack->size == 0){
        return NULL;
    }
    struct Node *node = stack->top;
    if (node->type == TOKEN){
        struct token *data = node->token;
        return data;
    }
    else if (node->type == STATE){
        int data = node->state;
        return (void *)data;
    }
}
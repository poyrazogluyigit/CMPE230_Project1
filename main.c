#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "lexer.h"
#include "stacks.h"
#include "concatenator.h"

#define TOKEN_SIZE 256
#define TABLE_SIZE 128
       
regex_t regex;
regmatch_t match[1];

int regexVal;
char msgbuf[257];


int tokenIndex = 0;

int isAssigned = 0;

char *keys[TABLE_SIZE];
long long variables[TABLE_SIZE];



const int parsingTable[75][16][2] =
        {{{1, 3}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {3, 1}, {3, 2}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {-1,-1}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {0,0}, {2, 1}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {1, 12}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 10}, {2, 10}, {2, 10}, {2, 10}, {0,0}, {2, 10}, {0,0}, {0,0}, {0,0}},
         {{1, 17}, {0,0}, {1, 14}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 18}, {0,0}, {0,0}, {0,0}, {3, 13}},
         {{0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {1, 20}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 11}, {2, 11}, {2, 11}, {2, 11}, {0,0}, {2, 11}, {0,0}, {0,0}, {0,0}},
         {{1, 22}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {0,0}, {3, 21}},
         {{1, 22}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {0,0}, {3, 23}},
         {{1, 22}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {0,0}, {3, 24}},
         {{1, 22}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {0,0}, {3, 25}},
         {{1, 22}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {0,0}, {3, 26}},
         {{0,0}, {0,0}, {0,0}, {1, 27}, {0,0}, {0,0}, {0,0}, {1, 28}, {1, 29}, {1, 30}, {1, 31}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{1, 17}, {0,0}, {1, 14}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 18}, {0,0}, {0,0}, {0,0}, {3, 32}},
         {{0,0}, {0,0}, {1, 33}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {1, 34}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {2, 10}, {0,0}, {0,0}, {0,0}, {2, 10}, {2, 10}, {2, 10}, {2, 10}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {2, 11}, {0,0}, {0,0}, {0,0}, {2, 11}, {2, 11}, {2, 11}, {2, 11}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{1, 39}, {0,0}, {1, 36}, {0,0}, {1, 37}, {0,0}, {1, 38}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 40}, {0,0}, {0,0}, {0,0}, {3, 35}},
         {{1, 17}, {0,0}, {1, 14}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 18}, {0,0}, {0,0}, {0,0}, {3, 41}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 6}, {2, 6}, {2, 6}, {2, 6}, {0,0}, {2, 6}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 10}, {2, 10}, {2, 10}, {2, 10}, {0,0}, {2, 10}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 8}, {2, 7}, {2, 7}, {2, 7}, {0,0}, {2, 7}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 8}, {1, 9}, {2, 8}, {2, 8}, {0,0}, {2, 8}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 8}, {1, 9}, {1, 10}, {2, 9}, {0,0}, {2, 9}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {0,0}, {2, 2}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {0,0}, {2, 3}, {0,0}, {0,0}, {0,0}},
         {{1, 17}, {0,0}, {1, 14}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 18}, {0,0}, {0,0}, {0,0}, {3, 42}},
         {{1, 17}, {0,0}, {1, 14}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 18}, {0,0}, {0,0}, {0,0}, {3, 43}},
         {{1, 17}, {0,0}, {1, 14}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 18}, {0,0}, {0,0}, {0,0}, {3, 44}},
         {{1, 17}, {0,0}, {1, 14}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 18}, {0,0}, {0,0}, {0,0}, {3, 45}},
         {{0,0}, {0,0}, {0,0}, {1, 46}, {0,0}, {0,0}, {0,0}, {1, 28}, {1, 29}, {1, 30}, {1, 31}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{1, 39}, {0,0}, {1, 36}, {0,0}, {1, 37}, {0,0}, {1, 38}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 40}, {0,0}, {0,0}, {0,0}, {3, 47}},
         {{1, 17}, {0,0}, {1, 14}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 18}, {0,0}, {0,0}, {0,0}, {3, 48}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 49}, {0,0}, {1, 50}, {1, 51}, {1, 52}, {1, 53}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{1, 17}, {0,0}, {1, 14}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 18}, {0,0}, {0,0}, {0,0}, {3, 54}},
         {{0,0}, {0,0}, {1, 55}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {1, 56}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 10}, {0,0}, {2, 10}, {2, 10}, {2, 10}, {2, 10}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 11}, {0,0}, {2, 11}, {2, 11}, {2, 11}, {2, 11}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {1, 57}, {0,0}, {0,0}, {0,0}, {1, 28}, {1, 29}, {1, 30}, {1, 31}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {2, 6}, {0,0}, {0,0}, {0,0}, {2, 6}, {2, 6}, {2, 6}, {2, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {2, 7}, {0,0}, {0,0}, {0,0}, {1, 28}, {2, 7}, {2, 7}, {2, 7}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {2, 8}, {0,0}, {0,0}, {0,0}, {1, 28}, {1, 29}, {2, 8}, {2, 8}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {2, 9}, {0,0}, {0,0}, {0,0}, {1, 28}, {1, 29}, {1, 30}, {2, 9}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {2, 3}, {0,0}, {0,0}, {0,0}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 58}, {0,0}, {1, 50}, {1, 51}, {1, 52}, {1, 53}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {1, 59}, {0,0}, {0,0}, {0,0}, {1, 28}, {1, 29}, {1, 30}, {1, 31}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{1, 17}, {0,0}, {1, 14}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 18}, {0,0}, {0,0}, {0,0}, {3, 60}},
         {{1, 39}, {0,0}, {1, 36}, {0,0}, {1, 37}, {0,0}, {1, 38}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 40}, {0,0}, {0,0}, {0,0}, {3, 61}},
         {{1, 39}, {0,0}, {1, 36}, {0,0}, {1, 37}, {0,0}, {1, 38}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 40}, {0,0}, {0,0}, {0,0}, {3, 62}},
         {{1, 39}, {0,0}, {1, 36}, {0,0}, {1, 37}, {0,0}, {1, 38}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 40}, {0,0}, {0,0}, {0,0}, {3, 63}},
         {{1, 39}, {0,0}, {1, 36}, {0,0}, {1, 37}, {0,0}, {1, 38}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 40}, {0,0}, {0,0}, {0,0}, {3, 64}},
         {{0,0}, {0,0}, {0,0}, {1, 65}, {0,0}, {0,0}, {0,0}, {1, 28}, {1, 29}, {1, 30}, {1, 31}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{1, 39}, {0,0}, {1, 36}, {0,0}, {1, 37}, {0,0}, {1, 38}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 40}, {0,0}, {0,0}, {0,0}, {3, 66}},
         {{1, 17}, {0,0}, {1, 14}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 18}, {0,0}, {0,0}, {0,0}, {3, 67}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {0,0}, {2, 5}, {0,0}, {0,0}, {0,0}},
         {{1, 17}, {0,0}, {1, 14}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 18}, {0,0}, {0,0}, {0,0}, {3, 68}},
         {{0,0}, {0,0}, {0,0}, {2, 5}, {0,0}, {0,0}, {0,0}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {1, 69}, {0,0}, {0,0}, {0,0}, {1, 28}, {1, 29}, {1, 30}, {1, 31}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 6}, {0,0}, {2, 6}, {2, 6}, {2, 6}, {2, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 7}, {0,0}, {1, 50}, {2, 7}, {2, 7}, {2, 7}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 8}, {0,0}, {1, 50}, {1, 51}, {2, 8}, {2, 8}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 9}, {0,0}, {1, 50}, {1, 51}, {1, 52}, {2, 9}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 3}, {0,0}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 70}, {0,0}, {1, 50}, {1, 51}, {1, 52}, {1, 53}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {1, 71}, {0,0}, {0,0}, {0,0}, {1, 28}, {1, 29}, {1, 30}, {1, 31}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {1, 72}, {0,0}, {0,0}, {0,0}, {1, 28}, {1, 29}, {1, 30}, {1, 31}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 4}, {2, 4}, {2, 4}, {2, 4}, {0,0}, {2, 4}, {0,0}, {0,0}, {0,0}},
         {{1, 17}, {0,0}, {1, 14}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 18}, {0,0}, {0,0}, {0,0}, {3, 73}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 5}, {0,0}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {2, 4}, {0,0}, {0,0}, {0,0}, {2, 4}, {2, 4}, {2, 4}, {2, 4}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {1, 74}, {0,0}, {0,0}, {0,0}, {1, 28}, {1, 29}, {1, 30}, {1, 31}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
         {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 4}, {0,0}, {2, 4}, {2, 4}, {2, 4}, {2, 4}, {0,0}, {0,0}, {0,0}, {0,0},{0,0}}
};




long long power(long long base, long long exp){
    long long result = 1;
    for (int i = 0; i < exp; i++){
        result = result*base;
    }
    return result;
}


int toHash(char* string){
    long hash = 0;
    int p = 991;
    int powr = 0;
    while (*string != '\0'){
        hash = hash + ((int) *string)*(power(p, powr));
        string++;
        powr++;
    }
    return hash%TABLE_SIZE;
}

long long get(char* string){
    int hash = toHash(string);
    int i = 0;
    for (int i = 0; i < TABLE_SIZE; i++){
        if (variables[hash+i] == -1) break;
    }
    return variables[hash+i];
}

void put(char* string, long long value){
    int hash = toHash(string);
    int i = 0;
    for (int i = 0; i < TABLE_SIZE; i++){
        if (variables[hash+i] == -1) break;
    }
    variables[hash+i] = value;
    keys[hash+i] = string;
}

struct intStack *stateStack;
struct Stack *tokenStack;


long long arithmetic(struct token* operator, struct token* leftoperand, struct token* rightoperand){
    long long leftVal = strtoll(leftoperand->value, NULL, 10);
    long long rightVal = strtoll(rightoperand->value, NULL, 10);

    long long result;
    switch(operator->type){
        case MULT:{
            switch (operator->value[0]){
                case '*':
                    result = leftVal * rightVal;
                    break;
                case '/':
                    result = leftVal / rightVal;
                    break;
                case '%':
                    result = leftVal % rightVal;
                    break;
            }
            break;
        }
        case ADD:{
            switch (operator->value[0]){
                case '+':
                    result = leftVal + rightVal;
                    break;
                case '-':
                    result = leftVal - rightVal;
                    break;
            }
            break;
        }
        case AND:{
            result = leftVal & rightVal;
            break;
        }
        case OR:{
            result = leftVal | rightVal;
            break;
        }
    }
    return result;
}

int getFunction(char* function){
    if (*function == '\0') return -1;
    switch (*function){
        case 'n':
            return 0;
        case 'x':
            return 1;
        case 'l':{
            if (*(function+1) == 's'){
                return 2;
            }
            if (*(function+1) == 'r'){
                return 3;
            }
        }
        case 'r':{
            if (*(function+1) == 's'){
                return 4;
            }
            if (*(function+1) == 'r'){
                return 5;
            }
        }
    }
}


long long evaluate(struct token* function, struct token* leftoperand, struct token* rightoperand){
    long long leftVal = strtoll(leftoperand->value, NULL, 10);
    if (rightoperand == NULL){
        return ~leftVal;
    }
    long long rightVal = strtoll(rightoperand->value, NULL, 10);

    long long result;
    switch (getFunction(function->value)){
        case 0:
            break;
        case 1:
            result = leftVal ^ rightVal;
            break;
        case 2:
            result = leftVal << rightVal;
            break;
        case 3:
            result = (leftVal << rightVal)|(leftVal >> (64 - rightVal));
            break;
        case 4:
            result = leftVal >> rightVal;
            break;
        case 5:
            result = (leftVal >> rightVal)|(leftVal << (64 - rightVal));
            break;
    }
    return result;
}

void goTo(int state){
    i_push(stateStack, state);
}

void shift(int state, struct token *token){
    struct token *newtoken = token;
    push(tokenStack, newtoken);
    token = NULL;
    goTo(state);
}

void reduce(int rule){
    switch (rule){
        case 0:{
            struct token* token = (struct token*) pop(tokenStack);
            token->type = Sp;
            push(tokenStack, token);
            break;
        }
        case 1:{
            struct token* token = (struct token*) pop(tokenStack);
            token->type = S;
            push(tokenStack, token);
            break;
        }
        case 2:{
            struct token* expression = (struct token*) pop(tokenStack);
            pop(tokenStack);
            struct token* var = (struct token*) pop(tokenStack);
            long long value = strtoll(expression->value, NULL, 10);
            put(var->value, value);
            char string[TOKEN_SIZE+1];
            sprintf(string, "%lld", value);
            struct token *newtoken = tokenize(string, strlen(string));
            newtoken->type = S;
            push(tokenStack, newtoken);
            isAssigned = 1;
            i_pop(stateStack);
            i_pop(stateStack);
            break;
        }
        case 3:{
            pop(tokenStack);
            struct token* token = (struct token*) pop(tokenStack);
            pop(tokenStack);
            push(tokenStack, token);
            i_pop(stateStack);
            i_pop(stateStack);
            break;
        }
        case 4:{
            pop(tokenStack);
            struct token* rightOperand = (struct token*) pop(tokenStack);
            pop(tokenStack);
            struct token* leftOperand = (struct token*) pop(tokenStack);
            pop(tokenStack);
            struct token* function = (struct token*) pop(tokenStack);
            char value[TOKEN_SIZE+1];
            sprintf(value, "%lld", evaluate(function, leftOperand, rightOperand));
            int len = strlen(value);
            struct token *newtoken = tokenize(value, len);
            newtoken->type = E;
            push(tokenStack, newtoken);
            i_pop(stateStack);
            i_pop(stateStack);
            i_pop(stateStack);
            i_pop(stateStack);
            i_pop(stateStack);
            break;
        }
        case 5:{
            pop(tokenStack);
            struct token* leftOperand = (struct token*) pop(tokenStack);
            pop(tokenStack);
            struct token* function = (struct token*) pop(tokenStack);
            char value[TOKEN_SIZE+1];
            sprintf(value, "%lld", evaluate(function, leftOperand, NULL));
            struct token *newtoken = tokenize(value, strlen(value));
            newtoken->type = E;
            push(tokenStack, newtoken);
            i_pop(stateStack);
            i_pop(stateStack);
            i_pop(stateStack);
            break;
        }
        case 6:
        case 7:
        case 8:
        case 9:{
            struct token* rightOperand = (struct token*) pop(tokenStack);
            struct token* operator = (struct token*) pop(tokenStack);
            struct token* leftOperand = (struct token*) pop(tokenStack);
            char value[TOKEN_SIZE+1];
            sprintf(value, "%lld", arithmetic(operator, leftOperand, rightOperand));
            struct token *newtoken = tokenize(value, strlen(value));
            newtoken->type = E;
            push(tokenStack, newtoken);
            i_pop(stateStack);
            i_pop(stateStack);
            break;
        }
        case 10:{
            struct token* variable = (struct token*) pop(tokenStack);
            long long value = get(variable->value) == -1 ? 0 : get(variable->value);
            char valStr[TOKEN_SIZE+1];
            sprintf(valStr, "%lld", value);
            strcpy(variable->value, valStr); 
            variable->type = E;
            push(tokenStack, variable);
            break;
        }
        case 11:{
            struct token* token = (struct token*) pop(tokenStack);
            token->type = E;
            push(tokenStack, token);
            break;
        }
    }
}

int main(){

    for (int i = 0; i < 128; i++){
        variables[i] = -1;
    }

    printf("> ");
    //compile the regex pattern
    regexVal = regcomp(&regex, "[a-zA-Z]+|[0-9]+|[^[:alnum:]]", REG_EXTENDED);

    if (regexVal) {
    fprintf(stderr, "Regex compilation error\n");
    return 1;
}

    //start reading input
    while (fgets(msgbuf, sizeof(msgbuf), stdin)) {
        if (msgbuf == NULL) break;

        //execute search
        regexVal = regexec(&regex, msgbuf, 1, match, 0);

        int lastPtr = 0;

        int i = 0;

        //allocate memory for tokens
        struct token* tokens = calloc(256, sizeof(struct token));




        //look for all matches in the line
        while (regexVal == 0) {

            int startBuffer = match[0].rm_so;
            int endBuffer = match[0].rm_eo;
            int len = endBuffer - startBuffer;

            //copy the match to a string
            char tokenStr[len + 1];
            strncpy(tokenStr, msgbuf + lastPtr + startBuffer, len);
            tokenStr[len] = '\0';
            
            //check for beginning of the comment 
            const char *comment = "%";
            int cmp = strcmp(tokenStr, comment);
            if (cmp == 0) break;
            //tokenize and add to array
            if (!isspace((int) tokenStr[0])) {
                struct token *token = tokenize(tokenStr, len);
                tokens[i] = *token;
                i++;
            }

            //go to next match
            lastPtr += endBuffer;
            regexVal = regexec(&regex, msgbuf + lastPtr, 1, match, 0);
        }
        struct token *eol = tokenize("$", 1);
        tokens[i] = *eol;

        //parsing block
        i_init(&stateStack);
        init(&tokenStack);

        i_push(stateStack, 0);
        int condition = 1;


        int reduced = 0;


        int step = 0;
        while (condition) {
            if ((step == 0) && (tokens[0].type == EOL)){
                printf("> ");
                break;
            }
            if (tokenIndex == TOKEN_SIZE-1) {
                //printf("Error!\n");
                break;
            }
            struct token *nextToken;
            if (reduced) {
                nextToken = ((struct token *) peek(tokenStack));
            } else {
                nextToken = &tokens[tokenIndex];
            }

            int type = (*nextToken).type;
            int currentState = i_pop(stateStack);

            int action = parsingTable[currentState][type][0];
            int targetState = parsingTable[currentState][type][1];


            reduced = 0;
            step++;
            switch (action) {
                //accept the statement
                case -1: {
                    if (isAssigned == 0) printf("%s\n", ((struct token *) peek(tokenStack))->value);
                    condition = 0;
                    printf("> ");
                    break;
                }
                //error
                case 0:
                    printf("Error!\n");
                    printf("> ");
                    condition = 0;
                    break;
                //shift
                case 1:
                    i_push(stateStack, currentState);
                    shift(targetState, nextToken);
                    nextToken = NULL;
                    tokenIndex++;
                    break;
                //reduce
                case 2:
                    reduce(targetState);
                    reduced = 1;
                    break;
                //goto
                case 3:
                    i_push(stateStack, currentState);
                    goTo(targetState);
                    break;
            }
        }
        free(stateStack);
        free(tokenStack);
        free(tokens);
        tokenIndex = 0;
        isAssigned = 0;
    }
    regfree(&regex);
}


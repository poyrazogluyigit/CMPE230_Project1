#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define TOKEN_SIZE 256
       
regex_t regex;
regmatch_t match[1];

int regexVal;
char msgbuf[257];


int tokenIndex = 0;


//enum corresponding to non-terminals and terminals 
//declared within the grammar of the language.
enum type{
    V, 
    EQ,          //61 (6 mod 11)
    OBR,         //40 (7 mod 11)
    CBR,         //41 (8 mod 11)
    F,   
    COMM,       //44 (0 mod 11)
    NOT,        
    AND,         //38 (5 mod 11)
    OR,          //124 (3 mod 11)
    MULT,        //42 (9 mod 11)
    ADD,       //43 (10 mod 11)
    SUB,        //45 (1 mod 11)
    I,        
    EOL,        
    Sp,
    S,
    E          

};


/*
    Each lexeme can be represented as a token.
    type -> enum corresponding to the type of the lexeme
    value -> value of the lexeme stored as char[]
*/
struct token{
    enum type type;
    char value[TOKEN_SIZE];
};



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
} Stack;


struct mapElement {
    char *key;
    long long value;
} mapElement;


const char *strings[] = {"S", "V", "E", "F", "I", "NOT", "AND", "OR", "MULT", "ADD", "COMM", "SUB", "OBR", "CBR", "EQ"};

const char *uniKeys[] = {",","-","","|","","&","=","(",")","*","+"};
const enum type uniTypes[] = {COMM, SUB, -1 , OR, -1, AND, EQ, OBR, CBR, MULT, ADD};


const char *keywords[] = {"not", "xor", "ls", "rs", "lr", "rr"};


const int parsingTable[80][17][2] = {{{1, 3}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {3, 1}, {3, 2}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {-1,-1}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, {0,0}, {2, 1}, {0,0}, {0,0}, {0,0}},
{{0,0}, {1, 13}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 11}, {2, 11}, {2, 11}, {2, 11}, {2, 11}, {0,0}, {2, 11}, {0,0}, {0,0}, {0,0}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 14}},
{{0,0}, {0,0}, {1, 20}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {1, 21}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 12}, {2, 12}, {2, 12}, {2, 12}, {2, 12}, {0,0}, {2, 12}, {0,0}, {0,0}, {0,0}},
{{1, 23}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {0,0}, {3, 22}},
{{1, 23}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {0,0}, {3, 24}},
{{1, 23}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {0,0}, {3, 25}},
{{1, 23}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {0,0}, {3, 26}},
{{1, 23}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {0,0}, {3, 27}},
{{1, 23}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {0,0}, {3, 28}},
{{0,0}, {0,0}, {0,0}, {1, 29}, {0,0}, {0,0}, {0,0}, {1, 30}, {1, 31}, {1, 32}, {1, 33}, {1, 34}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 35}},
{{0,0}, {0,0}, {1, 36}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {1, 37}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {2, 11}, {0,0}, {0,0}, {0,0}, {2, 11}, {2, 11}, {2, 11}, {2, 11}, {2, 11}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {2, 12}, {0,0}, {0,0}, {0,0}, {2, 12}, {2, 12}, {2, 12}, {2, 12}, {2, 12}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{1, 42}, {0,0}, {1, 39}, {0,0}, {1, 40}, {0,0}, {1, 41}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 43}, {0,0}, {0,0}, {0,0}, {3, 38}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 44}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 6}, {2, 6}, {2, 6}, {2, 6}, {2, 6}, {0,0}, {2, 6}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 11}, {2, 11}, {2, 11}, {2, 11}, {2, 11}, {0,0}, {2, 11}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 8}, {2, 7}, {2, 7}, {2, 7}, {2, 7}, {0,0}, {2, 7}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 8}, {1, 9}, {2, 8}, {2, 8}, {2, 8}, {0,0}, {2, 8}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 8}, {1, 9}, {1, 10}, {2, 9}, {2, 9}, {0,0}, {2, 9}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 8}, {1, 9}, {1, 10}, {2, 10}, {2, 10}, {0,0}, {2, 10}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, {0,0}, {2, 2}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {0,0}, {2, 3}, {0,0}, {0,0}, {0,0}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 45}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 46}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 47}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 48}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 49}},
{{0,0}, {0,0}, {0,0}, {1, 50}, {0,0}, {0,0}, {0,0}, {1, 30}, {1, 31}, {1, 32}, {1, 33}, {1, 34}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{1, 42}, {0,0}, {1, 39}, {0,0}, {1, 40}, {0,0}, {1, 41}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 43}, {0,0}, {0,0}, {0,0}, {3, 51}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 52}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 53}, {0,0}, {1, 54}, {1, 55}, {1, 56}, {1, 57}, {1, 58}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 59}},
{{0,0}, {0,0}, {1, 60}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {1, 61}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 11}, {0,0}, {2, 11}, {2, 11}, {2, 11}, {2, 11}, {2, 11}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 12}, {0,0}, {2, 12}, {2, 12}, {2, 12}, {2, 12}, {2, 12}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {1, 62}, {0,0}, {0,0}, {0,0}, {1, 30}, {1, 31}, {1, 32}, {1, 33}, {1, 34}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {2, 6}, {0,0}, {0,0}, {0,0}, {2, 6}, {2, 6}, {2, 6}, {2, 6}, {2, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {2, 7}, {0,0}, {0,0}, {0,0}, {1, 30}, {2, 7}, {2, 7}, {2, 7}, {2, 7}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {2, 8}, {0,0}, {0,0}, {0,0}, {1, 30}, {1, 31}, {2, 8}, {2, 8}, {2, 8}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {2, 9}, {0,0}, {0,0}, {0,0}, {1, 30}, {1, 31}, {1, 32}, {2, 9}, {2, 9}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {2, 10}, {0,0}, {0,0}, {0,0}, {1, 30}, {1, 31}, {1, 32}, {2, 10}, {2, 10}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {2, 3}, {0,0}, {0,0}, {0,0}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 63}, {0,0}, {1, 54}, {1, 55}, {1, 56}, {1, 57}, {1, 58}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {1, 64}, {0,0}, {0,0}, {0,0}, {1, 30}, {1, 31}, {1, 32}, {1, 33}, {1, 34}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 65}},
{{1, 42}, {0,0}, {1, 39}, {0,0}, {1, 40}, {0,0}, {1, 41}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 43}, {0,0}, {0,0}, {0,0}, {3, 66}},
{{1, 42}, {0,0}, {1, 39}, {0,0}, {1, 40}, {0,0}, {1, 41}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 43}, {0,0}, {0,0}, {0,0}, {3, 67}},
{{1, 42}, {0,0}, {1, 39}, {0,0}, {1, 40}, {0,0}, {1, 41}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 43}, {0,0}, {0,0}, {0,0}, {3, 68}},
{{1, 42}, {0,0}, {1, 39}, {0,0}, {1, 40}, {0,0}, {1, 41}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 43}, {0,0}, {0,0}, {0,0}, {3, 69}},
{{1, 42}, {0,0}, {1, 39}, {0,0}, {1, 40}, {0,0}, {1, 41}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 43}, {0,0}, {0,0}, {0,0}, {3, 70}},
{{0,0}, {0,0}, {0,0}, {1, 71}, {0,0}, {0,0}, {0,0}, {1, 30}, {1, 31}, {1, 32}, {1, 33}, {1, 34}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{1, 42}, {0,0}, {1, 39}, {0,0}, {1, 40}, {0,0}, {1, 41}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 43}, {0,0}, {0,0}, {0,0}, {3, 72}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 73}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {0,0}, {2, 5}, {0,0}, {0,0}, {0,0}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 74}},
{{0,0}, {0,0}, {0,0}, {2, 5}, {0,0}, {0,0}, {0,0}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {1, 75}, {0,0}, {0,0}, {0,0}, {1, 30}, {1, 31}, {1, 32}, {1, 33}, {1, 34}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 6}, {0,0}, {2, 6}, {2, 6}, {2, 6}, {2, 6}, {2, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 7}, {0,0}, {1, 54}, {2, 7}, {2, 7}, {2, 7}, {2, 7}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 8}, {0,0}, {1, 54}, {1, 55}, {2, 8}, {2, 8}, {2, 8}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 9}, {0,0}, {1, 54}, {1, 55}, {1, 56}, {2, 9}, {2, 9}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 10}, {0,0}, {1, 54}, {1, 55}, {1, 56}, {2, 10}, {2, 10}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 3}, {0,0}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 76}, {0,0}, {1, 54}, {1, 55}, {1, 56}, {1, 57}, {1, 58}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {1, 77}, {0,0}, {0,0}, {0,0}, {1, 30}, {1, 31}, {1, 32}, {1, 33}, {1, 34}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {1, 78}, {0,0}, {0,0}, {0,0}, {1, 30}, {1, 31}, {1, 32}, {1, 33}, {1, 34}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 4}, {2, 4}, {2, 4}, {2, 4}, {2, 4}, {0,0}, {2, 4}, {0,0}, {0,0}, {0,0}},
{{1, 18}, {0,0}, {1, 15}, {0,0}, {1, 16}, {0,0}, {1, 17}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 19}, {0,0}, {0,0}, {0,0}, {3, 79}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 5}, {0,0}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {2, 4}, {0,0}, {0,0}, {0,0}, {2, 4}, {2, 4}, {2, 4}, {2, 4}, {2, 4}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {1, 80}, {0,0}, {0,0}, {0,0}, {1, 30}, {1, 31}, {1, 32}, {1, 33}, {1, 34}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {2, 4}, {0,0}, {2, 4}, {2, 4}, {2, 4}, {2, 4}, {2, 4}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}}};




int isNumber(char *string){
    while (*string != '\0'){
        if (isdigit(*string) == 0) return 0;
        string++;
    }
    return 1;
}



//very dangerous return value on failure
//explicitly check failure with type == -1
enum type getType(char *string, int len){
    if (isNumber(string)) return I;
    else{
        if (len > 1){
            for (int i = 0; i < 6; i++){
                if (strcmp((const char*)string, (const char*)keywords[i]) == 0){
                    if (i == 0) return NOT; else return F;}
            }
            return V;
        } 
        //either f or v
        else{
            int ascii = (int) *string;
            ascii = ascii % 11;
            int k = strcmp(((const char*)uniKeys[ascii]), (const char*)string);
            if (k == 0){
                if (uniTypes[ascii] != -1) return uniTypes[ascii];
            }
        }   return V;
    }
}


//tokenizes a given string.
//does not check for whitespaces and the behavior is 
//undefined for whitespaces.
struct token tokenize(char *string, int len){
    enum type type = getType(string, len);
    struct token token;
    token.type = type;
    strncpy(token.value, string, len+1);
    return token;
}


//initialize mapElement array
struct mapElement* initMap(int len){
    struct mapElement *newVariables = (struct mapElement *)calloc(len + 1, sizeof(struct mapElement));
    struct mapElement lenHolder;
    lenHolder.key = "len";
    lenHolder.value = len;
    newVariables[0] = lenHolder;
    for (int i = 1; i < len+1; i++){
        newVariables[i].key = "";
        newVariables[i].value = 0;
    }
    return newVariables;
}

//hash function for the map
int hashFunction(char *key, int len){
    int hash = 0;
    for (int i = 0; i < strlen(key); i++){
        hash = hash + (int)key[i];
    }
    return hash % len;
}




void put_func(struct mapElement* arr, char *key, int value){
    int hash = hashFunction(key, arr[0].value);
    int i = 0;
    while (strcmp(arr[hash].key, "") != 0){
        hash = (hash + i) % arr[0].value;
        i++;
    }
    arr[hash].key = key;
    arr[hash].value = value;
}

//get the value of a key
int get(struct mapElement* arr, char *key){
    int len = arr[0].value; //get the length of the array
    int hash = hashFunction(key, len);
    int i = 0;
    while (arr[hash].key != key){
        hash = (hash + i) % len;
        i++;
        if (i == len) return 0;
    }
    return arr[hash].value;
}


//expand the size of the hashtable
struct mapElement* expand(struct mapElement* arr){
    int len = arr[0].value;
    struct mapElement *new = initMap(2*len);
    for (int i = 0; i < 2*len; i++){
        put_func(new, arr[i].key, arr[i].value);
    }
    free(arr);
    arr = new;
    return arr;
}

void put(struct mapElement* arr, int nOfElements, char *key, int value){
    if (nOfElements == arr[0].value){
        arr = expand(arr);
    }
    put_func(arr, key, value);
}
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

struct token tokens[256];
struct Stack *stateStack;
struct Stack *tokenStack;


long long arithmetic(struct token* operator, struct token* leftoperand, struct token* rightoperand){
    long long leftVal = strtoll(leftoperand, NULL, 10);
    long long rightVal = strtoll(rightoperand, NULL, 10);

    long long result;
    switch(operator->type){
        case AND:{
            result = leftVal & rightVal;
            break;
        }
        case OR:{
            result = leftVal | rightVal;
            break;
        }
        case MULT:{
            result = leftVal * rightVal;
            break;
        }
        case ADD:{
            result = leftVal + rightVal;
            break;
        }
        case SUB:{
            result = leftVal - rightVal;
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
            result = leftVal >> rightVal;
            break;
        case 3:
            result = (leftVal << rightVal)|(leftVal >> (64LL - rightVal));
            break;
        case 4:
            result = leftVal << rightVal;
            break;
        case 5:
            result = (leftVal >> rightVal)|(leftVal << (64 - rightVal));
            break;
    }
    return result;
}

void goTo(int token){
    push(tokenStack, TOKEN, (void*)token);
}

void shift(int state, int token){
    push(stateStack, STATE, (void*)state);
    goTo(token);
}

void reduce(int rule){
    switch (rule){
        case 0:{
            struct token* token = (struct token*) pop(tokenStack);
            token->type = Sp;
            push(tokenStack, TOKEN, token);
            break;
        }
        case 1:{
            struct token* token = (struct token*) pop(tokenStack);
            token->type = S;
            push(tokenStack, TOKEN, token);
            break;
        }
        case 2:{
            struct token* token = (struct token*) pop(tokenStack);
            pop(tokenStack);
            struct token* var = (struct token*) pop(tokenStack);
            put(variables, noelem, var->value, strtoll(token->value, NULL, 10));
            token->type = S;
            push(tokenStack, TOKEN, token);
            break;
        }
        case 3:{
            pop(tokenStack);
            struct token* token = (struct token*) pop(tokenStack);
            pop(tokenStack);
            push(tokenStack, TOKEN, token);
            break;
        }
        case 4:{
            pop(tokenStack);
            struct token* rightOperand = (struct token*) pop(tokenStack);
            pop(tokenStack);
            struct token* leftOperand = (struct token*) pop(tokenStack);
            pop(tokenStack);
            struct token* function = (struct token*) pop(tokenStack);
            char value[256];
            sprintf(value, "%lld", evaluate(function, leftOperand, rightOperand));
            struct token newtoken = tokenize(value, strlen(value));
            push(tokenStack, TOKEN, &newtoken);
            break;
        }
        case 5:{
            pop(tokenStack);
            struct token* leftOperand = (struct token*) pop(tokenStack);
            pop(tokenStack);
            struct token* function = (struct token*) pop(tokenStack);
            char value[256];
            sprintf(value, "%lld", evaluate(function, leftOperand, NULL));
            struct token newtoken = tokenize(value, strlen(value));
            push(tokenStack, TOKEN, &newtoken);
            break;
        }
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:{
            struct token* rightOperand = (struct token*) pop(tokenStack);
            struct token* operator = (struct token*) pop(tokenStack);
            struct token* leftOperand = (struct token*) pop(tokenStack);
            char value[256];
            sprintf(value, "%lld", arithmetic(operator, leftOperand, rightOperand));
            struct token newtoken = tokenize(value, strlen(value));
            push(tokenStack, TOKEN, &newtoken);
            break;
        }
        case 11:{
            struct token* token = (struct token*) pop(tokenStack);
            token->value = variables[token->value];
            //get value from hashmap, change value
            //ATTENTION CITIZEN! SOCIAL CREDIT DEDUCTION
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            token->type = E;
            push(tokenStack, TOKEN, token);
            break;
        }
        case 12:{
            struct token* token = (struct token*) pop(tokenStack);
            token->type = E;
            push(tokenStack, TOKEN, token);
            break;
        }
    }
}




int main(){

    //compile the regex pattern
    regexVal = regcomp(&regex, "[a-zA-Z0-9]+|[^[:alnum:]]", REG_EXTENDED);

    if (regexVal) {
    fprintf(stderr, "Regex compilation error\n");
    return 1;
}

    //start reading input
    while (fgets(msgbuf, sizeof(msgbuf), stdin)){
        if (msgbuf == NULL) break;
        
        //execute search
        regexVal = regexec(&regex, msgbuf, 1, match, 0);

        int lastPtr = 0;
        
        int i = 0;
        //look for all matches in the line
        while (regexVal == 0){

            int startBuffer = match[0].rm_so;
            int endBuffer = match[0].rm_eo;
            int len = endBuffer - startBuffer;

            //copy the match to a string
            char tokenStr[len+1];
            strncpy(tokenStr, msgbuf+lastPtr+startBuffer, len);
            tokenStr[len] = '\0';
        
            //tokenize and add to array
            if (!isspace((int) tokenStr[0])){
                struct token token = tokenize(tokenStr, len);
                tokens[i] = token;
                i++;
            }

            //go to next match
            lastPtr += endBuffer;
            regexVal = regexec(&regex, msgbuf + lastPtr, 1, match, 0);
        }

        //parsing block
    }
    while(continuation_condition){
        stateStack = createStack();
        tokenStack = createStack();
        push(stateStack, STATE, 0);
        int condition = 1;
        while (condition){
            if (tokenIndex == sizeof(tokens)/sizeof(tokens[0])){
                printf("Error!\n");
            }
            struct token nextToken = tokens[tokenIndex+1];
            int action[] = parsingTable[*((int*)peek(stateStack))][nextToken.type];
            switch (action[0]){
                case -1: {
                    if (tokens[1].type != EQ) printf("%s", ((struct token *) peek(tokenStack))->value);
                    condition = 0;
                    break;
                }
                case 0: printf("Error!\n"); condition = 0; break;
                case 1: shift(action[1], nextToken.type); tokenIndex++; break;
                case 2: reduce(action[1]); break;
                case 3: goTo(nextToken.type); tokenIndex++; break;
            }

        }
        free(stateStack);
        free(tokenStack);
        free(tokens);
    }



    regfree(&regex);
    return 0;

}


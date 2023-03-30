#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>

#define TOKEN_SIZE 256
#define TABLE_SIZE 128
       
regex_t regex;
regmatch_t match[1];

int regexVal;
char msgbuf[257];


int tokenIndex = 0;

int isAssigned = 0;


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
    char value[TOKEN_SIZE+1];
};



typedef enum nodeType {
    STATE,
    TOKEN
} nodeType;


struct Node{
    struct token *data;
    struct Node *next;
    int state;
};

struct Stack{
    struct Node *top;
    int capacity;
};


struct intNode{
    int data;
    struct intNode *next;
};

struct intStack{
    struct intNode *top;
    int capacity;
};









const char *strings[] = {"S", "V", "E", "F", "I", "NOT", "AND", "OR", "MULT", "ADD", "COMM", "SUB", "OBR", "CBR", "EQ"};

const char *uniKeys[] = {",","-","","|","","&","=","(",")","*","+"};
const enum type uniTypes[] = {COMM, SUB, -1 , OR, -1, AND, EQ, OBR, CBR, MULT, ADD};


const char *keywords[] = {"not", "xor", "ls", "rs", "lr", "rr"};

char *keys[TABLE_SIZE];
long long variables[TABLE_SIZE];



const int parsingTable[81][17][2] = {{{1, 3}, {0,0}, {1, 4}, {0,0}, {1, 5}, {0,0}, {1, 6}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {1, 7}, {0,0}, {0,0}, {3, 1}, {3, 2}},
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
            char eol[2] = "$";
            int x = strcmp((const char*)string, eol);
            if (x == 0) return EOL;
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
struct token *tokenize(char *string, int len){
    enum type type = getType(string, len);
    struct token *token = calloc(1, sizeof(struct token));
    token->type = type;
    strncpy(token->value, string, len+1);
    return token;
}








void init(struct Stack **stack){
    struct Stack *newStack = (struct Stack *)malloc(sizeof(struct Stack));
    newStack->capacity = 0;
    newStack->top = NULL;

    *stack = newStack;

}

void push(struct Stack *stack, struct token *data){
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = (stack->top);
    stack->top = newNode;
}

struct token *pop(struct Stack *stack){
    struct Node *temp = (stack->top);
    stack->top = ((stack->top)->next);
    struct token *data = temp->data;
    free(temp);
    return data;
}


struct token *peek(struct Stack *stack){
    return (stack->top)->data;
}






void i_init(struct intStack **stack){
    struct intStack *newStack = (struct intStack *)malloc(sizeof(struct intStack));
    newStack->capacity = 0;
    newStack->top = NULL;

    *stack = newStack;
}

void i_push(struct intStack *stack, int data){
    struct intNode *newNode = (struct intNode *)malloc(sizeof(struct intNode));
    newNode->data = data;
    struct intNode *temp = stack->top;
    newNode->next = temp;
    stack->top = newNode;
}

int i_pop(struct intStack *stack){
    struct intNode *temp = stack->top;
    stack->top = ((stack->top)->next);
    int data = temp->data;
    free(temp);
    return data;
}

int i_peek(struct intStack *stack){
    return (stack->top)->data;
}












//struct token tokens[];
struct intStack *stateStack;
struct Stack *tokenStack;
int noelem = 0;


long long arithmetic(struct token* operator, struct token* leftoperand, struct token* rightoperand){
    long long leftVal = strtoll(leftoperand->value, NULL, 10);
    long long rightVal = strtoll(rightoperand->value, NULL, 10);

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
        case 9:
        case 10:{
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
        case 11:{
            struct token* variable = (struct token*) pop(tokenStack);
            long long value = get(variable->value) == -1 ? 0 : get(variable->value);
            char valStr[TOKEN_SIZE+1];
            sprintf(valStr, "%lld", value);
            strcpy(variable->value, valStr); 
            variable->type = E;
            push(tokenStack, variable);
            break;
        }
        case 12:{
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
    regexVal = regcomp(&regex, "[a-zA-Z0-9]+|[^[:alnum:]]", REG_EXTENDED);

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
                    //tokenIndex++;
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


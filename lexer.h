#ifndef CMPE230_PROJECT1_LEXER_H
#define CMPE230_PROJECT1_LEXER_H

#define TOKEN_SIZE 256

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

struct token{
    enum type type;
    char value[TOKEN_SIZE+1];
};


int isNumber(char *string);
enum type getType(char *string, int len);
struct token *tokenize(char *string, int len);

#endif

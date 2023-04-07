#ifndef CMPE230_PROJECT1_LEXER_H
#define CMPE230_PROJECT1_LEXER_H

#define TOKEN_SIZE 256

//enum corresponding to non-terminals and terminals 
//declared within the grammar of the language.
enum type{
    V, 
    EQ,
    OBR,
    CBR,
    F,   
    COMM,
    NOT,
    MULT,
    ADD,
    AND,
    OR,
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

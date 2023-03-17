#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <ctype.h>

#define TOKEN_SIZE 5


//enum corresponding to non-terminals and terminals 
//declared within the grammar of the language.
enum type{
    S, 
    V,    //terminal? data type
    E,
    F,    //terminal data type
    I,    //terminal data type
    NOT, 
    AND,        //38 (5 mod 11)
    OR,         //124 (3 mod 11)
    MULT,       //42 (9 mod 11)
    ADD,        //43 (10 mod 11)
    COMM,       //44 (0 mod 11)
    SUB,        //45 (1 mod 11)
    OBR,        //40 (7 mod 11)
    CBR,        //41 (8 mod 11)
    EQ          //61 (6 mod 11)

};

enum type uniTypes[] = {COMM, SUB, -1 , OR, -1, AND, EQ, OBR, CBR, MULT, ADD};
char *keywords[] = {"not", "xor", "ls", "rs", "lr", "rr"};

/*
    Each lexeme can be represented as a token.
    type -> enum corresponding to the type of the lexeme
    value -> value of the lexeme stored as char[]
*/
struct token{
    enum type type;
    char value[TOKEN_SIZE];
};


//tokenizes a given string.
struct token tokenize(char *string, int len){
    enum type type = getType(string, len);
    struct token token = {type, string};
    return token;
}


//very dangerous return value on failure
//explicitly check failure with type == -1
enum type getType(char *string, int len){
    if (isNumber(string)) return I;
    else{
        if (len > 1){
            for (int i = 0; i < 6; i++){
                if (strcmp(string, keywords[i])){
                    if (i = 0) return NOT; else return F;}
            }
            return V;
        } 
        //either f or v
        else{
            int ascii = (int) *string;
            enum type typ = uniTypes[ascii];
            if (typ == -1) return -1;
            else return typ;
        }
    }
}

int isNumber(char *string){
    while (*string != '\0'){
        if (isdigit(*string) == 0) return 0;
    }
    return 1;
}



int main(){

}
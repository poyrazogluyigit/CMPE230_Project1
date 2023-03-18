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


const char *strings[] = {"S", "V", "E", "F", "I", "NOT", "AND", "OR", "MULT", "ADD", "COMM", "SUB", "OBR", "CBR", "EQ"};

char *uniKeys[] = {",","-","","|","","&","=","(",")","*","+"};
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
    //char tokSt[len+1];
    //strncpy(tokSt, string, len+1);
    struct token token;
    token.type = type;
    strncpy(token.value, string, len+1);
    return token;
}

//code used for unit testing
regex_t regex;
regmatch_t match[1];

int regexVal;
char msgbuf[257];

struct token tokens[256];

int main(){

    regexVal = regcomp(&regex, "[a-zA-Z0-9]+|[^[:alnum:]]", REG_EXTENDED);

    if (regexVal) {
    fprintf(stderr, "Regex compilation error\n");
    return 1;
}

    while (fgets(msgbuf, sizeof(msgbuf), stdin)){
        if (msgbuf == NULL) break;
        

        regexVal = regexec(&regex, msgbuf, 1, match, 0);

        int lastPtr = 0;
        
        int i = 0;
        while (regexVal == 0){
            int startBuffer = match[0].rm_so;
            int endBuffer = match[0].rm_eo;
            int len = endBuffer - startBuffer;

            char tokenStr[len+1];
            strncpy(tokenStr, msgbuf+lastPtr+startBuffer, len);
            tokenStr[len] = '\0';
            if (!isspace((int) tokenStr[0])){
                struct token token = tokenize(tokenStr, len);
                tokens[i] = token;
                i++;
            }

            lastPtr += endBuffer;
            regexVal = regexec(&regex, msgbuf + lastPtr, 1, match, 0);
        }

        for (int j = 0; j < i; j++){
            struct token tok = tokens[j];
            if (0<tok.type<15){
                const char *type = strings[tok.type];
                printf("type: %s value: %s\n", type, tok.value);
            }
        }
    }
    regfree(&regex);
    return 0;

}
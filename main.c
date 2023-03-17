#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <ctype.h>
       
regex_t regex;
regmatch_t match[5];


int reti;
char msgbuf[100];



int isNumber(char *string){
    while (*string != '\0'){
        if (isdigit(*string) == 0) return 0;
        string++;
    }
    return 1;
}

int main(){

fgets(msgbuf, sizeof(msgbuf), stdin);

/* Compile regular expression */
reti = regcomp(&regex, "[a-zA-Z0-9]+|[^[:alnum:]]", REG_EXTENDED);
if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    return 1;
}

/* Execute regular expression */
reti = regexec(&regex, msgbuf, 1, match, 0);
int last = 0;

while (reti == 0){
    int start = match[0].rm_so;
    int finish = match[0].rm_eo;
    int len = finish - start;
    char str[len + 1];
    str[len] = '\0';
    strncpy(str,msgbuf + last + start, len);
    printf("%.*s\n", len, msgbuf + last + start); 
    if(isNumber(str)) puts("yes");
    else puts("no");

    last += finish;
    reti = regexec(&regex, msgbuf+last, 1, match, 0);
}

if (reti == REG_NOMATCH) {
    puts("No match");
}
else {
    regerror(reti, &regex, msgbuf, sizeof(msgbuf));
    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
    return 1;
}

/* Free memory allocated to the pattern buffer by regcomp() */
regfree(&regex);

return 0;
}


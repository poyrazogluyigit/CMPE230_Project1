#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct mapElement {
    char *key;
    long long value;
} mapElement;

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

int main(){
    //call the initialization function
    struct mapElement* variables = initMap(10);

    int noelem = 0;

    srand(0);

    for (int i = 0; i < 1000; i++){
        int s = rand();
        put(variables, noelem, "" + i, s);
        noelem++;
    }

    for (int i = 0; i < 1000; i++){
        printf("%d", get(variables, "" + i));
    }





}
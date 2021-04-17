

#include <stdlib.h>
#include <stdio.h>

// перевод в двоичную

char* Translation(long x){
    int size = sizeof(x) * 8;
    char* str = (char*)malloc(sizeof(char) * (size+1));
    str[0] = '\0';
    if(x < 0){
        return str;
    }
    for(int i = size - 1; i >= 0; i--){
        str[size - i - 1] = ((x >> i) & 1) + '0';
    }

    return str;
}

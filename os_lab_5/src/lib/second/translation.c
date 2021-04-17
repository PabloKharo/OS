

#include <stdlib.h>
#include <stdio.h>

// перевод в троичную систему

char* Translation(long x){
    int size = sizeof(x) * 8;
    char* str = (char*)malloc(sizeof(char) * (size+1));
    str[0] = '\0';
    if(x < 0)
    {
        return str;
    }
    for(int i = 0; i <= size; i++){
        str[i] = '0';
    }

    int tmp = x;
    int length = 0;
    while(tmp > 0){
        tmp /= 3;
        length++;
    }
    tmp = x;
    for(int i = size; i > size - length; i--){
        str[i] = (tmp % 3) + '0';
        tmp /= 3;
    }
    
    return str;   
}

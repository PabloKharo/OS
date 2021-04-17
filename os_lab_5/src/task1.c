#include <stdio.h>
#include <stdlib.h>

#include "lib/first/first.h"

int main(){
    int command;
    printf("> ");
    while(scanf("%d", &command) == 1)
    {
        if(command == 1){
            float A = 1;
            float deltaX = 1;
            if(scanf("%f %f", &A, &deltaX) != 2){
                break;
            }                
            printf("%f\n", Derivative(A, deltaX));
        }
        else if(command == 2){
            long x = 1;
            if(scanf("%ld", &x) != 1){
                break;
            }
            char* ans = Translation(x);
            if(ans[0] != '\0')
                printf("%s\n", ans);
            else
                printf("Under zero\n");
            free(ans); 
        }
        else{
            printf("Wrong command!\n"); 
        }
        printf("> ");

    }


}
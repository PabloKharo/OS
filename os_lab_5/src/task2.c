
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(){
    int lib = 0;
    char* libs[] = {"./src/lib/first/libFirst.so", "./src/lib/second/libSec.so"};

    float (*Derivative)(float A, float deltaX);
    char* (*Translation)(long x);

    void* dlHandle = dlopen(libs[lib], RTLD_LAZY);
    if (dlHandle == NULL){
        perror(dlerror());
        exit(-1);
    }

    int command;
    printf("> ");
    while (scanf("%d", &command) == 1) {
        if (command == 1) {
            float A, deltaX;
            if (scanf("%f %f", &A, &deltaX) != 2) {
                break;
            }
            Derivative = dlsym(dlHandle, "Derivative");
            if (Derivative == NULL){
                perror(dlerror());
                exit(-1);
            }
            printf("%f\n", Derivative(A, deltaX));
        } else if (command == 2) {
            long x;
            if (scanf("%ld", &x) != 1) {
                break;
            }
            Translation = dlsym(dlHandle, "Translation");
            if (Translation == NULL){
                perror(dlerror());
                exit(-1);
            }
            char* ans = Translation(x);
            if(ans[0] != '\0')
                printf("%s\n", ans);
            else
                printf("Under zero\n");
            free(ans);     
        } else if (command == 0){
            if (dlclose(dlHandle) != 0){
                perror(dlerror());
                exit(-1);
            }
            lib = (lib == 0 ? 1 : 0);
            dlHandle = dlopen(libs[lib], RTLD_LAZY);
            if (dlHandle == NULL){
                perror(dlerror());
                exit(-1);
            }
            printf("Changed to another library\n");
        }else{
            printf("Wrong command!\n");
        }
        printf("> ");
    }

    if (dlclose(dlHandle) != 0){
        perror(dlerror());
        exit(-1);
    }

    return 0;
}
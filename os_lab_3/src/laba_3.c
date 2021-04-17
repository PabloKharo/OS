// Посчитать детерминант матрицы

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "get_int/get_int.h"

#define ll long long

struct counts
{
    int n;
    int* matr;
    int firstI;
    int lastI;
    int j;
    ll* res;
};

ll Minor(int n, int* matr, int i, int j)
{
    if(n == 1)
        return 1;

    ll res = 0;
    
    int* new_matr = (int*)malloc(sizeof(int)* (n-1) *(n-1));
    if(new_matr == NULL)
        exit(-1);
    if(!new_matr)
    {
        perror("Make matrix");
        exit(-1);
    }
    int k = 0;
    for(int a = 0; a < n; ++a)
    {
        if(a == i)
            continue;
        for(int b = 1; b < n; ++b)
        {
            new_matr[k] = matr[a*n + b]; 
            ++k;
        }
    }
    for(int a = 0; a < n-1; ++a)
    {
        if(a % 2 == 0)
            res += new_matr[a*(n-1)] * Minor(n-1, new_matr, a, j);
        if(a % 2 == 1)
            res -= new_matr[a*(n-1)] * Minor(n-1, new_matr, a, j);
    }
    free(new_matr);
    return res;

}

void* CountThradeMinors(void* args)
{
    struct counts* arg = (struct counts*) args;

    for(int i = arg->firstI; i < arg->lastI; ++i)
    {
        if(i % 2 == 0)
            *(arg->res) += arg->matr[i * arg->n] * Minor(arg->n, arg->matr, i, arg->j);
        if(i % 2 == 1)
            *(arg->res) -= arg->matr[i * arg->n] * Minor(arg->n, arg->matr, i, arg->j);
    }

    pthread_exit(0);
}

ll CountMinors(int n, int* matr, int firstI, int lastI, int j)
{
    ll res = 0;
    for(int i = firstI; i < lastI; ++i)
    {
        if(i % 2 == 0)
            res += matr[i * n] * Minor(n, matr, i, j);
        if(i % 2 == 1)
            res -= matr[i * n] * Minor(n, matr, i, j);
    }

    return res;
}

int main(int argc, char* argv[])
{
    int thread = 1;
    if(argc == 2) 
        thread = (int)(*argv[1] - '0');

    int n;
    if(get_int(STDIN_FILENO, &n) != 0)
        exit(-1);

    int* matr = (int*)malloc(sizeof(int) * (n*n));
    if(!matr){
        exit(-1);
    }
    for(int i = 0; i < n*n; ++i)
    {
        if(get_int(STDIN_FILENO, &matr[i]) != 0)
            exit(-1);
    }

    pthread_t tids[thread];
    for(int i = 0; i < thread; ++i){
        tids[i] = 0;
    }

    ll results[thread];
    struct counts data[thread];
    int k = 0;

    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);
    for(int t = 0; t < thread; ++t)
    {
        data[t].n = n;
        data[t].matr = matr;
        data[t].firstI = k;

        k = k + n/(thread+1);

        data[t].lastI = k;
        data[t].j = 0;
        results[t] = 0;
        data[t].res = &results[t];

        if(data[t].lastI - data[t].firstI > 0)
        {
            //printf("thread: From %d to %d \n", data[t].firstI, data[t].lastI);
            if (pthread_create(&tids[t], NULL, CountThradeMinors, (void*) &data[t]) != 0) 
            {
                perror("Creating the thread");
                exit(-1);
            }
        }
    }

    
    //printf("main: From %d to %d \n", k, n);
    ll res = CountMinors(n, matr, k, n, 0);
    for(int t = 0; t < thread; ++t)
    {
        int stat;
        if (tids[t] != 0 && pthread_join(tids[t], (void**)&stat) != 0) 
        {
            perror("Joining the thread");
            exit(-1);
        }
        if(stat != 0){
            exit(-1);
        }
        res += results[t];
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double time_spent = ( end.tv_sec - begin.tv_sec ) + (double)( end.tv_nsec - begin.tv_nsec )/ (double)1000000000L;
    printf("res: %lld, time: %.4lf\n", res, time_spent);

    free(matr);

    return 0;
}
#include <stdlib.h>
#include <stdio.h>

#include <math.h>
// f'(x) = (f(A + deltaX) - f(A))/deltaX

float Derivative(float A, float deltaX)
{
    return (cosf(A+deltaX) - cosf(A))/deltaX;
}
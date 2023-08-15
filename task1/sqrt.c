#include <stdio.h>
#include <math.h>

void _sqrt(float a) {
    if (a < 0) printf("Error: %f < 0\n", a);
    else printf("sqrt(%f) = %f\n", a, sqrtf(a));
}

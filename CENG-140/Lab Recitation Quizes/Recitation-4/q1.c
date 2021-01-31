#include <stdio.h>
#include "recit_4_1.h"


int gcd(int a, int b){
    if (a > b){
        if (b > 0){
            printf("gcd %d %d\n",a,b);
            return gcd(b, a%b);
        }else{
            return a;
        }
    }else{
        if (a > 0){
            printf("gcd %d %d\n",a,b);
            return gcd(a, b%a);
        }else{
            return b;
        }
    }
    return 0;
}

int i = 1;
double c;
double power(int base, int exponent){
    if (exponent == 0 || exponent == 1){
        if (exponent == 1){
            printf("calculate: %d^%d call: %d\n",base,exponent,i);
            i = 1;
            return base;
        }else{
            printf("calculate: %d^%d call: %d\n",base,exponent,i);
            i = 1;
            return 1;
        }
    }
    else if (exponent %2 == 0){
        printf("calculate: %d^%d call: %d\n",base,exponent,i);
        i++;
        c = power(base,exponent/2);
        return c*c;
    }else{
        printf("calculate: %d^%d call: %d\n",base,exponent,i);
        i++;
        return base*power(base,exponent-1);
    } 
}

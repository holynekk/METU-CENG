#include <stdio.h>
#include "recit_4_2.h"


int combinations(int n, int k){
    if (n/2 < k){
        printf("(%d %d)\n",n,k);
        combinations(n,n-k);
    }else{
        if (k == 0){
            printf("(%d %d)\n",n,k);
            return 1;
        }else{
            printf("(%d %d)\n",n,k);
            return (long double)n/(long double)(k) *combinations(n-1,k-1);
            }
        }
}


double helper(double q, int it, int num, double p){
    double next = power(-1,num-1)*power(p,2*(num-1)+1)/factorial(2*(num-1) + 1);

    if (it < num){
        return q;
    }
    if (absolute(next) <= ERROR){
        return q+next;
            
    }else{
        if (num == 1){
            return helper(q,it,num+1,p);
        }else{
            printf("sin(%f) = %f at recursive call %d\n",p,q+next,num);
            q += next;
            return helper(q,it,num+1,p);
        }
    }
}


double sine(double x, int iter){
    double k = x;
    printf("sin(%f) = %f at recursive call %d\n",x,x,1);
    return helper(x,iter,1,k);    
    
}

#include "arrayReverse.h"

void reverse(int * arr[], int length){
    int k, *a;
    for (k = 0; k <= length/2 - 1; k++){
        a = *(arr+k);
        *(arr+k) = *(arr+length-1-k);
        *(arr+length-1-k) = a;
    }
    
}

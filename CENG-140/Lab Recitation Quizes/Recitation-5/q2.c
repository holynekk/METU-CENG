#include "pointerUpdate.h"

void update(int * arr[], int length){
    int k, i;
    int *pt[50];
    for (k = 0; k < length; k++){
        pt[k] = arr[*arr[k] % length];
    }
    for (i = 0; i < length; i++){
        arr[i] = pt[i];
    }
}

#include <stdio.h>
#include "loop.h"

int arr1[255], arr2[255], b, c, d, i = 0, loopc1 = 0, loopc2 = 0;
char a, arr3[255];

int loop_variable_value(char c){
	int k;
	for (k = 0; k < i; k++){
		if (c == arr3[k]){
			return arr1[3*k];
		}else;
	}return 0;
}

void loop_continue(char c){
	int k, t, y;
	for (k = 0; k < i; k++){
		if (c == arr3[k]){
			arr1[3*k] += arr1[3*k+2];
			for (y = k; y > 0; y--){
				if (arr1[3*y+2] < 0){
					if (arr1[3*y] < arr1[3*y+1]){
						arr1[3*y] = arr2[y];
						arr1[3*(y-1)] += arr1[3*(y-1)+2];
					}else;
				}else if(arr1[3*y+2] > 0){
					if (arr1[3*y] > arr1[3*y+1]){
						arr1[3*y] = arr2[y];
						arr1[3*(y-1)] += arr1[3*(y-1)+2];
					}else;
				}else;
			}for (t=k+1; t < i; t++){
				arr1[3*t] = arr2[t];
				loopc2 += 1;
			}break;
		}else;
	}
}

int main(void){
	int k;
	while (scanf(" %c%d%d%d",&a,&b,&c,&d) == 4){
        if ( (b < c && d < 0) || ((b > c) && d > 0 )){
            	return 0;
            	}else;
        arr1[3*i] = b;
        arr1[3*i+1] = c;
        arr1[3*i+2] = d;
        arr2[i] = b;
        arr3[i] = a;
        i++;
    }

    if (arr1[2] > 0){
        while (arr1[0] <= arr1[1]){
            loop_execute();
            if (loopc2 > loopc1){
                loopc1 = 0;
                loopc2 = 0;
                continue;
                }else;
            loopc1 = 0;
            loopc2 = 0;
            arr1[3*(i-1)] += arr1[3*(i-1)+2];
            for(k = i-1; k > 0; k--){
                if (arr1[3*k+2] < 0){
                    if (arr1[3*k] < arr1[3*k+1]){
                        arr1[3*k] = arr2[k];
                        arr1[3*(k-1)] += arr1[3*(k-1)+2];
                    }else;
                }else if(arr1[3*k+2] > 0){
                    if (arr1[3*k] > arr1[3*k+1]){
                        arr1[3*k] = arr2[k];
                        arr1[3*(k-1)] += arr1[3*(k-1)+2];
                    }else;
                }else;
            }
        }
    }else{
        while (arr1[0] >= arr1[1]){
            loop_execute();
            if (loopc2 > loopc1){
                loopc1 = 0;
                loopc2 = 0;
                continue;
                }else;
            loopc1 = 0;
            loopc2 = 0;
            arr1[3*(i-1)] += arr1[3*(i-1)+2];
            for(k = i-1; k > 0; k--){
                if (arr1[3*k+2] < 0){
                    if (arr1[3*k] < arr1[3*k+1]){
                        arr1[3*k] = arr2[k];
                        arr1[3*(k-1)] += arr1[3*(k-1)+2];
                    }else;
                }else if(arr1[3*k+2] > 0){
                    if (arr1[3*k] > arr1[3*k+1]){
                        arr1[3*k] = arr2[k];
                        arr1[3*(k-1)] += arr1[3*(k-1)+2];
                    }else;
                }else;
            }
        }
    }




    return 0;
}


#include <stdio.h>

int main ()
{
    int task;
    scanf("%d", &task);

    if(task == 1) {
        
        /* Solution for task 1.*/

        int number, i;
        long double balance;
        float er, rd;
        
        scanf("%Lf %f %f", &balance, &er, &rd);
        scanf("%d", &number);
        
        for (i = 0 ; i < number; i++) {
            float amount;
            char type, currency;
            scanf(" %c %c %f", &type, &currency, &amount);
            
            if (type == 'D'  && currency == 'D') {
                balance += (amount * rd);
                
            } else if (type == 'W' && currency == 'D') {
                balance -= (amount * rd);
                
            } else if (type == 'W' && currency == 'E') {
                balance -= (amount * er);
                
            } else if (type == 'D' && currency == 'E') {
                balance += (amount * er);
            }
        }
        
        printf("Final balance: %.2Lf TL.", balance);

    } else if(task == 2) {
        

        /* Solution for task 2.*/
        
        float penalty, total, val1, val2, val3, val4; 
        int crr1, crr2, crr3, crr4;
        char extype, subtype;
        
        scanf(" %c",&extype);
        
        if (extype == 'T'){
            scanf("%f",&penalty);
            scanf("%d %f %d %f %d %f %d %f",&crr1, &val1, &crr2, &val2, &crr3, &val3, &crr4, &val4);
            total = crr1*val1 - (40-crr1)*penalty*val1 + crr2*val2 - (40-crr2)*penalty*val2 + crr3*val3 - (40-crr3)*penalty*val3 + crr4*val4 - (40-crr4)*penalty*val4; 
            printf("T result: %.2f points.",total);
            
        } else if (extype == 'A'){
            scanf(" %c %f",&subtype,&penalty);
            if (subtype == 'E'){
                scanf("%d %f %d %f %d %f %d %f",&crr1, &val1, &crr2, &val2, &crr3, &val3, &crr4, &val4);
                total += crr1*val1 - (30-crr1)*penalty*val1 + crr2*val2 - (30-crr2)*penalty*val2 + crr3*val3 - (30-crr3)*penalty*val3 + crr4*val4 - (30-crr4)*penalty*val4; 
                printf("A-E result: %.2f points.",total);
                
            } else if (subtype == 'C' || subtype == 'O'){
                scanf("%d %f %d %f",&crr1,&val1,&crr2,&val2);
                total = crr1*val1 - (30-crr1)*penalty*val1 + crr2*val2 - (30-crr2)*penalty*val2;
                printf("A-%c result: %.2f points.",subtype,total);
            }
            
        }

    }

    return 0;
}

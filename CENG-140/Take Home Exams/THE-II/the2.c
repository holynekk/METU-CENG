#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void eval(char * right, char * left);

int truth = 0;

int truth_check(char * right, char * left){
	int i, k;
	int st1 = strlen(left);
	int st2 = strlen(right);
	if (st1 == 1 || st2 == 1){
		if (st1 == 1 && st2 == 1){
			if (*left == *right){
				return 1;
			}else;
		}else if(st1 == 1){
			for (k = 0; right[k]; k++){
				if (right[k-1] == ',' || right[k+1] == ','){
					if (right[k] <= 90 && right[k] >= 65){
						if (*left == right[k]){
							return 1;
						}else;
					}else;
				}else;
			}
		}else if (st2 == 1){
			for (i = 0; left[i]; i++){
				if (left[i-1] == ',' || left[i+1] == ','){
					if (left[i] <= 90 && left[i] >= 65){
						if (left[i] == *right){
							return 1;
						}else;
					}else;
				}else;
			}
		}
	}
	for (i = 0; left[i]; i++){
		if (left[i-1] == ',' || left[i+1] == ','){
			if (left[i] <= 90 && left[i] >= 65){
				for (k = 0; right[k]; k++){
					if (right[k-1] == ',' || right[k+1] == ','){
						if (right[k] <= 90 && right[k] >= 65){
							if (right[k] == left[i]){
								return 1;
							}else;
						}else;
					}else;
				}
			}else;
		}else;
	}
	return 0;
}

void compress(char * str){
	char * p = str;
	while (*str){
		if (*str != ' '){
			*p++ = *str;
		}str++;
	}*p++ = '\0';
	while (*p){
		*p++ = ' ';
	}
}

void right_adder(char * right, char *left, char * start, char * finish){
	int len = strlen(right);
	if (len == 0){
		if (start == finish){
			right[0] = *start;
		}else{
			while (start <= finish){
				right[len++] = *start;
				*start = ' ';
				start++;
			}right[len] = '\0';
		}
	}else{
		if (start == finish){
			right[len++] = ',';
			right[len] = *start;
		}else{
			right[len++] = ',';
			while (start <= finish){
				right[len++] = *start;
				*start = ' ';
				start++;
			}right[len] = '\0';
		}
	}
}

void left_adder(char * right, char *left, char * start, char * finish){
	int len = strlen(left);
	if (len == 0){
		if (start == finish){
			left[0] = *start;
		}else{
			while (start <= finish){
				left[len++] = *start;
				*start = ' ';
				start++;
			}left[len] = '\0';
		}
	}else{
		if (start == finish){
			left[len++] = ',';
			left[len] = *start;
		}else{
			left[len++] = ',';
			while (start <= finish){
				left[len++] = *start;
				*start = ' ';
				start++;
			}left[len] = '\0';
		}
	}
}

char * p_matcher(char * str){
	int a = 1;
	while (1){
		if (*str == '(') str++, a++;
		else if (*str == ')') str++, a--;
		else str++; 
		if (a == 0){
			str--;
			break;
		}
	}return str;
}	

void negation_stack(char * str){
	str++;
	for (; *str; str++){
		if (*str == '-' && *(str-1) == '-'){
			*str = ' ';
			*(str-1) = ' ';
		}else;
	}
}

int is_p_in(char * str){
	for (; *str; str++){
		if (*str == '('){
			return 1;
		}else;
	}return 0;
}

char * op_finder(char *str){
	if (*(str+1) <= 90 && *(str+1) >= 65){
		return str+2;
	}else if (*(str+1) == '-'){
		str++;
		if (*(str+1) <= 90 && *(str+1) >= 65){
			return str+2;	
		}else if(*(str+1) == '('){
			return p_matcher(str+2)+1;
		}
	}else if (*(str+1) == '('){
		return p_matcher(str+2)+1;
	}
}

void not_left(char * left, char * right){
	char *lp;
	int n;
	if ( left[0] == '-' ){
		if (left[1] == '('){
			right_adder(right, left, (left+1), p_matcher(left+2));
			left[0] = ' ';
		}else{
			right_adder(right, left, (left+1), (left+1));
			left[0] = ' ', left[1] = ' ', left[2] = ' ';
		}
	}else;
	for (n = 1; left[n]; n++){
		if (left[n] == '-' && left[n-1] == ','){
			if (left[n+1] == '('){
				lp = p_matcher((left+n+2));
				right_adder(right,left,(left+n+1),lp);
				*(left+n) = ' ', *(left+n-1) = ' ';
			}else{
				right_adder(right,left,(left+n+1),(left+n+1));
				*(left+n+1) = ' ', *(left+n) = ' ', *(left+n-1) = ' ';
			}
		}else;
	}if (right[0] == ','){
		right[0] = ' ';
	}else;
	compress(left);
	compress(right);
}

void not_right(char * left, char * right){
	char * rp;
	int n;
	if ( right[0] == '-'){
		if (right[1] == '('){
			left_adder(right, left, (right+1), p_matcher(right+2));
			right[0] = ' ';
		}else{
			left_adder(right, left, (right+1), (right+1));
			right[0] = ' ', right[1] = ' ', right[2] = ' ';
		}
	}else;
	for (n = 1; right[n]; n++){
		if (right[n] == '-' && right[n-1] == ','){
			if (right[n+1] == '('){
				rp = p_matcher((right+n+2));
				left_adder(right,left,(right+n+1),rp);
				*(right+n) = ' ', *(right+n-1) = ' ';
			}else{
				left_adder(right,left,(right+n+1),(right+n+1));
				*(right+n+1) = ' ', *(right+n) = ' ', *(right+n-1) = ' ';
			}
		}else;
	}if (left[0] == ','){
		left[0] = ' ';
	}else;
	compress(left);
	compress(right);
}

int imp_left(char *str, char * right, char * left){
	int n = 0, k = 0, t = 0;
	int len_left = 0, len_right = 0;
	char * op = op_finder(str);
	char *m1 = str, *m2 = op;
	char *left2 , *right2, *rm2;
	str++, op++;
	if (*str == '-'){
		len_left = (((op-str)/sizeof(char)) - 1);
	}else if(*str == '('){
		len_left += (( (p_matcher(str+1)-str)/sizeof(char) )+1);
	}else if(*str <= 90 && *str >= 65){
		len_left = 1;
	}else;
	
	if (*op == '-'){
		op++;
		if(*op == '('){
			len_right += ((( p_matcher(op+1)-op )/sizeof(char) )+2);
		}else if(*op <= 90 && *op >= 65){
			len_right = 2;
		}
	}else if(*op == '('){
		len_right += (( (p_matcher(op+1)-op)/sizeof(char) )+1);
	}else if(*op <= 90 && *op >= 65){
		len_right = 1;
	}else;
	left2 = (char *)malloc(200*sizeof(char));
	right2 = (char *)malloc(200*sizeof(char));
	rm2 = left2 + (m1 - left);
	while (n < strlen(right)){
		right2[n] = right[n];
		n++;
	}right2[n] = '\0';
	n = 0;
	while (n < strlen(left)){
		left2[n] = left[n];
		n++;
	}left2[n] = '\0';
	if (*str == '-'){
		*m1 = ' ', *str = ' ';
	}else{
		*m1 = '-';
	}
	while (k < len_right+2){
		*m2 = ' ';
		m2++;
		k++;
	}*rm2++ = ' ';
	while (t < len_left+1){
		*rm2++ = ' ';
		t++;
	}*(rm2+len_right) = ' ';
	compress(left2);
	eval(right2,left2);
	free(right2);
	free(left2);
	return 0;
}

int imp_right(char *str, char * right, char * left){
	int len_left = 0, len_right = 0;
	char * l1 = str;
	char *op = op_finder(str);
	str++, op++;
	if (*str == '-'){
		len_left = (((op-str)/sizeof(char)) - 1);
	}else if(*str == '('){
		len_left += (( (p_matcher(str+1)-str)/sizeof(char) )+1);
	}else if(*str <= 90 && *str >= 65){
		len_left = 1;
	}else;
	
	if (*op == '-'){
		len_right++;
		if(*(op+1) == '('){
			len_right += ((( p_matcher(op+2)-op )/sizeof(char) )+1);
		}else if(*(op+1) <= 90 && *(op+1) >= 65){
			len_right++;
		}
	}else if(*op == '('){
		len_right += (( (p_matcher(op+1)-op)/sizeof(char) )+1);
	}else if(*op <= 90 && *op >= 65){
		len_right = 1;
	}else;
	
	if (*str == '-'){
		*l1 = ' ', *str = ' ';
	}else{
		*l1 = '-';
	}*(str+len_left) = ',';
	*(op+len_right) = ' ';
}

int and_right(char * str, char * right, char * left){
	int n = 0, k = 0, t = 0;
	int len_left = 0, len_right = 0;
	char * op = op_finder(str);
	char *m1 = str, *m2 = op;
	char *left2 , *right2, *rm2;
	str++, op++;
	if (*str == '-'){
		len_left = (((op-str)/sizeof(char)) - 1);
	}else if(*str == '('){
		len_left += (( (p_matcher(str+1)-str)/sizeof(char) )+1);
	}else if(*str <= 90 && *str >= 65){
		len_left = 1;
	}else;
	
	if (*op == '-'){
		op++;
		if(*op == '('){
			len_right += ((( p_matcher(op+1)-op )/sizeof(char) )+2);
		}else if(*op <= 90 && *op >= 65){
			len_right = 2;
		}
	}else if(*op == '('){
		len_right += (( (p_matcher(op+1)-op)/sizeof(char) )+1);
	}else if(*op <= 90 && *op >= 65){
		len_right = 1;
	}else;
	
	left2 = (char *)malloc(200*sizeof(char));
	right2 = (char *)malloc(200*sizeof(char));
	rm2 = right2 + (m1 - right) ;
	while (n < strlen(left)){
		left2[n] = left[n];
		n++;
	}left2[n] = '\0';
	n = 0;
	while (n < strlen(right)){
		right2[n] = right[n];
		n++;
	}*m1 = ' ';
	while (k < len_right+2){
		*m2 = ' ';
		m2++;
		k++;
	}
	*rm2++ = ' ';
	while (t < len_left+1){
		*rm2++ = ' ';
		t++;
	}*(rm2+len_right) = ' ';
	compress(right2);
	eval(right2,left2);
	free(right2);
	free(left2);
	return 0;
}

int or_left(char * str, char * right, char * left){
	int n = 0, k = 0, t = 0;
	int len_left = 0, len_right = 0;
	char * op = op_finder(str);
	char *m1 = str, *m2 = op;
	char *left2 , *right2, *rm2;
	str++, op++;
	if (*str == '-'){
		len_left = (((op-str)/sizeof(char)) - 1);
	}else if(*str == '('){
		len_left += (( (p_matcher(str+1)-str)/sizeof(char) )+1);
	}else if(*str <= 90 && *str >= 65){
		len_left = 1;
	}else;
	
	if (*op == '-'){
		op++;
		if(*op == '('){
			len_right += ((( p_matcher(op+1)-op )/sizeof(char) )+2);
		}else if(*op <= 90 && *op >= 65){
			len_right = 2;
		}
	}else if(*op == '('){
		len_right += (( (p_matcher(op+1)-op)/sizeof(char) )+1);
	}else if(*op <= 90 && *op >= 65){
		len_right = 1;
	}else;
	left2 = (char *)malloc(200*sizeof(char));
	right2 = (char *)malloc(200*sizeof(char));
	rm2 = left2 + (m1 - left) ;
	while (n < strlen(right)){
		right2[n] = right[n];
		n++;
	}right2[n] = '\0';
	n = 0;
	while (n < strlen(left)){
		left2[n] = left[n];
		n++;
	}left2[n] = '\0';
	*m1 = ' ';
	while (k < len_right+2){
		*m2 = ' ';
		m2++;
		k++;
	}
	*rm2++ = ' ';
	while (t < len_left+1){
		*rm2++ = ' ';
		t++;
	}*(rm2+len_right) = ' ';
	compress(left2);
	eval(right2,left2);
	free(right2);
	free(left2);
	return 0;
}

void and_left(char * str, char * left){
	char *next = p_matcher(str+1);
	*(op_finder(str)) = ',';
	*str = ' ';
	*next = ' ';
	compress(left);
}

void or_right(char * str, char * right){
	char *next = p_matcher(str+1);
	*(op_finder(str)) = ',';
	*str = ' ';
	*next = ' ';
	compress(right);
}

void eval(char * right, char * left){
	char *r1 = right, *l1 = left;
	do{
		if (truth_check(right, left)){
			/*Nice Hoccam*/
			break;
		}else{
			if (is_p_in(left)){
				while (*l1){
					if (*l1 == '('){
						switch( *(op_finder(l1)) ){
							case '&':
								and_left(l1, left);
								break;
							case '|':
								or_left(l1,	right, left);
								compress(left);
								break;
							case '>':
								imp_left(l1,right,left);
								compress(left);
								break;
						}break;
					}else;
					l1++;
				}
			}if (is_p_in(right)){
				while (*r1){
					if (*r1 == '('){
						switch( *(op_finder(r1)) ){
							case '|':
								or_right(r1, right);
								break;
							case '&':
								and_right(r1, right, left);
								compress(right);
								break;
							case '>':
								imp_right(r1,right,left);
								compress(right);
								break;
						}break;
					}else;
					r1++;
				}
			}
		}
		not_right(left,right);
		not_left(left,right);
		r1 = right, l1 = left;
	}while (is_p_in(right) || is_p_in(left));
	if (truth_check(right, left)){
		/* Bi daa nice hoccam*/
	}else{
		truth++;
	}
}

int main(void){
	char * str, * left, * right;
	int i , k, m = 0;
	left = (char *)malloc(200*sizeof(char));
	right = (char *)malloc(200*sizeof(char));
	str = (char *)malloc(200*sizeof(char));
	fgets(str,200,stdin);
	negation_stack(str);
	compress(str);
	for (i = 0; str[i] != '#'; i++){
		left[i] = str[i];
	}
	for (k = i+1; str[k]; k++){
		right[m++] = str[k];
	}right[strlen(right)-1] = '\0';
	free(str);
	compress(left);
	compress(right);
	not_right(left,right);
	not_left(left,right);
	eval(right, left);
	if (truth){
		printf("F\n");
	}else printf("T\n");
	free(right);
	free(left);
	return 0;
}

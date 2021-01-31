#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

double chance = 1.0;
int no_op_stack=0, no_num_stack=0;
float num_stack[50];

float eval(float n1, float n2, char op){
	switch(op){
		case '+':
			return n1 + n2;
			break;
		case '-':
			return n1 - n2;
			break;
		case '*':
			return n1 * n2;
			break;
		case '/':
			return n1 / n2;
			break;
		case '^':
			return pow(n1, n2);
			break;
	}
}

struct type_op{
  char op;
  int prec;
  char astv;
}ops[]={
	{'^', 9, 'R'},
	{'/', 8, 'L'},
	{'*', 8, 'L'},
	{'+', 5, 'L'},
	{'-', 5, 'L'},
	{'(', 0, 'N'},
	{')', 0, 'N'}
};

char * p_matcher(char * str){
	int a = 1;
	while (1){
		if (*str == '(') a++;
		else if (*str == ')') a--; 
		if (a == 0){
			break;
		}str++;
	}return str;
}	

int is_digimal(char q) {
  if (isdigit(q) || q == '.') return 1;
  return 0;
}

struct type_op * op_finder(char q){
	int k;
	for(k = 0; k < sizeof(ops)/sizeof(ops[0]); ++k){
		if(ops[k].op == q){
			return (ops + k);
		}else;
	}return NULL;
}

struct type_op * op_stack[50];

void push_op_stack(struct type_op *op){
	op_stack[no_op_stack++]=op;
}

void push_num_stack(float num){
	num_stack[no_num_stack++]=num;
}

struct type_op *pop_op_stack(){
	return op_stack[--no_op_stack];
}
float pop_num_stack(){
	return num_stack[--no_num_stack];
}

typedef struct random_variable{
	char name;
	float lw_bnd, up_bnd;
	float prob[100];
	struct random_varible *next;
}random_variable, *rnd;
#define NAME(p) ((p)->name)
#define LW(p) ((p)->lw_bnd)
#define UP(p) ((p)->up_bnd)
#define RN(p) ((p)->prob)
#define NEXT(p) ((p)->next)

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

rnd read_input(int letter_number, int count_int){
	rnd start = NULL, new;
	int i, k;
	for (i = 0; i < letter_number; i++){
		new = malloc(sizeof(random_variable));
		scanf(" %c",&NAME(new));
		scanf("%f %f",&LW(new), &UP(new));
		for (k = 0; k < count_int; k++){
			scanf("%f",&RN(new)[k]);
		}
		NEXT(new) = start;
		start = new;	
	}return start;
}

void randomizer(rnd start, int letter_number, int count_int, float * randoms){
	rnd ex = start;
	float diff;
	int i, k;
	double ce;
	for (i = 0; i < 26; i++){
		randoms[i] = 0;
	}
	while (1){
		k = rand()%count_int + 1;
		ce = RN(ex)[k];
		chance *= ce;
		diff = UP(ex)-LW(ex);
		if (diff < 0){
			randoms[NAME(ex)-65] = LW(ex) + ((LW(ex) - UP(ex))/count_int)*(k-1) + ((float)rand()/(float)(RAND_MAX)) * (LW(ex) - UP(ex))/count_int;
		}else{
			randoms[NAME(ex)-65] = LW(ex) + ((UP(ex) - LW(ex))/count_int)*(k-1) + ((float)rand()/(float)(RAND_MAX)) * (UP(ex) - LW(ex))/count_int;
		}
		ex = NEXT(ex);
		if (ex == NULL) break;
    }
}

void op_eval(struct type_op *op)
{
    struct type_op *pop;
    float n1, n2;

    if(op->op == '('){
        push_op_stack(op);
        return;
    }else if(op->op == ')') {
        while(no_op_stack>0 && op_stack[no_op_stack-1]->op!='(') {
            pop = pop_op_stack();
            n1 = pop_num_stack();
            n2 = pop_num_stack();
            push_num_stack(eval(n2, n1, pop->op));
        }
        return;
    }
    if(op->astv=='R'){
        while(no_op_stack && op->prec<op_stack[no_op_stack-1]->prec){
            pop = pop_op_stack();
            n1 = pop_num_stack();
            n2 = pop_num_stack();
            push_num_stack(eval(n2, n1, pop->op));
        }
    }else{
        while(no_op_stack && op->prec <= op_stack[no_op_stack-1]->prec) {
            pop = pop_op_stack();
            n1 = pop_num_stack();
            n2 = pop_num_stack();
            push_num_stack(eval(n2, n1, pop->op));
		}
    }push_op_stack(op);
}

float dijkstra(char * expr, rnd start, int letter_number, int count_int){
	char *beg=NULL, *copy, *extra;
	float n1, n2, * randoms = (float*)malloc(26*sizeof(float));
	int k = 0;
	struct type_op fst_op={'X', 0, 'N'}, *op = NULL, *snd_op=&fst_op;
	
	randomizer(start, letter_number, count_int, randoms);
	while (*expr){
		if (!beg) {
			if ((op=op_finder(*expr))) {
				if (snd_op && (snd_op == &fst_op || snd_op->op != ')')) {
					if (op->op == '-'){
						op = op_finder('_');
					}else;
                }else;
                op_eval(op);
                snd_op = op;
            }else if (is_digimal(*expr)){
				beg = expr;
			}else if(*expr <= 90 && *expr >= 65){			
				beg = expr;
			}else{
				beg = expr;
				switch(*expr){
					case '~':
						expr = p_matcher(beg + 2)+1;
						break;
					case 'l':
						expr = p_matcher(beg + 3)+1;
						break;
					case 'c':
						expr = p_matcher(beg + 4)+1;
						break;
					case 's':
						if (*(expr+1) == 'q'){
							expr = p_matcher(beg + 5)+1;
						}else{
							expr = p_matcher(beg + 4)+1;
						}
						break;
				}expr--;
			}
        }else {
            if ((op=op_finder(*expr))) {
            	if (*beg <= 90 && *beg >= 65){
            		push_num_stack( randoms[(*beg)-65] );
            	}else if (is_digimal(*beg)){
            		push_num_stack(atof(beg));
            	}else{
            		switch(*beg){
						case '~':
							copy = (char *)malloc(200*sizeof(char));
							extra = p_matcher(beg + 2);
							for (; beg < extra; beg++, k++){
								copy[k] = *beg;
							}k = 0;
							push_num_stack((-1)*dijkstra(copy, start, letter_number, count_int));
							expr = extra;
							break;
						case 'l':
							copy = (char *)malloc(200*sizeof(char));
							extra = p_matcher(beg + 3);
							for (; beg < extra; beg++, k++){
								copy[k] = *beg;
							}k = 0;
							push_num_stack(log(dijkstra(copy, start, letter_number, count_int)));
							expr = extra;
							break;
						case 'c':
							copy = (char *)malloc(200*sizeof(char));
							extra = p_matcher(beg + 4);
							for (; beg < extra; beg++, k++){
								copy[k] = *beg;
							}k = 0;
							push_num_stack(cos(dijkstra(copy, start, letter_number, count_int)));
							expr = extra;
							break;
						case 's':
							beg = expr;
							if (*(expr+1) == 'q'){
								copy = (char *)malloc(200*sizeof(char));
								extra = p_matcher(beg + 5);
								for (; beg < extra; beg++, k++){
									copy[k] = *beg;
								}k = 0;
								push_num_stack(sqrt(dijkstra(copy, start, letter_number, count_int)));
								expr = extra;
								break;
							}else{
								copy = (char *)malloc(200*sizeof(char));
								extra = p_matcher(beg + 4);
								for (; beg < extra; beg++, k++){
									copy[k] = *beg;
								}k = 0;
								push_num_stack(sin(dijkstra(copy, start, letter_number, count_int)));
								expr = extra;
								break;
							}
							break;
					}
            	}
                beg=NULL;
                op_eval(op);
                snd_op=op;
            }
        }expr++;
    }if (beg){
		if (*beg <= 90 && *beg >= 65){
			push_num_stack( randoms[(*beg)-65] );
    	}else{
			push_num_stack(atof(beg));
		}
	}
    while(no_op_stack) {
        op=pop_op_stack();
        n1=pop_num_stack();  
        n2=pop_num_stack();
        push_num_stack(eval(n2, n1, op->op));
    }free(randoms);
    return pop_num_stack();
    }

int main(void){
	char * mem = (char *) malloc(200*sizeof(char)), * ex = mem;
	int count_int,letter_number = 0, *exist = (int*)malloc(26*sizeof(int)), yer;
	long int count_exp, i;
	rnd list;
	float num, upper, lower, bound, result, boo, spec;
	double prob_dis[100], total = 0;
	srand(time(NULL));
	fgets(mem,200,stdin);
	compress(mem);
	while (*ex){
		if (*ex <= 90 && *ex >= 65){
			exist[*ex-65]++;
		}else;
		ex++;
	}for (i = 0; i < 26; i++){
		if (exist[i] > 0){
			letter_number++;
		}else;
	}
	free(exist);
	scanf("%d %ld", &count_int, &count_exp);
	list = read_input(letter_number, count_int);
	num = dijkstra(mem, list, letter_number, count_int);
	lower = num, upper = num;
	for (i = 0; i < 1000; i++){
		num = dijkstra(mem, list, letter_number, count_int);
		if (num < lower){
			lower = num;
		}else if(num > upper){
			upper = num;
		}
	}
	boo = upper - lower;
	spec = boo/count_int;
	for (i = 0; i < count_exp; i++){
		num = dijkstra(mem, list, letter_number, count_int);
		result = num;
		if (num > lower && num < upper){
			yer = (int)((num-lower)/( spec ));
			prob_dis[yer] += chance;
		}else;
		chance = 1.0;
	}
	for (i = 0; i < count_int; i++){
		total += prob_dis[i];
	}for (i = 0; i < count_int; i++){
		prob_dis[i] /= total;
	}printf("%.3f %.3f ",lower, upper);
	for (i = 0; i < count_int; i++){
		printf("%.3f ",prob_dis[i]);
	}printf("\n");
	free(mem);
	return 0;
}

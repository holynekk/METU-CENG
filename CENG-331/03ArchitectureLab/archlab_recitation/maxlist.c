#include <stdio.h>

struct node {
    long value;
    struct node *next;
};

long find_max(const struct node *list)
{ 
    long max = 0;
    while (list) {
        long value = list->value;
        if (value > max)
            max = value;
        list = list->next;
    }
    return max;
}

int main(void)
{
    // [3, 7, 35, 22, 17]
    struct node n4 = { 17, NULL };
    struct node n3 = { 22, &n4 };
    struct node n2 = { 35, &n3 };
    struct node n1 = { 7, &n2 };
    struct node list_head = { 3, &n1 };

    printf("Max of empty list: %ld\n", find_max(NULL));
    printf("Max of list: %ld\n", find_max(&list_head));

    return 0;
}


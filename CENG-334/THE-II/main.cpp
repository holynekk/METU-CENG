#include <pthread.h>
#include <iostream>

#include "hw2_output.h"

// void *worker_thread(void *arg)
// {
//     printf("This is worker_thread()\n");
//     pthread_exit(NULL);
// }

using namespace std;

int main(void)
{
    hw2_init_output();

    int n, m, k;

    // First matrix input ---------------------------------------------
    cin >> m >> n;
    int **first_matrix = new int *[m];
    for (int i = 0; i < m; ++i)
        first_matrix[i] = new int[n];

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cin >> first_matrix[i][j];
        }
    }

    // Second matrix input ---------------------------------------------
    cin >> m >> n;
    int **second_matrix = new int *[m];
    for (int i = 0; i < m; ++i)
        second_matrix[i] = new int[n];

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cin >> second_matrix[i][j];
        }
    }

    // Third matrix input ---------------------------------------------
    cin >> m >> k;
    int **third_matrix = new int *[m];
    for (int i = 0; i < m; ++i)
        third_matrix[i] = new int[k];

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < k; j++)
        {
            cin >> third_matrix[i][j];
        }
    }

    // Fourth matrix input ---------------------------------------------
    cin >> m >> k;
    int **fourth_matrix = new int *[m];
    for (int i = 0; i < m; ++i)
        fourth_matrix[i] = new int[k];

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < k; j++)
        {
            cin >> fourth_matrix[i][j];
        }
    }

    // pthread_t my_thread;
    // int ret;

    // printf("In main: creating thread\n");
    // ret = pthread_create(&my_thread, NULL, &worker_thread, NULL);
    // if (ret != 0)
    // {
    //     printf("Error: pthread_create() failed\n");
    //     exit(EXIT_FAILURE);
    // }

    // pthread_exit(NULL);
}
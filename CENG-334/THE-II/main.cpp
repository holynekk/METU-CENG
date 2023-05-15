#include <pthread.h>
#include <iostream>
#include <mutex>

#include "hw2_output.h"

using namespace std;

mutex mtx;

typedef struct
{
    int matrix_id;
    int **matrix1;
    int **matrix2;
    int **sum_matrix;
    int col_num;
    int row;
} sum_args;

void *sum_thread(void *arg)
{
    sum_args *args = (sum_args *)arg;

    for (int i = 0; i < args->col_num; i++)
    {
        int result = args->matrix1[args->row][i] + args->matrix2[args->row][i];
        args->sum_matrix[args->row][i] = result;
        hw2_write_output(args->matrix_id, args->row + 1, i + 1, result);
    }
}

int main(void)
{
    hw2_init_output();

    int n, m, k;

    // First matrix input ---------------------------------------------
    cin >> m >> n;
    int first_matrix_rows = m;
    int first_matrix_cols = n;
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

    int **sum1_mat = new int *[m];
    for (int i = 0; i < m; ++i)
        sum1_mat[i] = new int[n];

    // Third matrix input ---------------------------------------------
    cin >> m >> k;
    int third_matrix_rows = m;
    int third_matrix_cols = k;
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

    int **sum2_mat = new int *[m];
    for (int i = 0; i < m; ++i)
        sum2_mat[i] = new int[k];

    int **result_mat = new int *[first_matrix_rows];
    for (int i = 0; i < m; ++i)
        result_mat[i] = new int[third_matrix_cols];

    pthread_t *sum1_threads = new pthread_t[first_matrix_rows];
    pthread_t *sum2_threads = new pthread_t[third_matrix_rows];

    for (int i = 0; i < first_matrix_rows; i++)
    {
        sum_args *thread_args = new sum_args;
        thread_args->matrix_id = 0;
        thread_args->matrix1 = first_matrix;
        thread_args->matrix2 = second_matrix;
        thread_args->sum_matrix = sum1_mat;
        thread_args->col_num = first_matrix_cols;
        thread_args->row = i;
        pthread_create(sum1_threads + i, NULL, sum_thread, (void *)thread_args);
    }

    for (int i = 0; i < third_matrix_rows; i++)
    {
        sum_args *thread_args = new sum_args;
        thread_args->matrix_id = 1;
        thread_args->matrix1 = third_matrix;
        thread_args->matrix2 = fourth_matrix;
        thread_args->sum_matrix = sum2_mat;
        thread_args->col_num = third_matrix_cols;
        thread_args->row = i;
        pthread_create(sum2_threads + i, NULL, sum_thread, (void *)thread_args);
    }

    // Join threads -------------------------------------------------------
    for (int i = 0; i < first_matrix_rows; i++)
    {
        pthread_join(sum1_threads[i], NULL);
    }

    for (int i = 0; i < third_matrix_rows; i++)
    {
        pthread_join(sum2_threads[i], NULL);
    }

    for (int i = 0; i < first_matrix_rows; i++)
    {
        for (int j = 0; j < third_matrix_cols; j++)
        {
            for (int k = 0; k < third_matrix_rows; k++)
            {
                result_mat[i][j] += sum1_mat[i][k] * sum2_mat[k][j];
            }
        }
    }

    for (int i = 0; i < first_matrix_rows; i++)
    {
        for (int j = 0; j < third_matrix_cols; j++)
        {
            cout << result_mat[i][j] << " ";
        }
        cout << endl;
    }
}
// Mert Kaan YILMAZ - 2381093

#include "pthread.h"
#include "semaphore.h"
#include <iostream>
#include <mutex>

#include "hw2_output.h"

using namespace std;

sem_t **mult_sem;

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

typedef struct
{
    int matrix_id;
    int **matrix1;
    int **matrix2;
    int **result_matrix;
    int col_num;
    int row_num;
    int row;
} mult_args;

int **m1;
int **m2;
int first_matrix_rows;
int first_matrix_cols;
int third_matrix_rows;
int third_matrix_cols;

bool all_row_n_col_calculated(int row, int col)
{

    for (int j = 0; j < first_matrix_cols; j++)
    {
        if (m1[row][j] == 0)
        {
            return false;
        }
    }
    for (int i = 0; i < third_matrix_rows; i++)
    {
        if (m2[i][col] == 0)
        {
            return false;
        }
    }
    return true;
}

void *sum_thread(void *arg)
{
    sum_args *args = (sum_args *)arg;

    for (int i = 0; i < args->col_num; i++)
    {
        mtx.lock();
        int result = args->matrix1[args->row][i] + args->matrix2[args->row][i];
        args->sum_matrix[args->row][i] = result;
        if (args->matrix_id == 0)
        {
            m1[args->row][i] += 1;
        }
        else
        {
            m2[args->row][i] += 1;
        }
        hw2_write_output(args->matrix_id, args->row + 1, i + 1, result);
        for (int i = 0; i < first_matrix_rows; i++)
        {
            for (int j = 0; j < third_matrix_cols; j++)
            {
                if (all_row_n_col_calculated(i, j))
                {
                    sem_post(&mult_sem[i][j]);
                }
            }
        }
        mtx.unlock();
    }
}

void *mult_thread(void *arg)
{
    mult_args *args = (mult_args *)arg;
    for (int k = 0; k < third_matrix_cols; k++)
    {
        sem_wait(&mult_sem[args->row][k]);
        for (int a = 0; a < first_matrix_cols; a++)
        {
            args->result_matrix[args->row][k] += args->matrix1[args->row][a] * args->matrix2[a][k];
        }
        hw2_write_output(args->matrix_id, args->row + 1, k + 1, args->result_matrix[args->row][k]);
    }
}

int main(void)
{
    hw2_init_output();

    int n, m, k;

    // First matrix input ---------------------------------------------
    cin >> m >> n;
    first_matrix_rows = m;
    first_matrix_cols = n;
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

    m1 = new int *[m];
    for (int i = 0; i < m; ++i)
        m1[i] = new int[n];

    // Third matrix input ---------------------------------------------
    cin >> m >> k;
    third_matrix_rows = m;
    third_matrix_cols = k;
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

    m2 = new int *[m];
    for (int i = 0; i < m; ++i)
        m2[i] = new int[k];

    int **result_mat = new int *[first_matrix_rows];
    for (int i = 0; i < first_matrix_rows; ++i)
        result_mat[i] = new int[third_matrix_cols];

    mult_sem = new sem_t *[first_matrix_rows];
    for (int i = 0; i < first_matrix_rows; ++i)
        mult_sem[i] = new sem_t[third_matrix_cols];

    for (int i = 0; i < first_matrix_rows; i++)
    {
        for (int j = 0; j < third_matrix_cols; j++)
        {
            sem_init(&mult_sem[i][j], 0, 0);
        }
    }

    pthread_t *sum1_threads = new pthread_t[first_matrix_rows];
    pthread_t *sum2_threads = new pthread_t[third_matrix_rows];
    pthread_t *mult_threads = new pthread_t[first_matrix_rows];

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

    for (int i = 0; i < first_matrix_rows; i++)
    {
        mult_args *thread_args = new mult_args;
        thread_args->matrix_id = 2;
        thread_args->matrix1 = sum1_mat;
        thread_args->matrix2 = sum2_mat;
        thread_args->result_matrix = result_mat;
        thread_args->col_num = third_matrix_cols;
        thread_args->row_num = third_matrix_rows;
        thread_args->row = i;
        pthread_create(mult_threads + i, NULL, mult_thread, (void *)thread_args);
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
        pthread_join(mult_threads[i], NULL);
    }

    for (int i = 0; i < first_matrix_rows; i++)
    {
        for (int j = 0; j < third_matrix_cols; j++)
        {
            cout << result_mat[i][j] << " ";
        }
        cout << endl;
    }

    // free first_matrix
    for (int i = 0; i < first_matrix_rows; i++)
    {
        delete[] first_matrix[i];
    }
    delete[] first_matrix;

    // free second_matrix
    for (int i = 0; i < first_matrix_rows; i++)
    {
        delete[] second_matrix[i];
    }
    delete[] second_matrix;

    // free third_matrix
    for (int i = 0; i < third_matrix_rows; i++)
    {
        delete[] third_matrix[i];
    }
    delete[] third_matrix;

    // free fourth_matrix
    for (int i = 0; i < third_matrix_rows; i++)
    {
        delete[] fourth_matrix[i];
    }
    delete[] fourth_matrix;

    // free m1 matrix
    for (int i = 0; i < first_matrix_rows; i++)
    {
        delete[] m1[i];
    }
    delete[] m1;

    // free m2 matrix
    for (int i = 0; i < third_matrix_rows; i++)
    {
        delete[] m2[i];
    }
    delete[] m2;

    // free mult_sem
    for (int i = 0; i < first_matrix_rows; i++)
    {
        delete[] mult_sem[i];
    }
    delete[] mult_sem;
}
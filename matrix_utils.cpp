#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h> 
#include <iostream>
#include "matrix_utils.h"




int** read_matrix(FILE* in_file, int* size)
{
    fscanf(in_file, "%d", size);
    int** matrix = (int**)malloc(*size * sizeof(int*));
    //read current matrix
    for (int i = 0; i < *size; i++)
    {
        matrix[i] = (int*)malloc(*size * sizeof(int));
        for (int j = 0; j < *size; j++)
            fscanf(in_file, "%d", &matrix[i][j]);

    }

    return matrix;
}

void write_matrix(FILE* f, int** matr, int m, int n)
{
    fprintf(f, "%d %d\n", m, n);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
           fprintf(f, "%d ", matr[i][j]);

        fprintf(f, "%\n");
    }
}

void print_matrix(int** matr, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            printf("%d ", matr[i][j]);

        printf("%\n");
    }
}



int* shift_row(int* row, int size)
{
    int shift_counter = 0;

    //count shift
    for (int i = 0; i < size; i++)
        if (row[i] > 0)
            shift_counter += 1;

    //form new line
    int* new_row = (int*)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
        new_row[(i + shift_counter) % size] = row[i];

    free(row);
    return new_row;
}




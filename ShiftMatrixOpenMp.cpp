#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h> 
#include <iostream>
#include "matrix_utils.h"


int main(int argc, char* argv[]) {

    FILE* in_file;

    in_file = fopen(argv[2], "r");
    if (in_file == NULL)
    {
        puts("Open in file error");
        return -1;
    }


    int size;
    int** matrix = read_matrix(in_file, &size);

    fclose(in_file);

    double time_spent = 0.0;


    //shift matrices

    int i;
    clock_t begin = clock();

#pragma omp parallel for private(i) num_threads(atoi(argv[1]))
    for (int i = 0; i < size; i++) {
        matrix[i] = shift_row(matrix[i], size);
    }

    clock_t end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("N threads: %d Time: %f\n", atoi(argv[1]), time_spent);


    FILE* out_file;

    out_file = fopen(argv[3], "w");
    if (out_file == NULL)
    {
        puts("Open out file error");
        return -1;
    }

    write_matrix(out_file, matrix, size, size);

    fclose(out_file);


    return 0;


}
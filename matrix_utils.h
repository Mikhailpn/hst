#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h> 
#include <iostream>



int** read_matrix(FILE* in_file, int* size);

void print_matrix(int** matr, int size);

void write_matrix(FILE* f, int** matr, int m, int n);

void write_flat_matrix(FILE* f, int* matr, int m, int n);

int* shift_row(int* row, int size);

int* read_flat_matrix(FILE* in_file, int size, int* res_matrix);

void print_flat_matrix(int size, int* matrix);




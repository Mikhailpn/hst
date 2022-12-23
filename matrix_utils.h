﻿#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h> 
#include <iostream>



int** read_matrix(FILE* in_file, int* size);

void print_matrix(int** matr, int size);

void write_matrix(FILE* f, int** matr, int m, int n);

int* shift_row(int* row, int size);




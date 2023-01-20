// ShifyMatrixMPI.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <stdio.h>
#include <mpi.h>
#include <fstream>

extern int* shift_matrix_cuda(int* matrix, int m, int size);

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

    //free(row);
    return new_row;
}

int* read_flat_matrix(FILE* in_file, int size, int* res_matrix)
{
    for (int i = 0; i < size * size; i++)
        fscanf(in_file, "%d", &res_matrix[i]);
       
    return res_matrix;
}

void print_flat_matrix(int size, int* matrix)
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
            printf("%d ", matrix[i * size + j]);
        printf("\n");
    }
}

void write_flat_matrix(FILE* f, int* matr, int m, int n)
{
    fprintf(f, "%d %d\n", m, n);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
            fprintf(f, "%d ", matr[i * n + j]);

        fprintf(f, "%\n");
    }
}



int main(int argc, char** argv)

{


	int myrank, nprocs;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);


	FILE* in_file;

	int* matrix = NULL;
	int size;

	in_file = fopen(argv[1], "r");
	if (in_file == NULL)
	{
		puts("Open in file error");
		return -1;
	}

	fscanf(in_file, "%d", &size);

	matrix = (int*)malloc(size * size * sizeof(int));

	if (myrank == 0) {
		matrix = read_flat_matrix(in_file, size, matrix);
	}


	fclose(in_file);

	int* sendcounts;    // array describing how many elements to send to each process
	int* displs;        // array describing the displacements where each segment begins

	sendcounts = (int*)malloc(nprocs * sizeof(int));
	displs = (int*)malloc(nprocs * sizeof(int));

	int batch_size = (size / nprocs + 1) * size;
	

	for (int i = 0; i < nprocs; i++) {
		displs[i] = i * batch_size;
		if (i == nprocs - 1)
			sendcounts[i] = size * size - batch_size * i;
		else
			sendcounts[i] = batch_size;
	}


	int* recv_batch = (int*)malloc(batch_size * sizeof(int));


	clock_t begin;
	clock_t end;

	if (myrank == 0) {
		begin = clock();
	}


	//MPI_Scatter(matrix, size, MPI_INT, recv_row, size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatterv(matrix, sendcounts, displs, MPI_INT, recv_batch, batch_size, MPI_INT, 0, MPI_COMM_WORLD);


	/*
	//loop through rows of batch
	for (int i = 0; i < sendcounts[myrank] / size; i++) {
		int* shifted_row = shift_row(recv_batch + i * size, size);

		//loop thtough elements of row
		for (int j = 0; j < size; j++)
			res_batch[i * size + j] = shifted_row[j];
		free(shifted_row);

	}
	*/

	int* res_batch = shift_matrix_cuda(recv_batch, sendcounts[myrank] / size, size);

	int* res_matrix = NULL;
	if (myrank == 0) {
		res_matrix = (int*)malloc(size * size * sizeof(int));
	}

	//MPI_Gatherv(shifted_row, size, MPI_INT, res_matrix, size, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Gatherv(res_batch, sendcounts[myrank], MPI_INT, res_matrix, sendcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
	
	free(res_batch);


	if (myrank == 0) {
		end = clock();
		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		printf("N procs: %d Time: %f\n", nprocs, time_spent);
	}
	
	if (myrank == 0) {
		free(matrix);
		matrix = res_matrix;
	}
	

	//MPI_Barrier(MPI_COMM_WORLD);


	FILE* out_file;

	out_file = fopen(argv[2], "w");
	
	if (out_file == NULL)
	{
		puts("Open out file error");
		return -1;
	}
	if (myrank == 0) {
		write_flat_matrix(out_file, matrix, size, size);
	}

	fclose(out_file);
	MPI_Finalize();
	return 0;
}

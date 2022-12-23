#include <stdio.h>
#include <cuda.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include "matrix_utils.h"

__global__ void kernel(int* matrix, int* res_matrix, const int size) {

	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	int cnt = 0;
	if (tid < size) {
		//count positive
		for (int i = 0; i < size; i++)
			if (matrix[tid * size + i] > 0)
				cnt += 1;

		//write shifted
		for (int i = 0; i < size; i++) {
			res_matrix[tid * size + ((i + cnt) % size)] = matrix[tid * size + i];
		}
	}
}



int main(int argc, char* argv[]) {

	FILE* in_file;

	int* matrix = NULL;
	int* res_matrix = NULL;
	int* g_matrix = NULL;
	int* g_res_matrix = NULL;
	int size;

	in_file = fopen(argv[1], "r");
	if (in_file == NULL)
	{
		puts("Open in file error");
		return -1;
	}

	fscanf(in_file, "%d", &size);

	matrix = (int*)malloc(size * size * sizeof(int));
	res_matrix = (int*)malloc(size * size * sizeof(int));

	matrix = read_flat_matrix(in_file, size, matrix);

	cudaMalloc((void**)&g_matrix, sizeof(int) * size * size);
	cudaMalloc((void**)&g_res_matrix, sizeof(int) * size * size);

	printf("CUDA allocated memory for result vector, matrix and input vector\n");

	cudaMemcpy(g_matrix, matrix, sizeof(int) * size * size, cudaMemcpyHostToDevice);
	cudaMemcpy(g_res_matrix, res_matrix, sizeof(int) * size * size, cudaMemcpyHostToDevice);
	printf("CUDA copied input matrix and vector\n");

	print_flat_matrix(size, matrix);

	printf("Running kernel with N = %d\n", size);


	kernel << <size, 1 >> > (g_matrix, g_res_matrix, size);
	

	cudaMemcpy(res_matrix, g_res_matrix, sizeof(int) * size * size, cudaMemcpyDeviceToHost);
	print_flat_matrix(size, res_matrix);

	cudaFree(g_matrix);
	cudaFree(g_res_matrix);


	FILE* out_file;

	out_file = fopen(argv[2], "w");
	if (out_file == NULL)
	{
		puts("Open out file error");
		return -1;
	}

	write_flat_matrix(out_file, res_matrix, size, size);

	free(matrix);
	free(res_matrix);

	fclose(out_file);

	return 0;
};

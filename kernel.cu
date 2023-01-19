#include <stdio.h>
#include <cuda.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include "matrix_utils.h"

__global__ void kernel(int* matrix, int* res_matrix, const int m, const int size) {

	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	int cnt = 0;
	if (tid < m) {
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



int* shift_matrix_cuda(int* matrix, int m, int size) {

	int* res_matrix = NULL;
	int* g_matrix = NULL;
	int* g_res_matrix = NULL;

	res_matrix = (int*)malloc(m * size * sizeof(int));


	cudaMalloc((void**)&g_matrix, sizeof(int) * m * size);
	cudaMalloc((void**)&g_res_matrix, sizeof(int) * m * size);


	printf("CUDA allocated memory for result vector, matrix and input vector\n");

	cudaMemcpy(g_matrix, matrix, sizeof(int) * m * size, cudaMemcpyHostToDevice);
	cudaMemcpy(g_res_matrix, res_matrix, sizeof(int) * m * size, cudaMemcpyHostToDevice);
	printf("CUDA copied input matrix and vector\n");


	printf("Running kernel with N = %d\n", size);


	kernel << <size, 1 >> > (g_matrix, g_res_matrix, m, size);
	
	cudaMemcpy(res_matrix, g_res_matrix, sizeof(int) * m * size, cudaMemcpyDeviceToHost);
	print_flat_matrix(size, res_matrix);

	cudaFree(g_matrix);
	cudaFree(g_res_matrix);

	return res_matrix;
};

/*
 ============================================================================
 Name        : parallel.c
 Author      : Kayel Serafim
 Version     :
 Copyright   :
 Description : Parallel version of BS following the domain decomposition model
 ============================================================================
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define PHASES_TAG 0

// 0 para desabilitar os prints e 1 para habilitar
#define DEBUG 1
// trabalho final com o valores 10.000, 100.000, 1.000.000
const int ARRAY_SIZE = 20;
// Percentual de números a serem trocados a cada iteração
const int PERC_TO_EXCHANGE = 50;

/*
 * Bubble sort algorithm.
 */
void bs(int n, int *vetor) {
	int c = 0, d, troca, trocou = 1;

	while ((c < (n - 1)) & trocou) {
		trocou = 0;
		for (d = 0; d < n - c - 1; d++)
			if (vetor[d] > vetor[d + 1]) {
				troca = vetor[d];
				vetor[d] = vetor[d + 1];
				vetor[d + 1] = troca;
				trocou = 1;
			}
		c++;
	}
}

/*
 * Fills an array in the worst case
 */
void populate(int n, int proc_n, int my_rank, int *vector) {
	int i;
	for (i = 0; i < n; i++) {
		vector[i] = (proc_n - my_rank) * n - i;
	}
}

/*
 * Print an array.
 */
void print(int n, int *vector) {
	int i;
	printf("[");
	for (i = 0; i < n; i++)
		printf(" %d ", vector[i]);
	printf("]\n");
}

int main(int argc, char **argv) {
	// Número de processos
	int proc_n;
	// Identificador do processo
	int my_rank;
	int left_element;
	// index for loops
	int i;
	// Tempo inicial, local e total
	double startTime, localTime, totalTime;
	MPI_Status status;

	// função que inicializa o MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
	MPI_Barrier(MPI_COMM_WORLD);

	int vector_size = ARRAY_SIZE / proc_n;
	int vect_ctrl[proc_n];

	startTime = MPI_Wtime();

	int last_proc = proc_n - 1;
	int size_exchange = vector_size * PERC_TO_EXCHANGE / 100;
	int *vector = (int*) malloc((size_exchange + vector_size) * sizeof(int));

	populate(vector_size, proc_n, my_rank, vector);
	printf("Percentual de troca %d%%(%d), vetor %d populado\n ", PERC_TO_EXCHANGE, size_exchange, my_rank);

#if DEBUG
	/* print unsorted array */
	print(vector_size, vector);
#endif

	int pronto = 1;
	while (pronto) {

		// ordeno vetor local
		bs(vector_size, vector);

		// verifico condição de parada
		// se não for np-1, mando o meu maior elemento para a direita
		if (my_rank < last_proc) {
			MPI_Send(&vector[vector_size - 1], 1, MPI_INT, my_rank + 1, PHASES_TAG, MPI_COMM_WORLD);
		}

		// se não for 0, recebo o maior elemento da esquerda
		if (my_rank > 0) {
			MPI_Recv(&left_element, 1, MPI_INT, my_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

			// comparo se o meu menor elemento é maior do que o maior elemento recebido (se sim, estou ordenado em relação ao meu vizinho)
			if (vector[0] > left_element) {
				vect_ctrl[my_rank] = 0;
			} else {
				vect_ctrl[my_rank] = 1;
			}
		}
		// compartilho o meu estado com todos os processos
		for (i = 0; i < proc_n; i++) {
			MPI_Bcast(&vect_ctrl[i], 1, MPI_INT, i, MPI_COMM_WORLD);
		}

		for (i = 0; i < proc_n; i++) {
			if (vect_ctrl[i] == 1) {
				pronto = 1;
				break;
			} else
				pronto = 0;
		}
		// se todos estiverem ordenados com seus vizinhos, a ordenação do vetor global está pronta ( pronto = TRUE, break)
		if (pronto == 0) {
			break;
		}

		// senão continuo
		// troco valores para convergir

		// se não for o 0, mando os menores valores do meu vetor para a esquerda
		if (my_rank > 0) {
			MPI_Send(vector, size_exchange, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
		}
		// se não for np-1, recebo os menores valores da direita
		if (my_rank < last_proc) {
			MPI_Recv(&vector[vector_size], size_exchange, MPI_INT, my_rank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

			// ordeno estes valores com a parte mais alta do meu vetor local
			bs(vector_size, vector + size_exchange);

#if DEBUG
			printf("Vetor %d, ordenação parcial ", my_rank);
			print(vector_size, vector);
#endif

			// devolvo os valores que recebi para a direita
			MPI_Send(&vector[vector_size], size_exchange, MPI_INT, my_rank + 1, PHASES_TAG, MPI_COMM_WORLD);
		}

		// se não for o 0, recebo de volta os maiores valores da esquerda
		if (my_rank > 0) {
			MPI_Recv(vector, size_exchange, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, &status);
		}
	}

#if DEBUG
	printf("Vetor %d ordenado ", my_rank);
	print(vector_size, vector);
#endif

	localTime = MPI_Wtime() - startTime;
	MPI_Reduce(&localTime, &totalTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	if (my_rank == 0) {
		printf("Time %f\n", totalTime);
	}

	free(vector);

	MPI_Finalize();
	return 0;
}

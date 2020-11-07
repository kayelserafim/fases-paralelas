/*
 ============================================================================
 Name        : parallel.c
 Author      : Kayel Serafim
 Version     :
 Copyright   :
 Description : Parallel version of BS following the parallel phases model
 ============================================================================
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// 0 para desabilitar os prints e 1 para habilitar
#define DEBUG 1
// trabalho final com o valores 10.000, 100.000, 1.000.000
#define ARRAY_SIZE 40

/*
 * Buble sort algorithm.
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
void populate(int n, int *vector) {
	int i;
	for (i = 0; i < n; i++) {
		vector[i] = n - i;
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
	int vector[ARRAY_SIZE];
	int proc_n;
	int my_rank;
	int tam_vetor;
	// Tempo inicial e final
	double t0, t1;
	MPI_Status status;

	// função que inicializa o MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

	printf("init array with worst case for sorting...");
	populate(ARRAY_SIZE, vector);
	printf("\nVetor populado.");

#if DEBUG
	/* print unsorted array */
	print(ARRAY_SIZE, vector);
#endif

	pronto = FALSE;

	while (!pronto) {

		// ordeno vetor local

		bs(ARRAY_SIZE, vector);

		// verifico condição de parada

		// se não for np-1, mando o meu maior elemento para a direita

		// se não for 0, recebo o maior elemento da esquerda

		// comparo se o meu menor elemento é maior do que o maior elemento recebido (se sim, estou ordenado em relação ao meu vizinho)

		// compartilho o meu estado com todos os processos

		MPI_Bcast (estado)

		// se todos estiverem ordenados com seus vizinhos, a ordenação do vetor global está pronta ( pronto = TRUE, break)

		// senão continuo

		// troco valores para convergir

		// se não for o 0, mando os menores valores do meu vetor para a esquerda

		// se não for np-1, recebo os menores valores da direita

		// ordeno estes valores com a parte mais alta do meu vetor local

		// devolvo os valores que recebi para a direita

		// se não for o 0, recebo de volta os maiores valores da esquerda

	}

#if DEBUG
	printf("\nVetor: ");
	/* print sorted array */
	print(ARRAY_SIZE, vector);
#endif

	return 0;
}

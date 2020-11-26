# Enunciado 3o Trabalho MPI: Fases Paralelas (FP)

O objetivo do trabalho é implementar, usando a biblioteca MPI, uma versão paralela seguindo o modelo fases paralelas, de um programa que ordena um grande vetor usando o algortimo Bubble Sort (o programa sequencial está no final desta especificação). Após implementado, o programa deve ser executado no cluster Grad com 2 nós para realização das medições de desempenho para um tamanho de vetor de 1.000.000 elementos (sem os prints de tela). Cada grupo (de no máximo dois integrantes) deve entregar um relatório em .pdf de uma página com a análise dos resultados e o código anexado (seguir modelo proposto).

Cada um dos processos é responsável por 1/np do vetor (neste caso 1/16 ou 1/32 com HT), que já pode ser gerado internamente em cada processo, sem a necessidade de gerar em um único processo e depois distribuir entre os restantes. Depois, o processamento segue as seguintes fases, que são executadas em paralelo por todos os processos até a conclusão da ordenação (por isso o nome do modelo):

* Ordenação local: cada processo ordena a sua parte do vetor global (usar o código da rotina BS abaixo);
* É feita uma verificação distribuída para identificar se o vetor global esta ordenado: cada processo envia o seu maior valor para o vizinho da direita, este compara com o seu menor valor e verifica se os dois processos estão ordenados entre si. Como todos os processos fazem esta operação, cada um sabe se está ordenado em relação ao vizinho da esquerda. Esta informação é distribuída para todos os processos com uma operação de comunicação coletiva (Broadcast). Se todos estiverem ordenados entre si, todos terminam;
* Se não, cada processo troca uma parte dos seus valores mais baixos com os mais altos do vizinho da esquerda. A ideia é empurrar os valores que estão foram de lugar para os processos corretos, e volta para a fase 1.


Os itens para avaliação são:

* execução da versão sequencial do algoritmo Bubble Sort para o vetor inteiro (inicializar com dados em ordem decrescente);
* implementação da versão paralela SPMD do algoritmo MPI descrito acima seguindo o modelo de fases paralelas;
medição dos tempos de execução para a versão sequencial em uma máquina qualquer do aluno ou laboratório e da versão paralela para 1.000.000 elementos (usando 2 nós exclusivos da máquina grad totalizando 16/32 (HT) processos);
* cálculo do speed up e da eficiência para os casos de teste;
* análise do desempenho deste modelo em relação ao de divisão e conquista para a mesma tarefa (tamanho de vetor);
* clareza do código (utilização de comentários e nomes de variáveis adequadas);
* relatório no formato .pdf com uma página (coluna dupla)

```
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 40      // trabalho final com o valores 10.000, 100.000, 1.000.000

void bs(int n, int * vetor)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
}
```

```
int main()
{
    int vetor[ARRAY_SIZE];
    int i;

    for (i=0 ; i<ARRAY_SIZE; i++)              /* init array with worst case for sorting */
        vetor[i] = ARRAY_SIZE-i;
   

    #ifdef DEBUG
    printf("\nVetor: ");
    for (i=0 ; i<ARRAY_SIZE; i++)              /* print unsorted array */
        printf("[%03d] ", vetor[i]);
    #endif

    bs(ARRAY_SIZE, vetor);                     /* sort array */


    #ifdef DEBUG
    printf("\nVetor: ");
    for (i=0 ; i<ARRAY_SIZE; i++)                              /* print sorted array */
        printf("[%03d] ", vetor[i]);
    #endif

    return 0;
}
```

Pseudocódigo FP

```
MPI_Init();

my_rank = MPI_Comm_rank();  // descobre o numero do processo atual (rank)
proc_n  = MPI_Comm_size();  // descobre o numero total de processos (np usado na execução)

// gero parte local do vetor (1/np avos do vetor global)

Inicializa ( vetor );     // ordem reversa

pronto = FALSE;

while ( !pronto )
   {

   // ordeno vetor local

      BubbleSort (vetor);

   // verifico condição de parada

      // se não for np-1, mando o meu maior elemento para a direita

      // se não for 0, recebo o maior elemento da esquerda

      // comparo se o meu menor elemento é maior do que o maior elemento recebido (se sim, estou ordenado em relação ao meu vizinho)

      // compartilho o meu estado com todos os processos

      MPI_Bcast ( estado )

      // se todos estiverem ordenados com seus vizinhos, a ordenação do vetor global está pronta ( pronto = TRUE, break)

      // senão continuo

   // troco valores para convergir

      // se não for o 0, mando os menores valores do meu vetor para a esquerda

      // se não for np-1, recebo os menores valores da direita

         // ordeno estes valores com a parte mais alta do meu vetor local

         // devolvo os valores que recebi para a direita

      // se não for o 0, recebo de volta os maiores valores da esquerda

   }

MPI_Finalize();
```

# Instalação do OpenMPI no Ubuntu 20.04

```
$ sudo apt-get update -y
```

```
$ sudo apt-get install -y openmpi-bin
```

# Configuração Eclipse para C/C++

```
$ whereis openmpi
openmpi: /usr/lib/x86_64-linux-gnu/openmpi /etc/openmpi /usr/share/openmpi
```

**OpenMPI path:** /usr/lib/x86_64-linux-gnu/openmpi/include

![diagram](images/PathsAndSymbols.png?raw=true "Caminho para configurar o build do MPI no Eclipse")


# Exemplos de compilação e execução

**Compilação C em máquina local:** gcc sequential.c -o sequential.o

**Execução C em máquina local:** ./sequential.o

**Compilação MPI em máquina local:** mpicc parallel.c -o parallel.o

**Execução MPI em máquina local:** mpirun --hostfile ./hostfile -np 4 ./parallel.o

**Execução no lad:** ladrun -np 16 /parallel.o

**Execução no lad:** ladrun -np 32 /parallel.o

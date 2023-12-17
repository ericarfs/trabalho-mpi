/*
TRABALHO PRÁTICO 2 - Paralelização com MPI
Disciplina: SSC0903 - Computação de Alto Desempenho
Estudantes: 
----Erica Ribeiro Filgueira dos Santos  - 11836351
----Susy da Costa Dutra                 - 12694007 
----Théo da Mota dos Santos             - 10691331

Para compilar:
    mpicc matriz1d.c -o matriz1d
Para rodar:
    mpirun -np <num_proc> matriz1d <num_linhas> <num_colunas>

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "matriz.h"


//Função para salvar o tempo calculado em um arquivo
void salvarArquivo(double time, int linhas, int colunas){
  char resultFileName[50] = "resultados/TempoParalelo_";

  char n[10];
  char m[10];

  sprintf(n, "%d", linhas);
  sprintf(m, "%d", colunas);

  //Criar nome do arquivo de entrada
  strcat(resultFileName, n);
  strcat(resultFileName, "x");
  strcat(resultFileName, m);

  FILE *resultFile = fopen(resultFileName, "a");

  fprintf(resultFile, "%f\n", time);

  fclose(resultFile);
}


int main(int argc, char *argv[]) {
    double start, end, time, global_time; 

    int ROWS = atoi(argv[1]);
    int COLS = atoi(argv[2]);
    
    int rank, nprocs;
    MPI_Init(&argc, &argv);
    
    start = MPI_Wtime(); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int *matriz;

    if(rank == 0){
        matriz = malloc(sizeof(int) * ROWS * COLS);
 
        for (int i = 0; i < ROWS * COLS; i++)
            matriz[i] = rand() % 16;

        for (int i = 0; i < ROWS * COLS; i++)
            printf("%d ", matriz[i]);
        printf("\n");
        //print_matriz(matriz, ROWS, COLS);
    }

    int chunk_size = ROWS / nprocs; // quantas linhas o processo local vai processar
    int majority_chunk_size = chunk_size; // para quando for impar o número de linhas

    if(ROWS % 2 != 0 && rank == nprocs - 1 && nprocs > 1) // assim o último processo "pega" a linha faltante, se o número de colunas for ímpar
        chunk_size += 1;

    int *local_chunk = malloc(sizeof(int) * chunk_size * COLS);

    int *send_counts = malloc(sizeof(int) * nprocs);
    int *displs  = malloc(sizeof(int) * nprocs);

    if(rank == 0){
        for(int i = 0; i < nprocs; i++){
            send_counts[i] = chunk_size * COLS;
            displs[i] = i * (ROWS / nprocs) * COLS;
        }

        if(ROWS % 2)
            send_counts[nprocs - 1] += COLS;
    }

    MPI_Scatterv(matriz, send_counts, displs, MPI_INT, local_chunk, chunk_size * COLS, MPI_INT, 0, MPI_COMM_WORLD);

    int local_max[chunk_size], local_min[chunk_size];
    for(int i = 0; i < chunk_size; i++){
        local_max[i] = local_chunk[i * COLS];
        local_min[i] = local_chunk[i * COLS];
    }

    int local_row_sums[ROWS], local_col_sums[COLS];
    for(int j = 0; j < COLS; j++) local_col_sums[j] = 0;
    for(int i = 0; i < ROWS; i++) local_row_sums[i] = 0;

    int local_sum = 0;

    for (int i = 0; i < chunk_size; i++) {
        for (int j = 0; j < COLS; j++) {
            int index = i * COLS + j;

            if (local_chunk[index] > local_max[i])
                local_max[i] = local_chunk[index];

            if (local_chunk[index] < local_min[i])
                local_min[i] = local_chunk[index];

            local_sum += local_chunk[index];

            local_row_sums[(rank * majority_chunk_size) + i] += local_chunk[index];

            local_col_sums[j] += local_chunk[index];
        }
    }

    int global_max, global_min;
    for (int i = 0; i < chunk_size; i++){
        MPI_Reduce(&local_max[i], &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
        MPI_Reduce(&local_min[i], &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
    }

    int global_sum;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    int global_row_sums[ROWS];
    for(int i = 0; i < ROWS; i++)
        MPI_Reduce(&local_row_sums[i], &global_row_sums[i], 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    int global_col_sums[COLS];
    for(int j = 0; j < COLS; j++)
        MPI_Reduce(&local_col_sums[j], &global_col_sums[j], 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Soma dos elementos de cada linha:\n");
        for (int i = 0; i < ROWS; i++) {
            printf("%d ", global_row_sums[i]);
        }
        printf("\n\n");

        printf("Soma dos elementos de cada coluna:\n");
        for (int i = 0; i < COLS; i++) {
            printf("%d ", global_col_sums[i]);
        }
        printf("\n");

        printf("Maior elemento da matriz: %d\n", global_max);
        printf("Menor elemento da matriz: %d\n\n", global_min);

        printf("Soma total da matriz: %d\n\n", global_sum);
    }

    if(rank == 0){
        free(matriz);
        free(local_chunk);
        free(send_counts);
        free(displs);
    }
    
    end = MPI_Wtime(); 

    time = end - start;

    MPI_Reduce(&time, &global_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if(rank == 0){  
        printf( "Elapsed time is %f\n", global_time);
        salvarArquivo(global_time, ROWS, COLS);
    }


    MPI_Finalize();

    return 0;
}

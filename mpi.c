/*
TRABALHO PRÁTICO 2 - Paralelização com MPI
Disciplina: SSC0903 - Computação de Alto Desempenho
Estudantes: 
----Erica Ribeiro Filgueira dos Santos  - 11836351
----Susy da Costa Dutra                 - 12694007 
----Théo da Mota dos Santos             - 10691331

Para compilar:
    mpicc -o main mpi.c -lm
Para rodar:
    mpirun -np <num_proc> main <num_linhas> <num_colunas>

*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "matriz.h"

#define TAM 16

//Função para salvar o tempo calculado em um arquivo
void salvarArquivo(double time, int linhas, int colunas, int nprocs){
  char resultFileName[70] = "resultados/TempoParalelo_";

  char n[10];
  char m[10];
  char p[2];

  sprintf(n, "%d", linhas);
  sprintf(m, "%d", colunas);
  sprintf(p, "%d", nprocs);

  //Criar nome do arquivo de entrada
  strcat(resultFileName, n);
  strcat(resultFileName, "x");
  strcat(resultFileName, m);
  strcat(resultFileName, "_NP");
  strcat(resultFileName, p);
  strcat(resultFileName, ".txt");


  FILE *resultFile = fopen(resultFileName, "a");

  fprintf(resultFile, "%f\n", time);

  fclose(resultFile);
}


int main( int argc, char **argv ){
  
  int numLinhas = atoi(argv[1]);
  int numColunas = atoi(argv[2]);

  int *matriz;
  int i = 0, j = 0, k = 0;
  int qtd_elem, rank, size,rec_size=0,*vetor_rec, root=0;
  int global_min, global_max;
  unsigned long global_sum;
  int sum_rows[numLinhas];
  int global_sum_cols[numColunas];
  

  double start, end, global_time; 

  MPI_Init( &argc, &argv ); 

  start = MPI_Wtime(); 

  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );

  

  qtd_elem = numLinhas*numColunas;

  vetor_rec=(int*)malloc(rec_size*sizeof(int));

  //Quantidade de elementos a serem enviados para cada processador
  int *send_counts, *displs;
  send_counts = (int*)malloc(sizeof(int)*size); 
  displs = (int*)malloc(sizeof(int)*size);

  int rows_rem = numLinhas;
  int procs_rem = size;
  int rows_per_proc; 
  float rows_aux;
  int sum = 0;

  //Calcular vetor de valores a serem enviados e deslocamentos
  for (int i = 0; i < size; i++) {
    rows_aux = (float)rows_rem/procs_rem;
    rows_per_proc = ceil(rows_aux);
    rows_rem -= rows_per_proc;
    procs_rem--;
      
    send_counts[i] = rows_per_proc * numColunas;

    displs[i] = sum;
    sum += send_counts[i];

  }

  //Gerar matriz pseudo aleatoria
  if(rank == root) {
    matriz = (int*)malloc(qtd_elem*sizeof(int));
    for (i = 0; i < qtd_elem; i++) {
        matriz[i] = rand()%16;
    }
  }

  rec_size = send_counts[rank];
  vetor_rec=(int*)malloc(rec_size*sizeof(int));

  int rows = send_counts[rank]/numColunas;
 

  MPI_Scatterv(matriz,send_counts,displs, MPI_INT, vetor_rec,rec_size,MPI_INT,root,MPI_COMM_WORLD);

  int local_min = vetor_rec[0];
  int local_max = vetor_rec[0];
  unsigned long local_sum = 0;
  int local_sum_rows = 0;
  int local_sum_cols[numColunas];
  k = displs[rank]/numColunas;
  j = 0;

  for (i = 0; i < rec_size; i++){
    //Soma de todos os valores do processador
    local_sum += vetor_rec[i];

    //Busca do menor valor local
    if (vetor_rec[i] <= local_min){
      local_min = vetor_rec[i];
    }

    //Busca do maior valor local
    else if (vetor_rec[i] >= local_max){
      local_max = vetor_rec[i];
    }

    //Soma das linhas
    if (j<numColunas){
      local_sum_rows += vetor_rec[i];
      j++;
    }
    if (j==numColunas){
      sum_rows[k] = local_sum_rows;
      k++;
      local_sum_rows = 0;
      j = 0;
    }
      
  }

  //Soma das colunas dos vetores do processador
  for(i=0;i<numColunas;i++){
    local_sum_cols[i] = 0;
    for(j=0;j<rows;j++){
      local_sum_cols[i] += vetor_rec[j*numColunas+i]; 
    }

  }


  for (i=0;i<rows;i++){
    k = displs[rank]/numColunas + i;
    printf("A soma dos elementos da linha %d é: %d\n", k, sum_rows[k]);
    
  }


  MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);
  MPI_Reduce(&local_sum_cols, &global_sum_cols, numColunas, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
  MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, root, MPI_COMM_WORLD);
  MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, root, MPI_COMM_WORLD);


  if(rank == root){   
    for (i=0;i<numColunas;i++){
      printf("A soma dos elementos da coluna %d é: %d\n", i, global_sum_cols[i]);
    }
    printf("O maior elemento da matriz é: %d\n", global_max);
    printf("O menor elemento da matriz é: %d\n", global_min);
    printf("A soma de todos os elementos da matriz é: %ld\n", global_sum);

    
  }


	
  free(matriz);
  free(vetor_rec);
  free(send_counts);
  free(displs);

  end = MPI_Wtime(); 


  double time = end - start;

  MPI_Reduce(&time, &global_time, 1, MPI_DOUBLE, MPI_MAX, root, MPI_COMM_WORLD);

  if(rank == root){  
    printf( "Elapsed time is %f\n", global_time);
    salvarArquivo(global_time, numLinhas, numColunas, size);
  }


 
  MPI_Finalize();
}

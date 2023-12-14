#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "matriz.h"

#define TAM 16
 
int main( int argc, char **argv ){

  int numLinhas = atoi(argv[1]);
  int numColunas = atoi(argv[2]);

  int matriz[numLinhas][numColunas];
  int i, j, k;
  int qtd_elem, rank, size,rec_size=0,*vetor_rec, root=0;
  int global_min, global_max, local_sum = 0, global_sum = 0;
  int local_sum_rows[numLinhas];
  int global_sum_cols[numColunas];
  int rows_per_proc; 
  float rows_aux;

  int *send_counts, *displs;
  int sum = 0;

  double start, end, time; 

  MPI_Init( &argc, &argv ); 

  start = MPI_Wtime(); 

  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );

  

  qtd_elem = numLinhas*numColunas;

  //rec_size = 100;

  vetor_rec=(int*)malloc(rec_size*sizeof(int));

  //Quantidade de elementos a serem enviados para cada processador
  send_counts = (int*)malloc(sizeof(int)*size); 
  displs = (int*)malloc(sizeof(int)*size);

  int rows_rem = numLinhas;
  int procs_rem = size;

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
    for (i = 0; i < numLinhas; i++) {
        for (j = 0; j < numColunas; j++) {
          matriz[i][j] = qtd_elem/(numColunas+numLinhas+16) + rand()%16;
        }
    }
/*
//imprimir matriz
   for (i = 0; i < numLinhas; i++) {
        for (j = 0; j < numColunas; j++) {
          printf("%d ",matriz[i][j]);
        }
        printf("\n");
    }

    printf("\n");*/

  }

  rec_size = send_counts[rank];

  vetor_rec=(int*)malloc(rec_size*sizeof(int));

  int rows = send_counts[rank]/numColunas;

  MPI_Scatterv(&matriz,send_counts,displs, MPI_INT, vetor_rec,rec_size,MPI_INT,root,MPI_COMM_WORLD);

  int local_min = vetor_rec[0];
  int local_max = vetor_rec[0];
  int sum_rows_local = 0;
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
      sum_rows_local += vetor_rec[i];
      j++;
    }
    if (j==numColunas){
      local_sum_rows[k] = sum_rows_local;
      k++;
      sum_rows_local = 0;
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
  
 //MPI_Barrier(MPI_COMM_WORLD);


  for (i=0;i<rows;i++){
    k = displs[rank]/numColunas + i;
    printf("A soma dos elementos da linha %d é: %d\n", k, local_sum_rows[k]);
    
  }
 // printf("local_min = %d\n", local_min);
  //printf("local_max = %d\n", local_max);
    

 /* for(i=0;i<rec_size;i++){
    printf("Rank:%d recebeu %d \n",rank, vetor_rec[i]);
  }*/


  MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
  MPI_Reduce(&local_sum_cols, &global_sum_cols, numColunas, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
  MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, root, MPI_COMM_WORLD);
  MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, root, MPI_COMM_WORLD);


  //MPI_Barrier(MPI_COMM_WORLD);

 


  if(rank == root){   
    for (i=0;i<numColunas;i++){
      printf("A soma dos elementos da coluna %d é: %d\n", i, global_sum_cols[i]);
    }
    printf("O maior elemento da matriz é: %d\n", global_max);
    printf("O menor elemento da matriz é: %d\n", global_min);
    printf("A soma de todos os elementos da matriz é: %d\n", global_sum);

    
  }


	
  free(vetor_rec);
  free(send_counts);
  free(displs);
  
 
  MPI_Finalize();

  end = MPI_Wtime(); 

  time = end - start;

  printf( "Elapsed time is %f\n", time); 
}

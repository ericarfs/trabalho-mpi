#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matriz.h"

#define TAM 16
 
int main( int argc, char **argv ){

  int numLinhas = atoi(argv[1]);
  int numColunas = atoi(argv[2]);

  int matriz[numLinhas][numColunas];
  int matriz_transp[numColunas][numLinhas];
  int i, j, k;
  int qtd_elem, rank, size,rec_size, rec_size_max, rec_size_min,*vetor_rec, root=0;
  int global_min, global_max, local_sum = 0, global_sum = 0;
  int local_sum_rows[numLinhas],  local_sum_cols[numColunas];
  int global_sum_rows[numLinhas], global_sum_cols[numColunas];
  int rows_per_proc, rpp_aux; 
  float rows_aux;

  int *sendcounts, *displs;
  int sum = 0;

  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );

  
  k = size - 1;

  float con = 0.5;
  //Atribuir quantidade de linhas por processador
  rows_aux = (float)numLinhas/size;
  if (fmod(rows_aux,con) == 0){
    rows_per_proc = floor(rows_aux);
  }
  else{
    rows_per_proc = round(rows_aux);
  }
  rpp_aux = numLinhas - (rows_per_proc * (size - 1));


  qtd_elem = numLinhas*numColunas;


  rec_size = (rows_per_proc > rpp_aux) ? rows_per_proc * numColunas: rpp_aux * numColunas;

  vetor_rec=(int*)malloc(rec_size*sizeof(int));

  sendcounts = (int*)malloc(sizeof(int)*size);
  displs = (int*)malloc(sizeof(int)*size);
  rem = (qtd_elem)%size; // elements remaining after division among processes

  int teste[numLinhas]; 
  int rows_rem = numLinhas;
  int procs_rem = size;
    // calculate send counts and displacements

  for (int i = 0; i < size; i++) {
    rows_aux = (float)rows_rem/procs_rem;
    rows_per_proc = ceil(rows_aux);
    rows_rem -= rows_per_proc;
    procs_rem--;
      
    sendcounts[i] = rows_per_proc * numColunas;

    displs[i] = sum;
    sum += sendcounts[i];

  }



  if(rank == root) {
    for (i = 0; i < numLinhas; i++) {
        for (j = 0; j < numColunas; j++) {
          matriz[i][j] = qtd_elem/(numColunas+numLinhas+16) + rand()%16;
          matriz_transp[j][i] = matriz[i][j];
        }
    }

   for (i = 0; i < numLinhas; i++) {
        for (j = 0; j < numColunas; j++) {
          printf("%d ",matriz[i][j]);
        }
        printf("\n");
    }

    printf("\n");

    
    /**for(i=0;i<numLinhas;i++){
      for(j=0;j<numColunas;j++){
        printf("Processo %d: matriz[%d][%d]=%d \n",rank, i, j, matriz[i][j]);
      }
      printf("\n");
    }**/
  }

  int current;

  //MPI_Comm_rank( MPI_COMM_WORLD, &current );


 // int rows = displs[current];
  //printf("rank = %d, rows = %d\n", rank, rows);

  rec_size = sendcounts[rank];
  //printf("tam = %d\n", tam);

  MPI_Scatterv(&matriz,sendcounts,displs, MPI_INT, vetor_rec,rec_size,MPI_INT,root,MPI_COMM_WORLD);

  int local_min = vetor_rec[0];
  int local_max = vetor_rec[0];
  int local_sum_rws = 0;
  j = 0;

  for (i = 0; i < rec_size; i++){
      local_sum += vetor_rec[i];
      if (vetor_rec[i] <= local_min){
        local_min = vetor_rec[i];
      }
      else if (vetor_rec[i] >= local_max){
        local_max = vetor_rec[i];
      }

      if (j<numColunas){
        local_sum_rws += vetor_rec[i];
        //printf("rank = %d elemento = %d\n", rank, vetor_rec[i]);
      }
      else{
        //printf("Soma da linha = %d\n", local_sum_rws);
        j = 0;
      }
      j++;
  }

  

  printf("local_min = %d\n", local_min);
  printf("local_max = %d\n", local_max);
    

  for(i=0;i<rec_size;i++){
    printf("Rank:%d recebeu %d \n",rank, vetor_rec[i]);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
  MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, root, MPI_COMM_WORLD);
  MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, root, MPI_COMM_WORLD);


  if(rank == root){   
    printf("O maior elemento da matriz é: %d\n", global_max);
    printf("O menor elemento da matriz é: %d\n", global_min);
    printf("A soma de todos os elementos da matriz é: %d\n", global_sum);
      
  }


	
  free(vetor_rec);
  MPI_Finalize();

    
}

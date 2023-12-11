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
  int rank, size,rec_size, i, j,*vetor_rec, root=0;
  int global_min, global_max, local_sum = 0, global_sum = 0;
   
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );

  rec_size=(numLinhas*numColunas)/size;

  printf("%d\n", rec_size);
  vetor_rec=(int*)malloc(rec_size*sizeof(int));

  if(rank == root) {
    for (i = 0; i < numLinhas; i++) {
        for (j = 0; j < numColunas; j++) {
          matriz[i][j] = numColunas*numLinhas/(numColunas+numLinhas+16) + rand()%16;
        }
    }

    for (i = 0; i < numLinhas; i++) {
        for (j = 0; j < numColunas; j++) {
          printf("%d ",matriz[i][j]);
        }
        printf("\n");
    }

    /**for(i=0;i<numLinhas;i++){
      for(j=0;j<numColunas;j++){
        printf("Processo %d: matriz[%d][%d]=%d \n",rank, i, j, matriz[i][j]);
      }
      printf("\n");
    }**/
  }

  MPI_Scatter (&matriz[0][0],rec_size,MPI_INT,vetor_rec,rec_size,MPI_INT,root,MPI_COMM_WORLD);

  int local_min = vetor_rec[0];
  int local_max = vetor_rec[0];

    
  for (int i = 0; i < rec_size; i++){
      local_sum += vetor_rec[i];
      if (vetor_rec[i] <= local_min){
        local_min = vetor_rec[i];
      }
      else if (vetor_rec[i] >= local_max){
        local_max = vetor_rec[i];
      }
  }


  printf("\n");
  printf("local_min = %d\n", local_min);
  printf("local_max = %d\n", local_max);
    

  for(i=0;i<rec_size;i++){
    printf("Rank :%d recebeu %d \n",rank, vetor_rec[i]);
  }

  //MPI_Barrier(MPI_COMM_WORLD);

  MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
  MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, root, MPI_COMM_WORLD);
  MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, root, MPI_COMM_WORLD);

  if(rank == root){   
    
    printf("min = %d\n", global_min);
    printf("max = %d\n", global_max);

    printf("sum = %d\n", global_sum);
      
  }


	
  free(vetor_rec);
  MPI_Finalize();

    
}

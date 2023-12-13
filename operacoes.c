#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "matriz.h"


// Função principal
int main(int argc, char *argv[]) {
    
    int numLinhas = atoi(argv[1]);
    int numColunas = atoi(argv[2]);


    double start, end; 

    start = MPI_Wtime(); 
    // Leitura dos elementos da matriz
    matriz_t *matriz;

    matriz = iniciar(numLinhas,numColunas);
    
    gerarMatriz(matriz->matriz,numLinhas,numColunas);
    realizar_operacoes(matriz, numLinhas,numColunas);

    // Impressão da soma dos elementos de cada linha da matriz
    for (int i = 0; i < numLinhas; i++) {
        printf("A soma dos elementos da linha %d é: %d\n", i, matriz->somaLinhas[i]);
    }

    // Impressão da soma dos elementos de cada coluna da matriz
    for (int i = 0; i < numColunas; i++) {
        printf("A soma dos elementos da coluna %d é: %d\n", i, matriz->somaColunas[i]);
    }


    // Impressão do maior elemento da matriz
    printf("O maior elemento da matriz é: %d\n", matriz->maiorElemento);

    // Impressão do menor elemento da matriz
    printf("O menor elemento da matriz é: %d\n", matriz->menorElemento);

    // Impressão da soma de todos os elementos da matriz
    printf("A soma de todos os elementos da matriz é: %d\n", matriz->somaElementos);

    freeMatriz(matriz,numLinhas,numColunas);


    end = MPI_Wtime(); 

    printf( "Elapsed time is %f\n", end - start); 

    return 0;
}

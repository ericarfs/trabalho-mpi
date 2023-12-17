/*
TRABALHO PRÁTICO 2 - Paralelização com MPI
Disciplina: SSC0903 - Computação de Alto Desempenho
Estudantes: 
----Erica Ribeiro Filgueira dos Santos  - 11836351
----Susy da Costa Dutra                 - 12694007 
----Théo da Mota dos Santos             - 10691331

Para compilar:
    mpicc -o main  operacoes.c matriz.c
Para rodar:
    ./main <num_linhas> <num_colunas>

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "matriz.h"


// Função principal
int main(int argc, char *argv[]) {

    char *n = argv[1];
    char *m = argv[2];
    
    int numLinhas = atoi(n);
    int numColunas = atoi(m);

    char resultFileName[50] = "resultados/TempoSequencial_";

    //Criar nome do arquivo de entrada
    strcat(resultFileName, n);
    strcat(resultFileName, "x");
    strcat(resultFileName, m);
    strcat(resultFileName, ".txt");


    FILE *resultFile = fopen(resultFileName, "a");

    double start, end, time; 

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

    time = end - start;

    fprintf(resultFile, "%f\n", time);

    fclose(resultFile);

    printf("%s \n", resultFileName);


    printf( "Elapsed time is %f\n", time); 

    return 0;
}

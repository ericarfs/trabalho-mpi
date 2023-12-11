#ifndef MATRIZ_H
#define MATRIZ_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int **matriz;
    int maiorElemento;
    int menorElemento;
    int somaElementos;
    int *somaLinhas;
    int *somaColunas;
}matriz_t; 

matriz_t *iniciar(int linhas, int cols);
void gerarMatriz(matriz_t *m, int linhas, int cols);
void print_matriz(matriz_t *m, int linhas, int cols);
void maior_menor_elemento(matriz_t *m, int linhas, int cols);
void soma_total(matriz_t *m, int linhas, int cols);
void soma_linhas(matriz_t *m, int linhas, int cols);
void soma_colunas(matriz_t *m, int linhas, int cols);
void realizar_operacoes(matriz_t *m, int linhas, int cols);
void freeMatriz(matriz_t *m, int linhas, int cols);


#endif
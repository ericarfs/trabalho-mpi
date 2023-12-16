/*
TRABALHO PRÁTICO 2 - Paralelização com MPI
Disciplina: SSC0903 - Computação de Alto Desempenho
Estudantes: 
----Erica Ribeiro Filgueira dos Santos  - 11836351
----Susy da Costa Dutra                 - 12694007 
----Théo da Mota dos Santos             - 10691331
*/

#include "matriz.h"


matriz_t *iniciar(int linhas, int cols){
    matriz_t *m = (matriz_t *)malloc(sizeof(matriz_t));

    m->matriz = (int**)calloc(linhas, sizeof(int*));
    for (int i = 0; i < linhas; i++ ) {
        m->matriz[i] = (int*)calloc(cols, sizeof(int));
    }

    m->somaLinhas = (int*)calloc(linhas, sizeof(int));
    m->somaColunas = (int*)calloc(cols, sizeof(int));

    return m;
}


// Função para ler os elementos da matriz
void gerarMatriz(int **matriz, int linhas, int cols) {
    int i, j;        
    for (i = 0; i < linhas; i++) {
        for (j = 0; j < cols; j++) {
            matriz[i][j] = rand()%16;
        }
    }
}

// Função para imprimir os elementos da matriz
void print_matriz(int **matriz, int linhas, int cols) {
  int i, j;

    for (i = 0; i < linhas; i++) {
        for (j = 0; j < cols; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
}

// Função para calcular o maior elemento da matriz
void maior_menor_elemento(matriz_t *m, int linhas, int cols) {
    int i, j;
    int maior = m->matriz[0][0];
    int menor = m->matriz[0][0];

    
    for (i = 0; i < linhas; i++) {
        for (j = 0; j < cols; j++) {
            if (m->matriz[i][j] > maior) {
                maior = m->matriz[i][j];
            }
            else if (m->matriz[i][j] < menor) {
                menor = m->matriz[i][j];
            }
        }
    }

    m->maiorElemento = maior;
    m->menorElemento = menor;
}


// Função para calcular a soma de todos os elementos da matriz
void soma_total(matriz_t *m, int linhas, int cols) {
    int soma = 0;
    int i, j;

    for (i = 0; i < linhas; i++) {
        for (j = 0; j < cols; j++) {
            soma += m->matriz[i][j];
        }
    }

    m->somaElementos = soma;
}

// Função para calcular a soma dos elementos de cada linha da matriz
void soma_linhas(matriz_t *m, int linhas, int cols) {
    int soma;
    int i, j;

    for (i = 0; i < linhas; i++) {
        soma = 0;
        for (j = 0; j < cols; j++){
            soma += m->matriz[i][j];
        }
        m->somaLinhas[i] = soma;
    }
}

// Função para calcular a soma dos elementos de cada coluna da matriz
void soma_colunas(matriz_t *m, int linhas, int cols) {
    int soma;
    int i, j;

    for (j = 0; j < cols; j++) {
        soma = 0;
        for (i = 0; i < linhas; i++){
            soma += m->matriz[i][j];
        }
        m->somaColunas[j] = soma;
    }
}


void realizar_operacoes(matriz_t *m, int linhas, int cols){
    maior_menor_elemento(m, linhas, cols);
    soma_linhas(m, linhas, cols);
    soma_colunas(m, linhas, cols);
    soma_total(m, linhas, cols);
}


void freeMatriz(matriz_t *m, int linhas, int cols){
    int i;

    for (i = 0; i < linhas; i++){
        free(m->matriz[i]);
    }

    free(m->somaColunas);
    free(m->somaLinhas);
    free(m->matriz);
    free(m);

}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int **createTestGraph(int n) {//creates matrix (4x3) with 12 nodes, 3 columns and 4 rows,connected: up, down, right
    if(n!=12)
    {
        fprintf(stderr,"Unfortunatelly this test is only to 12 vecticies graph :c");
        return NULL;
    }
    int **matrix = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        matrix[i] = calloc(n, sizeof(int));
    }
    int columns=3;
    int rows=4;
    for(int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
        {
            if(i-1>=0)
                matrix[i*columns+j][(i-1)*columns+j]=1;
            if(i+1<rows)
                matrix[i*columns+j][(i+1)*columns+j]=1;
            if(j-1>=0)
                matrix[i*columns+j][i*columns+j-1]=1;
            if(j+1<columns)
                matrix[i*columns+j][i*columns+j+1]=1;
        }
    
    return matrix;
}


int** createIrregularTestGraph(int n) {//creates irregular matrix
    if(n!=12)
    {
        fprintf(stderr,"Unfortunatelly this test is only to 12 vecticies graph :c");
        return NULL;
    }

    int **matrix = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        matrix[i] = calloc(n, sizeof(int));
    }

    // dodajemy krawędzie ręcznie
    int edges[][2] = {
        {0, 1}, {0, 2},
        {1, 2}, {1, 3},
        {2, 4},
        {3, 4}, {3, 5},
        {4, 6},
        {5, 6},
        {6, 7}, {6, 8},
        {7, 8},
        {8, 9},
        {9, 10},
        {10, 11}
    };

    int edgeCount = sizeof(edges) / sizeof(edges[0]);
    for (int i = 0; i < edgeCount; i++) {
        int u = edges[i][0];
        int v = edges[i][1];
        matrix[u][v] = 1;
        matrix[v][u] = 1;
    }

    return matrix;
}

int** createLargeTestGraph(int n, double connectionProbability) {//creates matrix (n,n), that has some probability that random(or semi random because of choosen seed) edges are connected
    int **matrix = malloc(n * sizeof(int*));
    if (!matrix) {
        perror("malloc matrix");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        matrix[i] = calloc(n, sizeof(int));
        if (!matrix[i]) {
            perror("calloc row");
            exit(EXIT_FAILURE);
        }
    }

    srand(0);//srand(0) for repeatability

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (((double) rand() / RAND_MAX) < connectionProbability) {
                matrix[i][j] = 1;
                matrix[j][i] = 1; // undirected connections
            }
        }
    }

    return matrix;
}
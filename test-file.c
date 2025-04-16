#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

int **createTestGraph(int n) {
    if(n!=12)
    {
        fprintf(stderr,"Unfortunatelly this test is only to 12 vecticies graph :c");
        return NULL;
    }
    int **matrix = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        matrix[i] = calloc(n, sizeof(int));
    }
    int columns=3;//hard coded test
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


int** createIrregularTestGraph(int n) {
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

int main()
{
    int n = 12;  // liczba wierzchołków
    int k = 4;   // liczba partycji
    int maxPartSize = n / k;
    

    int **neighbourMatrix = createBasicTestGraph(n);
    //printConnections(neighbourMatrix, n);

    Node **neighbourList = convertMatrixToList(neighbourMatrix, n);

    int *partitionTab = createPartitionTab(n);
    int *vertexDegree = createVertexDegree(neighbourList,n);

    int curStart = 0;
    int curPartSize = 0;
    for (int i = 0; i < k; i++) {
        while (partitionTab[curStart] != -1)
            curStart++;
        curPartSize = 0;//assiumpion is that we finish each partition in dfs 
        dfs(neighbourList, partitionTab, &curPartSize, maxPartSize, vertexDegree, curStart, i);
    }

    int **partition=createPartition(partitionTab,n,k);
    printPartition(partition,n,k);

    
    int *outerConnections=createOuterConnections(neighbourList,partitionTab,partition,k,n);
    printOuterConnections(outerConnections,k);

    
    printPartitionsTab(partitionTab, n, k);


    freeAll(neighbourMatrix,neighbourList,partitionTab,vertexDegree,partition,outerConnections,n,k);

    

    return 0;
}
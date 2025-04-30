#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

int** createComplexTestGraph(int n) {
    int **matrix = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        matrix[i] = calloc(n, sizeof(int));
    }

    // dodajemy krawędzie ręcznie
    int edges[][2] = {
        {0, 1}, {0, 2}, {0, 3},
        {1, 3},
        {2, 3}, {2, 4},
        {3, 5},
        {4, 5}, {4, 6},
        {5, 6}, {5, 7},
        {6, 7}
    };

    int edgeCount = sizeof(edges) / sizeof(edges[0]);
    for (int i = 0; i < edgeCount; i++) {
        int u = edges[i][0];
        int v = edges[i][1];
        matrix[u][v] = 1;
        matrix[v][u] = 1; // bo graf nieskierowany
    }

    return matrix;
}

int** createLargeTestGraph(int n, double connectionProbability)
{
    int **matrix = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        matrix[i] = calloc(n, sizeof(int));
    }

    srand(0); // seed losowy

    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            if (((double) rand() / RAND_MAX) < connectionProbability) {
                matrix[i][j] = 1;
                matrix[j][i] = 1; // graf nieskierowany
            }
        }
    }

    return matrix;
}

int count_cut_edges(Node *neighbourList[], int partitionTab[], int n)
{
    int cutEdges = 0;
    
    for (int u = 0; u < n; u++)
    {
        Node* it = neighbourList[u];
        while (it)
        {
            int v = it->vertex;
            
            // Sprawdzamy czy krawędź przechodzi między różnymi partycjami
            if (partitionTab[u] != -1 && partitionTab[v] != -1 && partitionTab[u] != partitionTab[v])
            {
                cutEdges++;
            }
            
            it = it->next;
        }
    }
    
    // Dzielimy przez 2, bo każda krawędź była liczona dwa razy
    return cutEdges / 2;
}

int main()
{
    int n = 12;  // liczba wierzchołków
    int k = 4;   // liczba partycji
    int firstPartSize = n / k;
    double precision=0.1f;
    int remainingNodes=n;
    int average[2]={0,0};//0-index general average,1-index current average
    int pMin=0;//possible min
    int pMax=0;//possible max
    int* partSize=malloc(sizeof(int) * k);

    double connectionProbability = 0.02;
    int **neighbourMatrix = createBasicTestGraph(n);
    //printConnections(neighbourMatrix, n);

 
    Node **neighbourList = convertMatrixToList(neighbourMatrix, n);

    int *partitionTab = createPartitionTab(n);
    int *vertexDegree = createVertexDegree(neighbourList,n);

    int curStart = 0;
    for (int i = 0; i < k; i++) {
        curStart=findBestPartitionStart(partitionTab,n,neighbourList,vertexDegree);
        int curPartSize = 0;
        average[1]=0;

        int remainingParts = k - i;
        double avg = (double)remainingNodes / remainingParts;
        pMin = (int)(avg * (1.0 - precision));
        pMax = (int)(avg * (1.0 + precision));
        if (pMin < 1) pMin = 1;
        if (pMax > remainingNodes) pMax = remainingNodes;

        dfs(neighbourList, partitionTab, &curPartSize, firstPartSize, vertexDegree, curStart, i, average,pMin,pMax);
        remainingNodes-=curPartSize;
        partSize[i]=curPartSize;
        if(average[0]==0)
        {
            average[0]+=(average[1]/(curPartSize));
        }
        else
        {
            average[0]+=(average[1]/(curPartSize));
            average[0]/=2;
        }
        printf("   Partition %d average is %d\n",i,average[1]/curPartSize);
        printf("After partition %d average is: %d\n\n",i,average[0]);
    }

    //assigning remaining nodes
    assignRemainingNodes(neighbourList,partitionTab,partSize,n,k,precision);

    int **partition=createPartition(partitionTab,n,k,partSize);
    printPartition(partition,k,partSize);

    
    int *outerConnections=createOuterConnections(neighbourList,partitionTab,partition,k,n);
    printOuterConnections(outerConnections,k);

    
    printPartitionsTab(partitionTab, n, k);

    int cuts = count_cut_edges(neighbourList, partitionTab, n);
    printf("\nLiczba przeciętych krawędzi: %d\n", cuts);


    freeAll(neighbourMatrix,neighbourList,partitionTab,vertexDegree,partition,outerConnections,n,k);

    return 0;
}
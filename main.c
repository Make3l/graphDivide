#include <stdio.h>
#include <stdlib.h>
 
#include "graph.h"

int  main()
{
    
    int n = 12;  // liczba wierzchołków
    int k = 4;   // liczba partycji
    int maxPartSize = n / k;
    

    int **neighbourMatrix = createIrregularTestGraph(n);
    printConnections(neighbourMatrix, n);

 
    Node **neigbourList = convertMatrixToList(neighbourMatrix, n);

    int *partitionTab = malloc(n * sizeof(int));
    int *vertexDegree = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        partitionTab[i] = -1;
        vertexDegree[i] = 0;
        Node *it = neigbourList[i];
        while (it) {
            vertexDegree[i]++;
            it = it->next;
        }
    }

    int curStart = 0;
    for (int i = 0; i < k; i++) {
        while (partitionTab[curStart] != -1)
            curStart++;
        int curPartSize = 0;
        dfs(neigbourList, partitionTab, &curPartSize, maxPartSize, vertexDegree, curStart, i);
    }

    int **partition=createPartition(partitionTab,n,k);
    for (int i = 0; i < k; i++)
    {
        printf("\nPartition %d: ",i);
        for(int j=0;j<(n/k);j++)
            printf(" %d, ",partition[i][j]);
    }

    
    int *outerConnections=createouterConnectionsnection(neigbourList,partitionTab,partition,k,n);
    
    for(int i=0;i<k;i++)
    {
        printf("\nPartition %d <-> %d",i,outerConnections[i]);
    }
    printf("\n");
    

    printPartitions(partitionTab, n, k);


    freeAll(neighbourMatrix,neigbourList,partitionTab,vertexDegree,partition,outerConnections,n,k);

    
    return 0;
}
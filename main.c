#include <stdio.h>
#include <stdlib.h>
 
#include "graph.h"


int  main()
{
    
    int n = 12;  // liczba wierzchołków
    int k = 4;   // liczba partycji
    int maxPartSize = n / k;
    

    int **neighbourMatrix = createBasicTestGraph(n);
    printConnections(neighbourMatrix, n);

 
    Node **neighbourList = convertMatrixToList(neighbourMatrix, n);

    int *partitionTab = createPartitionTab(n);
    int *vertexDegree = createVertexDegree(neighbourList,n);

    int curStart = 0;
    for (int i = 0; i < k; i++) {
        while (partitionTab[curStart] != -1)
            curStart++;
        int curPartSize = 0;
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
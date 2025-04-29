#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "graph.h"


int  main()
{
    
    int n = 12;  // liczba wierzchołków
    int k = 4;   // liczba partycji
    int firstPartSize = n / k;
    double precision=0.1f;
    int remainingNodes=n;
    int average[2]={0,0};//0-index general average,1-index current average
    int boundaryValues[2]={n/k,n/k};//min, max partition sizes
    int pMin=0;//possible min
    int pMax=0;//possible max


    int **neighbourMatrix = createBasicTestGraph(n);
    printConnections(neighbourMatrix, n);

 
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
        if(average[0]==0)
        {
            average[0]+=(average[1]/(curPartSize));
        }
        else
        {
            average[0]+=(average[1]/(curPartSize));
            average[0]/=2;
        }
        boundaryValues[0]=min(boundaryValues[0],curPartSize);//current min
        boundaryValues[1]=max(boundaryValues[1],curPartSize);//current max

        printf("   Partition %d average is %d\n",i,average[1]/curPartSize);
        printf("After partition %d average is: %d\n\n",i,average[0]);
    }

    int **partition=createPartition(partitionTab,n,k);
    printPartition(partition,n,k);

    
    int *outerConnections=createOuterConnections(neighbourList,partitionTab,partition,k,n);
    printOuterConnections(outerConnections,k);

    
    printPartitionsTab(partitionTab, n, k);


    freeAll(neighbourMatrix,neighbourList,partitionTab,vertexDegree,partition,outerConnections,n,k);

    
    return 0;
}
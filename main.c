#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "graph.h"
#include "input.h"

int  main(int argc, char **argv)
{
    int n=0;
    Node **neighbourList=NULL;
    int** neighbourMatrix=NULL;
    if(argc<2)
    {
        printf("Algorithm is currently running on basic graph:\n 12 nodes, 3 columns and 4 rows,\nconnected: up, down, right, left\n");
        n=12;
        int** neighbourMatrix=createBasicTestGraph(n);
        neighbourList=convertMatrixToList(neighbourMatrix,n);
    }
    else
    {
        if(argv[1][0]=='-' && argv[1][1]=='c')
        {
            printf("Algorithm is currently running on your %s file\n",argv[2]);
            neighbourList = runCsrrg(argv[2],&n);
        }
        else
        {
            fprintf(stderr,"You gave a wrong flag, to run .csrrg file you need to type -c your-file.csrrg ");
            exit(1);
        }
    }

    int k = 5;   // liczba partycji
    int firstPartSize = n / k; 
    double precision=0.5f;
    int remainingNodes=n;
    long long average[2]={0,0};//0-index general average,1-index current average
    int pMin=0;//possible min
    int pMax=0;//possible max
    int* partSize=malloc(sizeof(int) * k);
        

    int *partitionTab = createPartitionTab(n);
    int *vertexDegree = createVertexDegree(neighbourList,n);

    int curStart = 0;
    for (int i = 0; i < k; i++) {
        curStart=findBestPartitionStart(partitionTab,n,neighbourList);
        if (curStart == -1) {
            printf("Brak dostępnych wierzchołków do partycji %d\n", i);
            break;
        }
        int curPartSize = 0;
        average[1]=0;

        int remainingParts = k - i;
        double avg = (double)remainingNodes / remainingParts;
        pMin = (int)(avg * (1.0 - precision));
        pMax = (int)(avg * (1.0 + precision));
        if (pMin < 1) pMin = 1;
        if (pMax > remainingNodes) pMax = remainingNodes;

        dfsIterative(neighbourList,partitionTab,&curPartSize,firstPartSize,vertexDegree,curStart,i,average,pMin,pMax,n);
        remainingNodes-=curPartSize;
        partSize[i]=curPartSize;
        if(i==0)
        {
            average[0]+=(average[1]/(curPartSize));
        }
        else
        {
            average[0]+=(average[1]/(curPartSize));
            average[0]/=2;
        }

        printf("DFS zakończony: partycja %d, rozmiar = %d, suma = %lld\n", i, curPartSize, average[1]);
        //printf("Average[1]=%lld\n",average[1]);
        if (curPartSize > 0)
            printf("Średnia: %lld\n", average[1] / curPartSize);
    }

    //assigning remaining nodes
    assignRemainingNodes(neighbourList,partitionTab,partSize,n,k,precision);

    printPartitionsSizes(partSize,k);

    int **partition=createPartition(partitionTab,n,k,partSize);
    printPartition(partition,k,partSize);

    
    int *outerConnections=createOuterConnections(neighbourList,partitionTab,partition,partSize,k,n);
    printOuterConnections(outerConnections,k);

    //printPartitionsTab(partitionTab, n, k);

    freeAll(neighbourMatrix,neighbourList,partitionTab,vertexDegree,partition,outerConnections,n,k);

    return 0;
}
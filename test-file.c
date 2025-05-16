#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "graph.h"
#include "input.h"

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
int  main(int argc, char **argv)
{
    int n=0;//nummber of verticies
    Node **neighbourList=NULL;
    int** neighbourMatrix=NULL;
    if(argc<2)//options of tests
    {
        printf("Welcome to test file, choose your test:\n1 - Basic test 12 nodes, 3 columns and 4 rows,connected: up, down, right\n2 - Creates irregular graph with 12 nodes\n3 - Hybrid custom-random graph you need to choose number of nodes and connect probability\n");
        int option=-1;
        printf("Option: ");
        scanf("%d",&option);
        printf("\n");
        switch(option)
        {
            case 1:
                printf("You choosed option 1: basic test 12 nodes, 3 columns and 4 rows,connected: up, down, right\n");
                n=12;
                neighbourMatrix=createBasicTestGraph(n);
                if (!neighbourMatrix) {
                    perror("ERROR: neighbourMatrix==NULL");
                    exit(EXIT_FAILURE);
                }
                break;
            case 2:
                printf("You choosed option 1: irregular graph with 12 nodes\n");
                n=12;
                neighbourMatrix=createIrregularTestGraph(n);
                if (!neighbourMatrix) {
                    perror("ERROR: neighbourMatrix==NULL");
                    exit(EXIT_FAILURE);
                }
                break;
            case 3:
                printf("You choosed option 1: hybrid custom-random graph you need to choose number of nodes and connect probability\n");
                printf("Please enter number of nodes and connection probability, (example 12 0.01)");
                double connectionProbability=0;
                scanf("%d %lf",&n,&connectionProbability);
                if(connectionProbability>=1 || connectionProbability<=0)
                {
                    fprintf(stderr,"ERROR: Connction probability is probability, it needs to be with in (0,1) set: ");
                    exit(1);
                }
                if(n<=0 || n>10000)
                {
                    fprintf(stderr,"ERROR: Number of nodes is a natural number it has to be in <1,10000> set");
                    exit(1);
                }
                neighbourMatrix=createLargeTestGraph(n,connectionProbability);
                if (!neighbourMatrix) {
                    perror("ERROR: neighbourMatrix==NULL");
                    exit(EXIT_FAILURE);
                }
                break;

            default:
                fprintf(stderr,"ERROR: You choosed a wrong option");
                exit(1);
        }
        printf("N = %d\n",n);
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

    int k = 5;   // number of partitions
    int firstPartSize = n / k; 
    double precision=0.5f;
    int remainingNodes=n;
    long long average[2]={0,0};//0-index general average,1-index current average
    int pMin=0;//possible min
    int pMax=0;//possible max
    int* partSize=malloc(sizeof(int) * k);//table of partitions sizes
        

    int *partitionTab = createPartitionTab(n);//table in which vertex numbers are given a partition number
    int *vertexDegree = createVertexDegree(neighbourList,n);//how many connectins vertex has

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
        pMin = (int)(avg * (1.0 - precision));//calculats possible min, for next partition
        pMax = (int)(avg * (1.0 + precision));//calculats possible max, for next partition
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

        printf("DFS ended: partition %d, size = %d, sum = %lld\n", i, curPartSize, average[1]);
        if (curPartSize > 0)
            printf("Average: %lld\n", average[1] / curPartSize);
    }

    //assigning remaining nodes
    assignRemainingNodes(neighbourList,partitionTab,partSize,n,k,precision);

    printPartitionsSizes(partSize,k);

    int **partition=createPartition(partitionTab,n,k,partSize);
    printPartition(partition,k,partSize);

    
    int *outerConnections=NULL;//createOuterConnections(neighbourList,partitionTab,partition,partSize,k,n);
    //printOuterConnections(outerConnections,k);

    //printPartitionsTab(partitionTab, n, k);

    freeAll(neighbourMatrix,neighbourList,partitionTab,vertexDegree,partition,outerConnections,n,k);

    return 0;
}
#include <stdio.h>
#include <unistd.h>  
#include <stdlib.h>
#include "graph.h"
#include "tests.h"
#include "input.h"

void runProgram(Node **neighbourList, int n,int k, double precision, FILE *file)
{
    if (k > n)
    {
        fprintf(file, "ERROR: Number of partitions should be less or equal to number of verticies");
        exit(1);
    }

    if (precision >= 1 || precision <= 0)
    {
        fprintf(file, "ERROR: Precision should be within(0,1) range");
        exit(1);
    }

    int firstPartSize = n / k;

    int remainingNodes = n;
    long long average[2] = {0, 0};           // 0-index general average,1-index current average
    int pMin = 0;                            // possible min
    int pMax = 0;                            // possible max
    int *partSize = malloc(sizeof(int) * k); // table of partitions sizes

    int *partitionTab = createPartitionTab(n);                // table in which vertex numbers are given a partition number
    int *vertexDegree = createVertexDegree(neighbourList, n); // how many connectins vertex has

    int curStart = 0;
    for (int i = 0; i < k; i++)
    {
        curStart = findBestPartitionStart(partitionTab, n, neighbourList);
        if (curStart == -1)
        {
            printf("Brak dostępnych wierzchołków do partycji %d\n", i);
            break;
        }
        int curPartSize = 0;
        average[1] = 0;

        int remainingParts = k - i;
        double avg = (double)remainingNodes / remainingParts;
        pMin = (int)(avg * (1.0 - precision)); // calculats possible min, for next partition
        pMax = (int)(avg * (1.0 + precision)); // calculats possible max, for next partition
        if (pMin < 1)
            pMin = 1;
        if (pMax > remainingNodes)
            pMax = remainingNodes;

        dfsIterative(neighbourList, partitionTab, &curPartSize, firstPartSize, vertexDegree, curStart, i, average, pMin, pMax, n);
        remainingNodes -= curPartSize;
        partSize[i] = curPartSize;
        if (i == 0)
        {
            average[0] += (average[1] / (curPartSize));
        }
        else
        {
            average[0] += (average[1] / (curPartSize));
            average[0] /= 2;
        }
    }

    // assigning remaining nodes
    assignRemainingNodes(neighbourList, partitionTab, partSize, n, k, precision);

    printPartitionsSizes(partSize, k, file);

    printPartitionsTab(partitionTab,n,k,file);

    int **partition = NULL;//createPartition(partitionTab, n, k, partSize);
    //printPartition(partition, k, partSize);

    int *outerConnections = NULL;//createOuterConnections(neighbourList, partitionTab, partition, partSize, k, n);
    //printOuterConnections(outerConnections, k);

    freeAll(neighbourList, partitionTab, vertexDegree, partition, outerConnections, n, k);
}

void fromFile(char *inputFilePath, int k, double precision,char *outputFilePath)
{
    FILE *file=fopen(outputFilePath,"w");//output file to communicate with ui
    int n = 0; // nummber of verticies
    Node **neighbourList = NULL;

    neighbourList = runCsrrg(inputFilePath, &n);

    runProgram(neighbourList,n,k,precision,file);
    fclose(file);
}


void fromTests(int option, int n, double connectionProbability, int k, double precision, char *outputFilePath)
{
    FILE *file=fopen(outputFilePath,"w");//output file to communicate with ui
    Node **neighbourList = NULL;


    int **neighbourMatrix;
    switch(option)
    {
        case 0:
            n=12;
            neighbourMatrix=createTestGraph(n);
            break;
        case 1:
            n=12;
            neighbourMatrix=createIrregularTestGraph(n);
            break;
        case 2:
            neighbourMatrix=createLargeTestGraph(n,connectionProbability);
            break;
        default:
            fprintf(file,"ERROR: WRONG TEST OPTION");
            exit(1);
    }
    neighbourList=convertMatrixToList(neighbourMatrix,n);
    freeNieghbourMatrix(neighbourMatrix,n);

    runProgram(neighbourList,n,k,precision,file);
    fclose(file);
}



void fromUserInput(char *inputFile, double connectionProbability, int k, double precision, char *outputFilePath)
{
    FILE *file=fopen(outputFilePath,"w");//output file to communicate with ui
    Node **neighbourList = NULL;

    int n;
    int **neighbourMatrix=readMaxtrixFromFile(inputFile,&n);
    if(n<=0)
    {
        fprintf(file,"ERROR: Number of verticies is <= 0");
        exit(1);
    }

    neighbourList=convertMatrixToList(neighbourMatrix,n);
    freeNieghbourMatrix(neighbourMatrix,n);

    runProgram(neighbourList,n,k,precision,file);
    fclose(file);
}

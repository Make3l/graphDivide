#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

// Pomocnicza funkcja do dzielenia linii po separatorze ';'
int parseLineToInts(const char* line, int* buffer) {
    int count = 0;
    const char* token = strtok((char*)line, ";");
    while (token != NULL) {
        buffer[count++] = atoi(token);
        token = strtok(NULL, ";");
    }
    return count;
}

// Wczytuje graf z pliku .csrrg i zwraca listę sąsiedztwa oraz liczbę wierzchołków
Node** parseCSRRG(const char* filename, int* out_n) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Nie mozna otworzyc pliku");
        exit(EXIT_FAILURE);
    }

    char line[100000];

    // 1. Wczytaj pierwszą linię (maxPerRow)
    fgets(line, sizeof(line), file);
    int maxPerRow = atoi(line);

    // 2. Wczytaj linię z indeksami (sąsiedzi)
    fgets(line, sizeof(line), file);
    int* indices = malloc(1000000 * sizeof(int));
    int idxCount = parseLineToInts(line, indices);

    // 3. Wczytaj linię z rowPtr
    fgets(line, sizeof(line), file);
    int* rowPtr = malloc(100000 * sizeof(int));
    int rowCount = parseLineToInts(line, rowPtr);
    printf("Liczba wczytanych wierzcholkow: %d\n", rowCount);

    // Tworzenie listy sąsiedztwa
    Node** neighbourList = malloc(rowCount * sizeof(Node*));
    for (int i = 0; i < rowCount; i++) {
        neighbourList[i] = NULL;
    }

    for (int i = 0; i < rowCount; i++) {
        int start = rowPtr[i];
        int end = (i + 1 < rowCount) ? rowPtr[i + 1] : idxCount;
        for (int j = start; j < end; j++) {
            addConnection(neighbourList, i, indices[j]);
        }
    }

    *out_n = rowCount;
    free(indices);
    free(rowPtr);
    fclose(file);
    return neighbourList;
}


int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Uzycie: %s plik.csrrg\n", argv[0]);
        return 1;
    }

    int n;
    Node** neighbourList = parseCSRRG(argv[1], &n);
/*
    printf("Wczytano graf z %d wierzcholkami:\n", n);
    for (int i = 0; i < n; i++) {
        printf("%d:", i);
        Node* it = neighbourList[i];
        while (it) {
            printf(" %d", it->vertex);
            it = it->next;
        }
        printf("\n");
    }
*/
    int k = 7;   // liczba partycji
    int firstPartSize = n / k;
    double precision=0.6f;
    int remainingNodes=n;
    int average[2]={0,0};//0-index general average,1-index current average
    int pMin=0;//possible min
    int pMax=0;//possible max
    int* partSize=malloc(sizeof(int) * k);

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

    //int cuts = count_cut_edges(neighbourList, partitionTab, n);
    //printf("\nLiczba przeciętych krawędzi: %d\n", cuts);

    printf("\nSIZE:");
    for(int i=0;i<k;i++)
    {
        printf("Partition %d size = %d\n",i,partSize[i]);
    }


    freeAllWithoutMatrix(neighbourList,partitionTab,vertexDegree,partition,outerConnections,n,k);

    return 0;

}

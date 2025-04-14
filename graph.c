#include "graph.h"

#include <stdio.h>
#include <stdlib.h>

//helpers

void countOuterPartitionConnections(int outerConnectionsnections[],Node *neighborList[], int startNode,int partitionTab[])
{
    int curPartition=partitionTab[startNode];
    Node *node_it=neighborList[startNode];
    while(node_it)
    {
        if(partitionTab[node_it->vertex]!=curPartition)
            outerConnectionsnections[curPartition]++;
        node_it=node_it->next;
    }
}

int evaluateNeighbours(Node *neighborList[],MarkedNeighbours neighbour,int vertexDegree[],int curPartId, int partitionsTab[])
{
    int w1=3;
    int w2=5;
    int w3=1;
    int innerConnections=0;
    Node *neighbour_id=neighborList[neighbour.id];//neighbours of evalueted neighbour
    while(neighbour_id)
    {
        if(neighbour_id)
        {
            if(partitionsTab[neighbour_id->vertex]==curPartId)
                innerConnections++;
            neighbour_id=neighbour_id->next;
        }
    }
    int outerConnectionsnections=vertexDegree[neighbour.id]-innerConnections;
    return w1*innerConnections-w2*outerConnectionsnections-w3*vertexDegree[neighbour.id];
}

int qsortComparator(const void *a, const void *b) {
    const MarkedNeighbours *ma=(const MarkedNeighbours*)a;
    const MarkedNeighbours *mb=(const MarkedNeighbours*)b;
    return mb->score - ma->score;
}

//logic

void dfs(Node *neighborList[], int partitionsTab[],int* curPartSize,int maxPartSize,int vertexDegree[],int current, int curPartId)//vertexDegree->connction counter,
{
    if(*curPartSize>=maxPartSize)
        return;
    partitionsTab[current]=curPartId;
    (*curPartSize)++;
    Node *node_it=neighborList[current];
    int elemCounter=0;
    MarkedNeighbours *neighbours = malloc(vertexDegree[current]*sizeof(MarkedNeighbours));
    while(node_it)
    {
        if(partitionsTab[node_it->vertex]==-1)//doesnt belong to specyfic partition
        {
            neighbours[elemCounter].id=node_it->vertex;
            neighbours[elemCounter].score=evaluateNeighbours(neighborList,neighbours[elemCounter],vertexDegree,curPartId,partitionsTab);
            elemCounter++;
        }
        node_it=node_it->next;
    }
    qsort( neighbours, elemCounter, sizeof(MarkedNeighbours),qsortComparator);
    for(int i=0;i<elemCounter && maxPartSize>*curPartSize;i++)
    {
        if(partitionsTab[neighbours[i].id]==-1)
            dfs(neighborList,partitionsTab,curPartSize,maxPartSize,vertexDegree,neighbours[i].id,curPartId);
    }
    free(neighbours);
}


//convert

Node **convertMatrixToList(int *neighboursMatrix[],int n)//convert neighbourMatrix to neighbourList
{
    Node **mapNeighbours=malloc(n*sizeof(Node*));
    Node *node_index;
    Node *dummy_node;
    for(int i=0;i<n;i++)
    {
        dummy_node=malloc(sizeof(Node));
        node_index=dummy_node;
        mapNeighbours[i]=NULL;
        for(int j=0;j<n;j++)
        {
            if(neighboursMatrix[i][j]==1)
            {
                node_index->next=malloc(sizeof(Node));
                node_index=node_index->next;
                node_index->next=NULL;
                node_index->vertex=j;
            }
        }
        mapNeighbours[i]=dummy_node->next;
        free(dummy_node);
    }
    return mapNeighbours;
}



//create

int *createouterConnectionsnection(Node *neighborList[],int partitionTab[],int *partitions[],int k,int n)
{
    int *outerConnectionsnections=calloc(k,sizeof(int));
    int *visited;
    int partitionSize=n/k;

    for (int i = 0; i < k; i++)
    {
        visited=calloc(n,sizeof(int));
        for(int j=0;j<partitionSize;j++)
        {
            countOuterPartitionConnections(outerConnectionsnections,neighborList,partitions[i][j],partitionTab);
        }
        free(visited);
    }
    return outerConnectionsnections;
}


int **createPartition(int partitionsTab[],int n,int k)
{
    int **partition=malloc(k*sizeof(int*));
    int partitionSize=n/k;
    for(int i=0;i<k;i++)
        partition[i]=malloc(partitionSize*sizeof(int));

    int elemCounter=0;
    for (int i = 0; i < k; i++) 
    {
        elemCounter=0;
        for (int j = 0; j < n && elemCounter<partitionSize; j++) 
        {
            if (partitionsTab[j] == i)
            {
                partition[i][elemCounter]=j;
                elemCounter++;
            }
        }
    }

    return partition;
}


//print

void printPartitions(int *partitions, int n, int k) {
    printf("== Podział na %d części ==\n", k);
    for (int i = 0; i < k; i++) {
        printf("Partycja %d: ", i);
        for (int j = 0; j < n; j++) {
            if (partitions[j] == i)
                printf("%d ", j);
        }
        printf("\n");
    }
}

void printConnections(int **matrix, int n) {
    printf("== Wszystkie połączenia z macierzy sąsiedztwa ==\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (matrix[i][j]) {
                printf("%d -> %d\n", i, j);
            }
        }
    }
}



//free

void freeAll(int **neighbourMatrix,Node *neigbourList[], int partitionTab[],int vertexDegree[],int *partition[],int outerConnections[],int n,int k)
{
    for (int i = 0; i < n; i++)//cleanning neighbourMatrix
        free(neighbourMatrix[i]);
    free(neighbourMatrix);


    Node *cur;
    Node *next;
    for(int i=0;i<n;i++)//cleaning neighbourList
    {
        cur=neigbourList[i];
        while(cur)
        {
            next=cur->next;
            free(cur);
            cur=next;
        }
    }
    free(neigbourList);


    free(partitionTab);//cleanig partition tab(nodes[i]=partition,)

    free(vertexDegree);//cleanig vertex degree

    for (int i = 0; i < k; i++)//cleaning partition(partition:nodes,...)
        free(partition[i]);
    free(partition);

    free(outerConnections);//cleaning outer connections

}
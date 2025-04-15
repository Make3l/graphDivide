#include "graph.h"

#include <stdio.h>
#include <stdlib.h>

//helpers

void countOuterPartitionConnections(int outerConnectionsnections[],Node *neighbourList[], int startNode,int partitionTab[])
{
    int curPartition=partitionTab[startNode];
    Node *node_it=neighbourList[startNode];
    while(node_it)
    {
        if(partitionTab[node_it->vertex]!=curPartition)
            outerConnectionsnections[curPartition]++;
        node_it=node_it->next;
    }
}

int evaluateNeighbours(Node *neighbourList[],MarkedNeighbours neighbour,int vertexDegree[],int curPartId, int partitionsTab[])
{
    int w1=3;
    int w2=5;
    int w3=1;
    int innerConnections=0;
    Node *neighbour_id=neighbourList[neighbour.id];//neighbours of evalueted neighbour
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

void dfs(Node *neighbourList[], int partitionsTab[],int* curPartSize,int maxPartSize,int vertexDegree[],int current, int curPartId)//vertexDegree->connction counter,
{
    if(*curPartSize>=maxPartSize)
        return;
    partitionsTab[current]=curPartId;
    (*curPartSize)++;
    Node *node_it=neighbourList[current];
    int elemCounter=0;
    MarkedNeighbours *neighbours = malloc(vertexDegree[current]*sizeof(MarkedNeighbours));
    while(node_it)
    {
        if(partitionsTab[node_it->vertex]==-1)//doesnt belong to specyfic partition
        {
            neighbours[elemCounter].id=node_it->vertex;
            neighbours[elemCounter].score=evaluateNeighbours(neighbourList,neighbours[elemCounter],vertexDegree,curPartId,partitionsTab);
            elemCounter++;
        }
        node_it=node_it->next;
    }
    qsort( neighbours, elemCounter, sizeof(MarkedNeighbours),qsortComparator);
    for(int i=0;i<elemCounter && maxPartSize>*curPartSize;i++)
    {
        if(partitionsTab[neighbours[i].id]==-1)
            dfs(neighbourList,partitionsTab,curPartSize,maxPartSize,vertexDegree,neighbours[i].id,curPartId);
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

int *createouterConnectionsnection(Node *neighbourList[],int partitionTab[],int *partitions[],int k,int n)
{
    int *outerConnectionsnections=calloc(k,sizeof(int));
    int *visited;
    int partitionSize=n/k;

    for (int i = 0; i < k; i++)
    {
        visited=calloc(n,sizeof(int));
        for(int j=0;j<partitionSize;j++)
        {
            countOuterPartitionConnections(outerConnectionsnections,neighbourList,partitions[i][j],partitionTab);
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

int *createVertexDegree(Node **neighbourList,int n)
{
    int *vertexDegree = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        vertexDegree[i] = 0;
        Node *it = neighbourList[i];
        while (it) {
            vertexDegree[i]++;
            it = it->next;
        }
    }
    return vertexDegree;
}


int *createPartitionTab(int n)
{
    int *partitionTab = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        partitionTab[i] = -1;
    return partitionTab;
}

//basic test n=12 graph with 3 columns and 4 rows, conencted to up,down,right ,left
int **createBasicTestGraph(int n) 
{
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


//print

void printPartitionsTab(int *partitions, int n, int k) {
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

void printPartition(int **partition,int n,int k)
{
    for (int i = 0; i < k; i++)
    {
        printf("\nPartition %d: ",i);
        for(int j=0;j<(n/k);j++)
            printf(" %d, ",partition[i][j]);
    }
}

void printOuterConnections(int *outerConnections,int k)
{
    for(int i=0;i<k;i++)
    {
        printf("\nPartition %d <-> %d",i,outerConnections[i]);
    }
    printf("\n");
}



//free

void freeAll(int **neighbourMatrix,Node *neighbourList[], int partitionTab[],int vertexDegree[],int *partition[],int outerConnections[],int n,int k)
{
    for (int i = 0; i < n; i++)//cleanning neighbourMatrix
        free(neighbourMatrix[i]);
    free(neighbourMatrix);


    Node *cur;
    Node *next;
    for(int i=0;i<n;i++)//cleaning neighbourList
    {
        cur=neighbourList[i];
        while(cur)
        {
            next=cur->next;
            free(cur);
            cur=next;
        }
    }
    free(neighbourList);


    free(partitionTab);//cleanig partition tab(nodes[i]=partition,)

    free(vertexDegree);//cleanig vertex degree

    for (int i = 0; i < k; i++)//cleaning partition(partition:nodes,...)
        free(partition[i]);
    free(partition);

    free(outerConnections);//cleaning outer connections

}
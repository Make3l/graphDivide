#ifndef GRAPH_H
#define GRAPH_H

typedef struct Node{
    int vertex;
    struct Node *next;
}Node;

typedef struct {
    int id;
    int score;
} MarkedNeighbours;


//helpers
void countOuterPartitionConnections(int outerConnectionsnections[],Node *neighborList[], int startNode,int partitionTab[]);
int evaluateNeighbours(Node *neighborList[],MarkedNeighbours neighbour,int vertexDegree[],int curPartId, int partitionsTab[]);
int qsortComparator(const void *a, const void *b);

//logic
void dfs(Node *neighborList[], int partitionsTab[],int* curPartSize,int maxPartSize,int vertexDegree[],int current, int curPartId);

//convert
Node **convertMatrixToList(int *neighboursMatrix[],int n);

//create
int *createouterConnectionsnection(Node *neighborList[],int partitionTab[],int *partitions[],int k,int n);
int **createPartition(int partitionsTab[],int n,int k);
int *createVertexDegree(Node **neighbourList,int n);
int *createPartitionTab(int n);

//basic test n=12 graph with 3 columns and 4 rows, conencted to up,down,right ,left
int **createBasicTestGraph(int n);

//print
void printPartitionsTab(int *partitions, int n, int k);
void printConnections(int **matrix, int n);
void printPartition(int **partition,int n,int k);
void printOuterConnections(int *outerConnections,int k);

//free
void freeAll(int **neighbourMatrix,Node *neighbourList[], int partitionTab[],int vertexDegree[],int *partition[],int outerConnections[],int n,int k);

#endif
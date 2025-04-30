#ifndef GRAPH_H
#define GRAPH_H

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

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
int evaluateStart( Node *neighbourList[],int evNode,int partitionTab[],int vertexDegree[]);
int findBestPartitionStart(int partitionTab[],int n, Node *neighbourList[],int vertexDegree[]);
int findBestNearPartition(Node *neighbourList[],int partitionTab[],int partSize[],int unassignedNode,int k,double p,int n);
void assignRemainingNodes(Node *neighbourList[],int partitionTab[],int partSize[],int n,int k,double p);

//logic
void dfs(Node *neighborList[], int partitionsTab[],int* curPartSize,int maxPartSize,int vertexDegree[],int current, int curPartId,int average[],int pMin,int pMax);

//convert
Node **convertMatrixToList(int *neighboursMatrix[],int n);

//create
int *createOuterConnections(Node *neighborList[],int partitionTab[],int *partitions[],int k,int n);
int **createPartition(int partitionsTab[],int n,int k,int partitionSize[]);
int *createVertexDegree(Node **neighbourList,int n);
int *createPartitionTab(int n);

//basic test n=12 graph with 3 columns and 4 rows, conencted to up,down,right ,left
int **createBasicTestGraph(int n);

//print
void printPartitionsTab(int *partitions, int n, int k);
void printConnections(int **matrix, int n);
void printPartition(int **partition,int k,int partitionSize[]);
void printOuterConnections(int *outerConnections,int k);

//free
void freeAll(int **neighbourMatrix,Node *neighbourList[], int partitionTab[],int vertexDegree[],int *partition[],int outerConnections[],int n,int k);

#endif
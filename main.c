#include <stdio.h>
#include <stdlib.h>

typedef struct Node{
    int vertex;
    struct Node *next;
}Node;

typedef struct {
    int id;
    int score;
} MarkedNeighbours;

int evaluateNeighbours(Node *nodes[],MarkedNeighbours neighbour,int degree[],int curPartId, int partitionsTab[])
{
    int w1=3;
    int w2=5;
    int w3=1;
    int innerConnections=0;
    Node *neighbour_id=nodes[neighbour.id];//neighbours of evalueted neighbour
    while(neighbour_id)
    {
        if(neighbour_id)
        {
            if(partitionsTab[neighbour_id->vertex]==curPartId)
                innerConnections++;
            neighbour_id=neighbour_id->next;
        }
    }
    int outerConnections=degree[neighbour.id]-innerConnections;
    return w1*innerConnections-w2*outerConnections-w3*degree[neighbour.id];
}

int qsortComparator(const void *a, const void *b) {
    const MarkedNeighbours *ma=(const MarkedNeighbours*)a;
    const MarkedNeighbours *mb=(const MarkedNeighbours*)b;
    return mb->score - ma->score;
}


void dfs(Node *nodes[], int partitionsTab[],int* curPartSize,int maxPartSize,int degree[],int current, int curPartId)//degree->connction counter,
{
    if(*curPartSize>=maxPartSize)
        return;
    partitionsTab[current]=curPartId;
    (*curPartSize)++;
    Node *node_it=nodes[current];
    int elemCounter=0;
    MarkedNeighbours *neighbours = malloc(degree[current]*sizeof(MarkedNeighbours));
    while(node_it)
    {
        if(partitionsTab[node_it->vertex]==-1)//doesnt belong to specyfic partition
        {
            neighbours[elemCounter].id=node_it->vertex;
            neighbours[elemCounter].score=evaluateNeighbours(nodes,neighbours[elemCounter],degree,curPartId,partitionsTab);
            elemCounter++;
        }
        node_it=node_it->next;
    }
    qsort( neighbours, elemCounter, sizeof(MarkedNeighbours),qsortComparator);
    for(int i=0;i<elemCounter && maxPartSize>*curPartSize;i++)
    {
        if(partitionsTab[neighbours[i].id]==-1)
            dfs(nodes,partitionsTab,curPartSize,maxPartSize,degree,neighbours[i].id,curPartId);
    }
    free(neighbours);
}

Node **mapNeighboursFunction(int *neighboursMatrix[],int n)//convert neighbourMatrix to neighbourList
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

void freeNeighbours(int n, Node **mapNeighbours)
{
    for (int i = 0; i < n; i++) {
        Node* current = mapNeighbours[i];
        while (current) {
            Node* next = current->next;
            free(current);
            current = next;
        }
    }
    free(mapNeighbours);
}



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

int **createTestGraph(int n) {
    int **matrix = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        matrix[i] = calloc(n, sizeof(int));
    }

    // prosty graf: 0-1-2-3-4-5-6-7-8-9
    for (int i = 0; i < n - 1; i++) {
        matrix[i][i + 1] = 1;
        matrix[i + 1][i] = 1;
    }

    return matrix;
}

void freeMatrix(int **matrix, int n) {
    for (int i = 0; i < n; i++)
        free(matrix[i]);
    free(matrix);
}

int  main()
{
    int n = 10;  // liczba wierzchołków
    int k = 3;   // liczba partycji
    int maxPartSize = n / k;

    int **neighbourMatrix = createTestGraph(n);
    Node **neighbours = mapNeighboursFunction(neighbourMatrix, n);

    int *partitionTab = malloc(n * sizeof(int));
    int *degree = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        partitionTab[i] = -1;
        degree[i] = 0;
        Node *it = neighbours[i];
        while (it) {
            degree[i]++;
            it = it->next;
        }
    }

    int curStart = 0;
    for (int i = 0; i < k; i++) {
        while (partitionTab[curStart] != -1)
            curStart++;
        int curPartSize = 0;
        dfs(neighbours, partitionTab, &curPartSize, maxPartSize, degree, curStart, i);
    }

    printPartitions(partitionTab, n, k);

    freeMatrix(neighbourMatrix, n);
    freeNeighbours(n, neighbours);
    free(partitionTab);
    free(degree);

    
    return 0;
}
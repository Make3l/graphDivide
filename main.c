#include <stdio.h>

typedef struct {
    int vertex;
    Node *next;
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
    if(curPartSize>=maxPartSize)
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
    for(int i=0;i<elemCounter && maxPartSize>curPartSize;i++)
    {
        if(partitionsTab[neighbours[i].id]==-1)
            dfs(nodes,partitionsTab,&curPartSize,maxPartSize,degree,neighbours[i].id,curPartId);
    }
    free(neighbours);
}

Node *mapNeighboursFunction(int *neighboursMatrix[],int n)//convert neighbourMatrix to neighbourList
{
    Node **mapNeighbours=malloc(n*sizeof(Node*));

    for(int i=0;i<n;i++)
    {
        Node node;
        Node *node_index=&node;
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
        mapNeighbours[i]=node.next;
    }
    return mapNeighbours;
}

int  main()
{
    int n=100;//example of nodes number
    int k=4;//number of partitions
    //later add precision

    int *partitionTab=malloc(n*sizeof(int));
    for(int i=0;i<n;i++)
        partitionTab[i]=-1;

    int curPartSize=0;
    int maxPartSize=n/k;



    
    return 0;
}
#include <stdio.h>

typedef struct {
    int vertex;
    int *next;
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

void dfs(Node *nodes[], int partitionsTab[],int curPartSize,int maxPartSize,int degree[],int start, int curPartId)//degree->connction counter,
{
    if(curPartSize>=maxPartSize)
        return;

    Node *node_it=nodes[start];
    int elemCounter=0;
    MarkedNeighbours *neighbours = malloc(degree[start]*sizeof(MarkedNeighbours));
    while(node_it)
    {
        if(partitionsTab[node_it->vertex]!=-1)//doesnt belong to specyfic partition
        {
            neighbours[elemCounter].id=node_it->vertex;
            neighbours[elemCounter].score=evaluateNeighbours(nodes,neighbours[elemCounter],degree,curPartId,partitionsTab);
            elemCounter++;
        }
        node_it=node_it->next;
    }

    //ocenic pojsc w kolejne
}

int  main()
{
    //lista sasiedztwa 

    return 0;
}
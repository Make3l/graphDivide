#include <stdio.h>

typedef struct {
    int vertex;
    int *next;
}Node;

void dfs(Node *nodes, int partitionsTab[],int curPartSize,int maxPartSize)
{
    if(curPartSize>=maxPartSize)
        return;

    Node *node_it=nodes;
    while(node_it)
    {
        if(partitionsTab[node_it->vertex]!=-1)//doesnt belong to specyfic partition
        {
            
        }
        node_it=node_it->next;
    }
}

int  main()
{
    //lista sasiedztwa 

    return 0;
}
#include "graph.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>



// helpers

void countOuterPartitionConnections(int outerConnectionsnections[], Node *neighbourList[], int startNode, int partitionTab[])//counts number of outer connections of i-th partition 
{
    int curPartition = partitionTab[startNode];
    Node *node_it = neighbourList[startNode];
    while (node_it)//iterates through startNode's neighbours and checks if they are in the same partition if not adds to outerConnection counter
    {
        if (partitionTab[node_it->vertex] != curPartition)
            outerConnectionsnections[curPartition]++;
        node_it = node_it->next;
    }
}

int evaluateNeighbours(Node *neighbourList[], MarkedNeighbours neighbour, int vertexDegree[], int curPartId, int partitionsTab[], int curPartSize, int pMin) // evaluates vertex based of innerConnections(within partition), outerConnectionsnections(out of partition),
{
    //"w" are weights for calculating score based on 3 factors mentioned above
    int w1 = 2;
    int w2 = 5;
    int w3 = 1;
    int score = 0;
    int innerConnections = 0;
    Node *neighbour_id = neighbourList[neighbour.id]; // neighbours of evalueted neighbour(vertex)
    while (neighbour_id)
    {
        if (vertexDegree[neighbour_id->vertex] == 1)
        {
            if (curPartSize >= pMin)
            {
                score += 300; // boost score in order to close this single "tunnel"
            }
            else
            {
                score -= 50; // leave it for now
            }
        }
        if (partitionsTab[neighbour_id->vertex] == curPartId) // calculates innerConnections
            innerConnections++;
        neighbour_id = neighbour_id->next;
    }
    int outerConnectionsnections = vertexDegree[neighbour.id] - innerConnections;
    score += w1 * innerConnections - w2 * outerConnectionsnections - w3 * vertexDegree[neighbour.id] + 100; // added 100 to convert negative values to positive
    return score;
}

int qsortComparator(const void *a, const void *b) // custom comparator for sorting neighbours by score
{
    const MarkedNeighbours *ma = (const MarkedNeighbours *)a;
    const MarkedNeighbours *mb = (const MarkedNeighbours *)b;
    return mb->score - ma->score;
}

int countUnassignedNeighbours(Node *neighbours, int partitionTab[])//counts unassigned neighbours
{
    int count=0;
    Node* it=neighbours;
    while(it)
    {
        if(partitionTab[it->vertex]==-1)
            count++;
        it=it->next;
    }
    return count;
}

int countEdgesAmongUnassigned(Node *neighbourList[],int partitionTab[], int startNode)//function counts density, how many connections are between two neighbours of startNode
{
    Node *it=neighbourList[startNode];//iterator of startNode's neighbours
    int connections=0;
    int neighboursTab[2048];//table of startNode's neighbours
    int size=0;
    while(it)
    {
        if(partitionTab[it->vertex]==-1)
            neighboursTab[size++]=it->vertex;
        it=it->next;
    }

    int u=0,w=0;
    for(int i=0;i<size-1;i++)//checks how many connections there are between each unique vertex pair (u,w), and counts it in "connctions"
        for(int j=i+1;j<size;j++)
        {
            u=neighboursTab[i];
            w=neighboursTab[j];
            for(Node *it=neighbourList[u];it;it=it->next)
                if(it->vertex==w)
                {
                    connections++;
                    break;
                }   
        }
        return connections;
}


long scoreVertex(int v, Node *neighbourList[], int partitionTab[])//counst a vertex score based on unassigned neighbours and neighbours conneections density
{
    int w1=10; // weight for every unassigned neighbour
    int w2=1; // weight for every edge between unassigned nieghbours(density)
    int degUn = countUnassignedNeighbours(neighbourList[v], partitionTab);
    if (degUn <= 1)
        return LONG_MIN; // skip weak
    int dens = countEdgesAmongUnassigned(neighbourList, partitionTab, v);
    return (long)w1 * degUn + (long)w2 * dens;
}

int findBestPartitionStart(int partitionTab[], int n, Node *neighbourList[])//finds best node to start partition 
{
    int bestNode = -1;
    long bestScore = LONG_MIN;

    for (int i = 0; i < n; i++)
    {
        if (partitionTab[i] != -1)//if already assigned skip
            continue;
        long score = scoreVertex(i, neighbourList, partitionTab);//evaluate this vertex
        if (score > bestScore)
        {
            bestScore = score;
            bestNode = i;
        }
    }

    // if nonting interesting finded take first vetrex
    if (bestNode == -1)
    {
        for (int i = 0; i < n; i++)
        {
            if (partitionTab[i] == -1)
                return i;
        }
    }

    // printf("WYBRANO WIERZCHOŁEK %d, score = %ld\n", bestNode, bestScore);
    return bestNode;
}

int findBestNearPartition(Node *neighbourList[], int partitionTab[], int partSize[], int unassignedNode, int k, double p, int n) // searches for best partition that is near that node
{
    Node *curNeighbour = neighbourList[unassignedNode];
    MarkedNeighbours *partScore = calloc(k, sizeof(MarkedNeighbours)); // table to rank partitions, and store pair(socre,id)
    for (int i = 0; i < k; i++)
        partScore[i].id = i;
    while (curNeighbour) // assigns score to partitions based of how many connections unassigned node has to that partition
    {
        if (partitionTab[curNeighbour->vertex] >= 0)
            partScore[partitionTab[curNeighbour->vertex]].score++;
        curNeighbour = curNeighbour->next;
    }
    qsort(partScore, k, sizeof(MarkedNeighbours), qsortComparator);

    int minPartitionId = -1;
    int minPartitionSize = INT_MAX;
    int maxPartitionSize = INT_MIN;
    for (int i = 0; i < k; i++)
    {
        if (partSize[i] < minPartitionSize) // used if instead of min macro, because i might need minPartitionId
        {
            minPartitionSize = partSize[i];
            minPartitionId = i;
        }
        maxPartitionSize = max(maxPartitionSize, partSize[i]);
    }

    if ((double)(maxPartitionSize - minPartitionSize) / (double)n > p) // if one partition is too small (choose it regardless, of other's score)
        return minPartitionId;

    int maxPossibleSize = (double)minPartitionSize * (1.0 + p);
    for (int i = 0; i < k; i++)
    {
        if (partScore[i].score <= 0) // partitions not near vertex, maby evaluate later(base on size, better choose smaller)
            break;
        if (partSize[partScore[i].id] + 1 <= maxPossibleSize) //+1 because, current unassignedNode may be added there
        {
            return partScore[i].id;
        }
    }

    // not near, return the smallest partition
    return minPartitionId;
}

void assignRemainingNodes(Node *neighbourList[], int partitionTab[], int partSize[], int n, int k, double p) // fuunction that assigns unassigned node
{
    int choosedPartition = 0;
    for (int i = 0; i < n; i++)
    {
        if (partitionTab[i] != -1)
            continue;
        choosedPartition = findBestNearPartition(neighbourList, partitionTab, partSize, i, k, p, n); // return best partition id
        partSize[choosedPartition]++;
        partitionTab[i] = choosedPartition;
    }
}

// logic

/*
I used iterative version(one function below this one) of dfs because it uses less memory than recursion type
*/
void dfs(Node *neighbourList[], int partitionsTab[], int *curPartSize, int maxFirstPartSize, int vertexDegree[], int current, int curPartId, long long average[], int pMin, int pMax) // vertexDegree->connction counter,
{
    if (partitionsTab[current] != -1) // if visited return
        return;
    partitionsTab[current] = curPartId; // assigns a partition
    (*curPartSize)++;

    if (curPartId == 0 && maxFirstPartSize <= (*curPartSize)) // if it is first partition do it till fixed size (maxFirstPartSize ) == n/k (numberOfVerticies/numberOfPartiton)
        return;
    else if (curPartId != 0 && pMax < (*curPartSize)) // if not first partition do it till pMax
        return;

    Node *node_it = neighbourList[current];
    int elemCounter = 0;
    MarkedNeighbours *neighbours = malloc(vertexDegree[current] * sizeof(MarkedNeighbours));
    while (node_it) // iterates through currentNode's neighbours
    {
        if (partitionsTab[node_it->vertex] == -1) // doesnt belong to specyfic partition
        {
            neighbours[elemCounter].id = node_it->vertex;// assigns id
            neighbours[elemCounter].score = evaluateNeighbours(neighbourList, neighbours[elemCounter], vertexDegree, curPartId, partitionsTab, (*curPartSize), pMin); // assigns score
            elemCounter++;
        }
        node_it = node_it->next;
    }

    qsort(neighbours, elemCounter, sizeof(MarkedNeighbours), qsortComparator); // sorts in descending order by nieghbour score

    for (int i = 0; i < elemCounter; i++)
    {
        if (curPartId != 0)
        {
            if (pMax < (*curPartSize) || (neighbours[i].score < average[0] && (*curPartSize) >= pMin)) // if size is exceeded break, or when (score is below average and minSize is reached) break
                break;
        }
        else
        {
            if (maxFirstPartSize <= (*curPartSize)) // if first partition size is exceeded break
                break;
        }

        if (partitionsTab[neighbours[i].id] == -1) // if unvisited
        {
            average[1] += neighbours[i].score; // adding to current average sum
            dfs(neighbourList, partitionsTab, curPartSize, maxFirstPartSize, vertexDegree, neighbours[i].id, curPartId, average, pMin, pMax);
        }
    }
    free(neighbours);
}

void dfsIterative(Node *neighbourList[], int partitionsTab[], int *curPartSize, int maxFirstPartSize, int vertexDegree[], int current, int curPartId, long long average[], int pMin, int pMax, int n) // vertexDegree->connction counter,
{
    MarkedNeighbours *stack = malloc(10 * n * sizeof(MarkedNeighbours)); // malocs 10x the size because sometimes normal size can be exceeded
    int stackSize = 0;
    stack[stackSize++] = (MarkedNeighbours){current, INT_MAX}; // adding first element on stack

    while (stackSize > 0)
    {
        MarkedNeighbours curNode = stack[--stackSize];
        if (partitionsTab[curNode.id] != -1) // if visited skip this vertex
            continue;

        partitionsTab[curNode.id] = curPartId;
        (*curPartSize)++;

        //code below checks for overflows
        if (curNode.score > 0 && average[1] > LLONG_MAX - curNode.score)
        {
            fprintf(stderr, "Overflow: average[1] + curNode.score > LLONG_MAX\n");
            exit(1);
        }
        else if (curNode.score < 0 && average[1] < LLONG_MIN - curNode.score)
        {
            fprintf(stderr, "Underflow: average[1] + curNode.score < LLONG_MIN\n");
            exit(1);
        }

        average[1] += curNode.score;// adding to current average sum

        if (curPartId != 0)
        {
            if (pMax <= *curPartSize)//do it till partition reaches pMax
            {
                break;
            }

            if (curNode.score < (int)(average[0] * 0.8) && *curPartSize >= pMin)//when score is below 80% average and minSize is reached -> continue
                continue;
        }

        if (curPartId == 0)
        {
            if (maxFirstPartSize <= (*curPartSize))// do it till first partition reaches maxFirstPartSize
            {
                break;
            }
        }

        Node *node_it = neighbourList[curNode.id];
        int elemCounter = 0;
        MarkedNeighbours *neighbours = malloc(vertexDegree[curNode.id] * sizeof(MarkedNeighbours));
        if (!stack || !neighbours)
        {
            fprintf(stderr, "Allocation failed in dfsIterative\n");
            exit(1);
        }

        while (node_it)// iterates through currentNode's neighbours
        {
            if (partitionsTab[node_it->vertex] == -1)
            {
                neighbours[elemCounter].id = node_it->vertex;// assigns id
                neighbours[elemCounter].score = evaluateNeighbours(neighbourList, neighbours[elemCounter], vertexDegree, curPartId, partitionsTab, (*curPartSize), pMin);// assigns score
                elemCounter++;
            }
            node_it = node_it->next;
        }

        /*
        for (int i = 0; i < elemCounter; i++)
        {
            printf("  DFS %d: sąsiad %d -> score = %d (deg = %d)\n", curPartId, neighbours[i].id, neighbours[i].score, vertexDegree[neighbours[i].id]);
        }
        */

        qsort(neighbours, elemCounter, sizeof(MarkedNeighbours), qsortComparator);// sorts in descending order by nieghbour score

        for (int i = elemCounter - 1; i >= 0; i--)//iterates by neighbours in reverse order because "best" nodes need to be on top of the stack
        {
            if (partitionsTab[neighbours[i].id] == -1)
            {
                if (stackSize >= 10 * n)
                {
                    fprintf(stderr, "Stack overflow in dfsIterative()\n");
                    break;
                }
                stack[stackSize++] = neighbours[i];//adds neighbours to the stack
            }
        }

        free(neighbours);
    }
    free(stack);
}

// convert

Node **convertMatrixToList(int *neighboursMatrix[], int n) // convert neighbourMatrix to neighbourList
{
    /*
    The idea of this neihbourList is to minimize the memory usage,
    example of the idea, is that for exaple there are connections 1-2,1-3,1-5,1-6
    neihbourList[1]={2,3,5,6} <-this will be linked list
    */
    Node **mapNeighbours = malloc(n * sizeof(Node *));
    Node *node_index;
    Node *dummy_node;
    for (int i = 0; i < n; i++)
    {
        dummy_node = malloc(sizeof(Node));
        node_index = dummy_node;
        mapNeighbours[i] = NULL;
        for (int j = 0; j < n; j++)
        {
            if (neighboursMatrix[i][j] == 1) // if there is connection creates new Node
            {
                node_index->next = malloc(sizeof(Node));
                node_index = node_index->next;
                node_index->next = NULL;
                node_index->vertex = j;
            }
        }
        mapNeighbours[i] = dummy_node->next;
        free(dummy_node);
    }
    return mapNeighbours;
}

// create
int *createOuterConnections(Node *neighbourList[], int partitionTab[], int *partitions[], int partSize[], int k, int n)//creates and fills outerConnectionTab, wchich represents number of outerConnection of i-th partition
{
    int *outerConnections = calloc(k, sizeof(int));

    for (int i = 0; i < k; i++)
        for (int j = 0; j < partSize[i]; j++)
            countOuterPartitionConnections(outerConnections, neighbourList, partitions[i][j], partitionTab);
    
    return outerConnections;
}

int **createPartition(int partitionsTab[], int n, int k, int partitionSize[]) // creates a table which stores indexes of nodes that belong at that partition
{
    int **partition = malloc(k * sizeof(int *));
    for (int i = 0; i < k; i++)
        partition[i] = malloc(partitionSize[i] * sizeof(int));

    int elemCounter = 0;
    for (int i = 0; i < k; i++)
    {
        elemCounter = 0; // counts number of current elements in partition[i], in order to add element to current end(push it back)
        for (int j = 0; j < n && elemCounter < partitionSize[i]; j++)
        {
            if (partitionsTab[j] == i) // if node belongs to i-th partition ad it
            {
                partition[i][elemCounter] = j;
                elemCounter++;
            }
        }
    }

    return partition;
}

int *createVertexDegree(Node **neighbourList, int n) // creates table that stores number of connections that node has
{
    int *vertexDegree = calloc(n, sizeof(int));
    if (!vertexDegree)
    {
        perror("calloc");
        exit(1);
    }

    for (int i = 0; i < n; i++)
    {
        Node *it = neighbourList[i];
        while (it) // loop trought i-th node neighbours and count them
        {
            vertexDegree[i]++;
            it = it->next;
        }
    }
    return vertexDegree;
}

int *createPartitionTab(int n) // just malloc partitionTab and set all it's values to -1, because there will never be -1, partition
{
    int *partitionTab = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        partitionTab[i] = -1;
    return partitionTab;
}

// creates basic test n=12 graph with 3 columns and 4 rows, conencted to up,down,right ,left
int **createBasicTestGraph(int n)
{
    if (n != 12)
    {
        fprintf(stderr, "Unfortunatelly this test is only to 12 vecticies graph :c");
        return NULL;
    }

    int **matrix = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        matrix[i] = calloc(n, sizeof(int));
    }
    int columns = 3; // hard coded test
    int rows = 4;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
        {
            if (i - 1 >= 0) // if not out of bounderies adds connction
                matrix[i * columns + j][(i - 1) * columns + j] = 1;
            if (i + 1 < rows)
                matrix[i * columns + j][(i + 1) * columns + j] = 1;
            if (j - 1 >= 0)
                matrix[i * columns + j][i * columns + j - 1] = 1;
            if (j + 1 < columns)
                matrix[i * columns + j][i * columns + j + 1] = 1;
        }

    return matrix;
}

// print

void printPartitionsTab(int *partitions, int n, int k) // just prints partitionTab
{
    printf("== Divide on %d parts ==\n", k);
    for (int i = 0; i < k; i++)
    {
        printf("Partition %d: ", i);
        for (int j = 0; j < n; j++)
        {
            if (partitions[j] == i)
                printf("%d ", j);
        }
        printf("\n");
    }
}

void printConnections(int **matrix, int n) // prints connections from connectinMatrix
{
    printf("== Connections ==\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (matrix[i][j])
            {
                printf("%d -> %d\n", i, j);
            }
        }
    }
}

void printPartition(int **partition, int k, int partitionSize[]) // prints partitions
{
    printf("Partitions:\n");
    for (int i = 0; i < k; i++)
    {
        printf("Partition %d: ", i);
        for (int j = 0; j < partitionSize[i]; j++)
            printf(" %d, ", partition[i][j]);
        printf("\n");
    }
    printf("\n");
}

void printOuterConnections(int *outerConnections, int k) // prints number of partition's outer connections
{
    printf("Partitions outer connections:\n");
    for (int i = 0; i < k; i++)
    {
        printf("Partition %d <-> %d\n", i, outerConnections[i]);
    }
    printf("\n");
}

void printPartitionsSizes(int partSize[], int k) // prints partitions sizes
{
    printf("Partition sizes:\n");
    for (int i = 0; i < k; i++)
    {
        printf("Partition %d size = %d\n", i, partSize[i]);
    }
    printf("\n");
}

// free

void freeAll(int **neighbourMatrix, Node *neighbourList[], int partitionTab[], int vertexDegree[], int *partition[], int outerConnections[], int n, int k)
{
    if (neighbourMatrix)
    {
        for (int i = 0; i < n; i++) // cleanning neighbourMatrix
            free(neighbourMatrix[i]);
        free(neighbourMatrix);
    }

    if (neighbourList)
    {
        Node *cur;
        Node *next;
        for (int i = 0; i < n; i++) // cleaning neighbourList
        {
            cur = neighbourList[i];
            while (cur)
            {
                next = cur->next;
                free(cur);
                cur = next;
            }
        }
        free(neighbourList);
    }

    free(partitionTab); // cleanig partition tab(nodes[i]=partition,)

    free(vertexDegree); // cleanig vertex degree

    if (partitionTab)
    {
        for (int i = 0; i < k; i++) // cleaning partition(partition:nodes,...)
            free(partition[i]);
        free(partition);
    }

    free(outerConnections); // cleaning outer connections
}

void freeAllWithoutMatrix(Node *neighbourList[], int partitionTab[], int vertexDegree[], int *partition[], int outerConnections[], int n, int k) // without neighbour matrix
{
    Node *cur;
    Node *next;
    for (int i = 0; i < n; i++) // cleaning neighbourList
    {
        cur = neighbourList[i];
        while (cur)
        {
            next = cur->next;
            free(cur);
            cur = next;
        }
    }
    free(neighbourList);

    free(partitionTab); // cleanig partition tab(nodes[i]=partition,)

    free(vertexDegree); // cleanig vertex degree

    for (int i = 0; i < k; i++) // cleaning partition(partition:nodes,...)
        free(partition[i]);
    free(partition);

    free(outerConnections); // cleaning outer connections
}
#include "graph.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define WEIGHT_DEG 10 // waga za każdy nieprzydzielony sąsiad
#define WEIGHT_DENS 1 // waga za każdą krawędź pomiędzy nieprzydzielonymi sąsiadami

// helpers

void countOuterPartitionConnections(int outerConnectionsnections[], Node *neighbourList[], int startNode, int partitionTab[])
{
    int curPartition = partitionTab[startNode];
    Node *node_it = neighbourList[startNode];
    while (node_it)
    {
        if (partitionTab[node_it->vertex] != curPartition)
            outerConnectionsnections[curPartition]++;
        node_it = node_it->next;
    }
}

int evaluateNeighbours(Node *neighbourList[], MarkedNeighbours neighbour, int vertexDegree[], int curPartId, int partitionsTab[], int curPartSize, int pMin)
{
    int w1 = 2;
    int w2 = 5;
    int w3 = 1;
    int score = 0;
    int innerConnections = 0;
    Node *neighbour_id = neighbourList[neighbour.id]; // neighbours of evalueted neighbour
    while (neighbour_id)
    {
        if (vertexDegree[neighbour_id->vertex] == 1)
        {
            if (curPartSize >= pMin)
            {
                score += 300; // chętnie zamknij tę gałąź
            }
            else
            {
                score -= 50; // na razie nie
            }
        }
        if (partitionsTab[neighbour_id->vertex] == curPartId)
            innerConnections++;
        neighbour_id = neighbour_id->next;
    }
    int outerConnectionsnections = vertexDegree[neighbour.id] - innerConnections;
    score += w1 * innerConnections - w2 * outerConnectionsnections - w3 * vertexDegree[neighbour.id] + 100; // added 100 to convert negative values to positive
    return score;
}

int qsortComparator(const void *a, const void *b)
{
    const MarkedNeighbours *ma = (const MarkedNeighbours *)a;
    const MarkedNeighbours *mb = (const MarkedNeighbours *)b;
    return mb->score - ma->score;
}

// zwraca liczbę nieprzydzielonych sąsiadów
int countUnassignedNeighbors(Node *nbrs, int partitionTab[])
{
    int cnt = 0;
    for (Node *it = nbrs; it; it = it->next)
        if (partitionTab[it->vertex] == -1)
            cnt++;
    return cnt;
}

// liczy, ile krawędzi jest pomiędzy parami nieprzydzielonych sąsiadów v
int countEdgesAmongUnassigned(Node *nbrs, int partitionTab[], Node *neighbourList[])
{
    // zbierz najpierw nieprzydzielonych sąsiadów do tablicy:
    int tmp[1024], t = 0;
    for (Node *it = nbrs; it; it = it->next)
    {
        if (partitionTab[it->vertex] == -1)
            tmp[t++] = it->vertex;
    }
    // policz krawędzie między każdą parą (u,w)
    int edges = 0;
    for (int i = 0; i < t; i++)
    {
        int u = tmp[i];
        for (int j = i + 1; j < t; j++)
        {
            int w = tmp[j];
            // sprawdź czy istnieje krawędź u–w
            for (Node *it = neighbourList[u]; it; it = it->next)
            {
                if (it->vertex == w)
                {
                    edges++;
                    break;
                }
            }
        }
    }
    return edges;
}

// nowa heurystyka zwraca score = WEIGHT_DEG * degUnassigned + WEIGHT_DENS * localDensity
long scoreVertex(int v, Node *neighbourList[], int partitionTab[])
{
    int degUn = countUnassignedNeighbors(neighbourList[v], partitionTab);
    if (degUn <= 1)
        return LONG_MIN; // pomiń liście i węzły bez potencjału rozrostu
    int dens = countEdgesAmongUnassigned(neighbourList[v], partitionTab, neighbourList);
    return (long)WEIGHT_DEG * degUn + (long)WEIGHT_DENS * dens;
}

int findBestPartitionStart(int partitionTab[], int n, Node *neighbourList[])
{
    int bestNode = -1;
    long bestScore = LONG_MIN;

    for (int i = 0; i < n; i++)
    {
        if (partitionTab[i] != -1)
            continue;
        long sc = scoreVertex(i, neighbourList, partitionTab);
        if (sc > bestScore)
        {
            bestScore = sc;
            bestNode = i;
        }
    }

    // jeżeli nic nie znaleźliśmy (np. zostały liście), weź pierwszy nieprzydzielony
    if (bestNode == -1)
    {
        for (int i = 0; i < n; i++)
        {
            if (partitionTab[i] == -1)
                return i;
        }
    }

    //printf("WYBRANO WIERZCHOŁEK %d, score = %ld\n", bestNode, bestScore);
    return bestNode;
}

int findBestNearPartition(Node *neighbourList[], int partitionTab[], int partSize[], int unassignedNode, int k, double p, int n) // searches for best partition that is near that node
{
    Node *curNeighbour = neighbourList[unassignedNode];
    MarkedNeighbours *partScore = calloc(k, sizeof(MarkedNeighbours));
    for (int i = 0; i < k; i++)
        partScore[i].id = i;
    while (curNeighbour)
    {
        if (partitionTab[curNeighbour->vertex] >= 0)
            partScore[partitionTab[curNeighbour->vertex]].score++;
        curNeighbour = curNeighbour->next;
    }
    qsort(partScore, k, sizeof(MarkedNeighbours), qsortComparator);

    // max possible partition size
    int minPartitionId = -1;
    int minPartitionSize = INT_MAX;
    int maxPartitionSize = INT_MIN;
    for (int i = 0; i < k; i++)
    {
        if (partSize[i] < minPartitionSize)
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
        if (partSize[partScore[i].id] + 1 <= maxPossibleSize)
        {
            return partScore[i].id;
        }
    }

    // not near, return the smallest partition
    return minPartitionId;
}

void assignRemainingNodes(Node *neighbourList[], int partitionTab[], int partSize[], int n, int k, double p)
{
    int choosedPartition = 0;
    for (int i = 0; i < n; i++)
    {
        if (partitionTab[i] != -1)
            continue;
        choosedPartition = findBestNearPartition(neighbourList, partitionTab, partSize, i, k, p, n);
        partSize[choosedPartition]++;
        partitionTab[i] = choosedPartition;
    }
}

// logic

void dfs(Node *neighbourList[], int partitionsTab[], int *curPartSize, int maxFirstPartSize, int vertexDegree[], int current, int curPartId, long long average[], int pMin, int pMax) // vertexDegree->connction counter,
{
    if (partitionsTab[current] != -1)
        return;
    partitionsTab[current] = curPartId;
    (*curPartSize)++;

    if (curPartId == 0 && maxFirstPartSize <= (*curPartSize))
        return;
    else if (curPartId != 0 && pMax < (*curPartSize))
        return;

    Node *node_it = neighbourList[current];
    int elemCounter = 0;
    MarkedNeighbours *neighbours = malloc(vertexDegree[current] * sizeof(MarkedNeighbours));
    while (node_it)
    {
        if (partitionsTab[node_it->vertex] == -1) // doesnt belong to specyfic partition
        {
            neighbours[elemCounter].id = node_it->vertex;
            neighbours[elemCounter].score = evaluateNeighbours(neighbourList, neighbours[elemCounter], vertexDegree, curPartId, partitionsTab, (*curPartSize), pMin);
            elemCounter++;
        }
        node_it = node_it->next;
    }

    qsort(neighbours, elemCounter, sizeof(MarkedNeighbours), qsortComparator);
    for (int i = 0; i < elemCounter; i++)
    {
        if (curPartId != 0)
        {
            if (pMax < (*curPartSize) || (neighbours[i].score < average[0] && (*curPartSize) >= pMin)) // not worth it
                break;
        }
        else
        {
            if (maxFirstPartSize <= (*curPartSize))
                break;
        }

        if (partitionsTab[neighbours[i].id] == -1)
        {
            if (curPartId != 0 && *curPartSize >= pMax)
                return;
            average[1] += neighbours[i].score; // added to current average sum
            dfs(neighbourList, partitionsTab, curPartSize, maxFirstPartSize, vertexDegree, neighbours[i].id, curPartId, average, pMin, pMax);
        }
    }
    free(neighbours);
}

void dfsIterative(Node *neighbourList[], int partitionsTab[], int *curPartSize, int maxFirstPartSize, int vertexDegree[], int current, int curPartId, long long average[], int pMin, int pMax, int n) // vertexDegree->connction counter,
{
    /*
    if (*curPartSize == 0)
    {
        Node *n_id = neighbourList[current];
        int counter = 0;
        while (n_id)
        {
            if (partitionsTab[n_id->vertex] == -1)
                counter++;
            n_id = n_id->next;
        }
        printf("    PART %d, vertexDegree[start] = %d, realnie ma %d drog\n", curPartId, vertexDegree[current], counter);
    }
    */

    MarkedNeighbours *stack = malloc(10 * n * sizeof(MarkedNeighbours));
    int stackSize = 0;
    stack[stackSize++] = (MarkedNeighbours){current, INT_MAX};

    while (stackSize > 0)
    {
        MarkedNeighbours curNode = stack[--stackSize];
        if (partitionsTab[curNode.id] != -1)
            continue;

        partitionsTab[curNode.id] = curPartId;
        (*curPartSize)++;
        if (curNode.score < INT_MAX)
        {
            if (curNode.score > 0 && average[1] > LLONG_MAX - curNode.score)
            {
                fprintf(stderr, "Overflow: average[1] + curNode.score > LLONG_MAX\n");
                exit(1);
            }
            if (curNode.score < 0 && average[1] < LLONG_MIN - curNode.score)
            {
                fprintf(stderr, "Underflow: average[1] + curNode.score < LLONG_MIN\n");
                exit(1);
            }
            average[1] += curNode.score;
        }

        if (curPartId != 0)
        {
            if (pMax <= *curPartSize)
            {
                break;
            }

            if (curNode.score < (int)(average[0] * 0.8) && *curPartSize >= pMin)
                continue;
        }

        if (curPartId == 0)
        {
            if (maxFirstPartSize <= (*curPartSize))
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

        while (node_it)
        {
            if (partitionsTab[node_it->vertex] == -1)
            {
                neighbours[elemCounter].id = node_it->vertex;
                neighbours[elemCounter].score = evaluateNeighbours(neighbourList, neighbours[elemCounter], vertexDegree, curPartId, partitionsTab, (*curPartSize), pMin);
                elemCounter++;
            }
            node_it = node_it->next;
        }

        for (int i = 0; i < elemCounter; i++)
        {
            printf("  DFS %d: sąsiad %d -> score = %d (deg = %d)\n", curPartId, neighbours[i].id, neighbours[i].score, vertexDegree[neighbours[i].id]);
        }
        qsort(neighbours, elemCounter, sizeof(MarkedNeighbours), qsortComparator);

        for (int i = elemCounter - 1; i >= 0; i--)
        {
            if (partitionsTab[neighbours[i].id] == -1)
            {
                if (stackSize >= 10 * n)
                {
                    fprintf(stderr, "Stack overflow in dfsIterative()\n");
                    break;
                }
                stack[stackSize++] = neighbours[i];
            }
        }

        free(neighbours);
    }
    free(stack);
}

// convert

Node **convertMatrixToList(int *neighboursMatrix[], int n) // convert neighbourMatrix to neighbourList
{
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
            if (neighboursMatrix[i][j] == 1)
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
int *createOuterConnections(Node *neighbourList[], int partitionTab[], int *partitions[],int partSize[], int k, int n)
{
    int *outerConnectionsnections = calloc(k, sizeof(int));
    int *visited;

    for (int i = 0; i < k; i++)
    {
        visited = calloc(n, sizeof(int));
        for (int j = 0; j < partSize[i]; j++)
        {
            countOuterPartitionConnections(outerConnectionsnections, neighbourList, partitions[i][j], partitionTab);
        }
        free(visited);
    }
    return outerConnectionsnections;
}

int **createPartition(int partitionsTab[], int n, int k, int partitionSize[])
{
    int **partition = malloc(k * sizeof(int *));
    for (int i = 0; i < k; i++)
        partition[i] = malloc(partitionSize[i] * sizeof(int));

    int elemCounter = 0;
    for (int i = 0; i < k; i++)
    {
        elemCounter = 0;
        for (int j = 0; j < n && elemCounter < partitionSize[i]; j++)
        {
            if (partitionsTab[j] == i)
            {
                partition[i][elemCounter] = j;
                elemCounter++;
            }
        }
    }

    return partition;
}

int *createVertexDegree(Node **neighbourList, int n)
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
        while (it)
        {
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

// basic test n=12 graph with 3 columns and 4 rows, conencted to up,down,right ,left
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
            if (i - 1 >= 0)
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

void printPartitionsTab(int *partitions, int n, int k)
{
    printf("== Podział na %d części ==\n", k);
    for (int i = 0; i < k; i++)
    {
        printf("Partycja %d: ", i);
        for (int j = 0; j < n; j++)
        {
            if (partitions[j] == i)
                printf("%d ", j);
        }
        printf("\n");
    }
}

void printConnections(int **matrix, int n)
{
    printf("== Wszystkie połączenia z macierzy sąsiedztwa ==\n");
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

void printPartition(int **partition, int k, int partitionSize[])
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

void printOuterConnections(int *outerConnections, int k)
{
    printf("Partitions outer connections:\n");
    for (int i = 0; i < k; i++)
    {
        printf("Partition %d <-> %d\n", i, outerConnections[i]);
    }
    printf("\n");
}

void printPartitionsSizes(int partSize[],int k)
{
    printf("Partition sizes:\n");
    for(int i=0;i<k;i++)
    {
        printf("Partition %d size = %d\n",i,partSize[i]);
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
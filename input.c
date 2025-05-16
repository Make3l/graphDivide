#include "input.h"
#include "graph.h"

#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// parsuje pierwsze trzy linie pliku .csrrg:
/// 1) pomija (to jest max dł. wiersza),
/// 2) flattened indices (druga linia),
/// 3) rowPtr (trzecia linia);
/// zwraca *nVertices = (liczba elementów rowPtr)−1,
/// *nEntries = liczba flattened indices,
/// *rowPtr i *indices.
void parseCSR(const char *filename,size_t *nVertices,size_t *nEntries,int **rowPtr,int **indices)
{
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char buf[8192];
    char *tok;

    // 1) pomiń pierwszą linię
    if (!fgets(buf, sizeof buf, f))
    {
        fprintf(stderr, "EMPTY FILE?\n");
        exit(EXIT_FAILURE);
    }

    // 2) flattened indices
    if (!fgets(buf, sizeof buf, f))
    {
        fprintf(stderr, "MISSING indices line\n");
        exit(EXIT_FAILURE);
    }
    size_t capI = 128, cntI = 0;
    int *idx = malloc(capI * sizeof *idx);
    if (!idx)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    tok = strtok(buf, "; \r\n");
    while (tok)
    {
        if (cntI == capI)
        {
            capI *= 2;
            idx = realloc(idx, capI * sizeof *idx);
            if (!idx)
            {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }
        idx[cntI++] = atoi(tok);
        tok = strtok(NULL, "; \r\n");
    }

    // 3) rowPtr
    if (!fgets(buf, sizeof buf, f))
    {
        fprintf(stderr, "MISSING rowPtr line\n");
        exit(EXIT_FAILURE);
    }
    size_t capR = 64, cntR = 0;
    int *rp = malloc(capR * sizeof *rp);
    if (!rp)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    tok = strtok(buf, "; \r\n");
    while (tok)
    {
        if (cntR == capR)
        {
            capR *= 2;
            rp = realloc(rp, capR * sizeof *rp);
            if (!rp)
            {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }
        rp[cntR++] = atoi(tok);
        tok = strtok(NULL, "; \r\n");
    }
    if (cntR < 2)
    {
        fprintf(stderr, "BAD rowPtr (too short)\n");
        exit(EXIT_FAILURE);
    }

    fclose(f);

    *indices = idx;
    *nEntries = (int)cntI;
    *rowPtr = rp;
    *nVertices = (int)(cntR - 1);
}

void addConnection(Node *neighbourList[], int from, int to)
{
    Node *newNode = malloc(sizeof(Node));
    if (!newNode)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    newNode->vertex = to;
    newNode->next = neighbourList[from];
    neighbourList[from] = newNode;
}

Node **buildAdjList(size_t nVertices, int *rowPtr, int *indices)
{
    Node **neighbourList = calloc(nVertices, sizeof *neighbourList);
    if (!neighbourList)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (size_t u = 0; u < nVertices; u++)
    {
        for (int k = rowPtr[u]; k < rowPtr[u + 1]; k++)
        {
            int v = indices[k];
            if (v < 0 || (size_t)v >= nVertices)
            {
                fprintf(stderr, "ERROR: nieprawidłowy wierzchołek %d\n", v);
                exit(EXIT_FAILURE);
            }
            addConnection(neighbourList, u, v);
        }
    }
    return neighbourList;
}



Node **runCsrrg(const char *filename,int *n)
{
    size_t nV_sz, nE;
    int *rowPtr, *indices;

    // parsujemy: nV_sz ← #wierzchołków, nE ← #wejść w CSR
    parseCSR(filename, &nV_sz, &nE, &rowPtr, &indices);
    printf("Wierzchołków: %zu, wpisów w CSR: %zu\n", nV_sz, nE);

    // tu masz już int n jako liczbę wierzchołków
    (*n) = (int)nV_sz;

    // teraz przekazujesz n do buildAdjList
    return buildAdjList(nV_sz, rowPtr, indices);
}
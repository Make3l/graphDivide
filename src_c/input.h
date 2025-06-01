#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>
#include "graph.h" 

void parseCSR(const char *filename,size_t *nVertices,size_t *nEntries,int **rowPtr,int **indices);
void addConnection(Node* neighbourList[], int from, int to);//creates connection in neighbourList
Node **buildAdjList(size_t nVertices, int *rowPtr, int *indices);
Node **runCsrrg(const char *filename,int *n);//just runs .csrrg files, (returns neighbourList and modifies n)
int **readMaxtrixFromFile(char *filePath,int *n);//converts matrix in file to int** matrix

#endif
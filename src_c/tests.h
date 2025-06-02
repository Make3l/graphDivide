#ifndef TESTS_H
#define TESTS_H

int** createTestGraph(int n); //creates matrix (4x3) with 12 nodes, 3 columns and 4 rows,connected: up, down, right
int** createIrregularTestGraph(int n); //creates irregular matrix
int** createLargeTestGraph(int n, double connectionProbability); //creates matrix (n,n), that has some probability that random(or semi random because of choosen seed) edges are connected

#endif
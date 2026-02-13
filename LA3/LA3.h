#ifndef _MM_H
#define _MM_H


void assignA(double* mat, int n, int m);
void assignB(double* mat, int n, int m);
void dotProduct(double* A, double* B, double* C, int n);
double* mm(double* A, double* B, int n, int m, int p);
void MPI_mm(double* A, double* B, double* C, int n, int m, int p, int rank, int numranks);
void printMatrix(double* mat, int n, int m);
void transposeMatrix(double* A, int m, int n);

#endif

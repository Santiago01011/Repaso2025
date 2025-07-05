#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef void print_callback(void *);


int loadMatFromArch(const char* filePath, float** mat, int* dim);
int printMatFromArch(const char* filePath);
int printMatDiagAndTri(float** mat, int dim);
int printMatTriInf(void** mat, int dim);
int printMatTriSup(void** mat, int dim);
int printMatTriR(void** mat, int dim);
int printMatTriL(void** mat, int dim);
int printMatMDiag(void** mat, int dim);
int printMatIDiag(void** mat, int dim);
#endif // MAIN_H_INCLUDED

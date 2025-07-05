#ifndef LIBVECTOR_H
#define LIBVECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_INICIAL 10


typedef struct {
    void* data;
    int cantElem, totalLength;
    size_t tamElem;
} tVector;

int crearVector(tVector* vec, size_t tamElem);
void vectorLiberar(tVector *vec);
int insertarEnVectorFinal(tVector* vec, void* elem);
int insertarEnVectorOrdenado(tVector* vec, void* elem, int (*cmp)(const void*, const void*));
int ordenarVectorInsercion(tVector* vec, int (*cmp)(const void*, const void*));
int myBSort(tVector* vec, int (*cmp)(const void*, const void*), const void*);
int cargarVectorDeArchivo(tVector *vec, const char *nombreArchivo, void* elem);
void mapVector(tVector *vec, void(*action)(void* elem));


void printEntero(void* elem);
void printCadena(void* elem);
int cmpInt(const void* a, const void* b);
int cmpString(const void* a, const void* b);
int cmpCuenta(const void* a, const void* b);





#endif // LIBVECTOR_H
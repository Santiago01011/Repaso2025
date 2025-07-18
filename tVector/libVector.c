#include "libVector.h"
#include <string.h>


int crearVector(tVector *vec, size_t tamElem)
{
    if (!vec)
        return 0;
    vec->tamElem = tamElem;
    vec->cantElem = 0;
    vec->totalLength = TAM_INICIAL;
    vec->data = malloc(vec->totalLength * tamElem);
    if (!vec->data)
        return 0;
    return 1;
}

void vectorLiberar(tVector *vec)
{
    if (!vec || !vec->data)
        return;
    free(vec->data);
    vec->data = NULL;
    vec->cantElem = 0;
    vec->totalLength = 0;
}

int insertarEnVectorOrdenado(tVector *vec, void *elem, int (*cmp)(const void *, const void *))
{
    int pos = 0;
    if (!vec || !vec->data)
        return 0;
    if (vec->cantElem >= vec->totalLength)
    {
        void *aux = realloc(vec->data, vec->totalLength * 2 * vec->tamElem);
        if (!aux)
            return 0;
        vec->data = aux;
        vec->totalLength *= 2;
    }
    
    while (pos < vec->cantElem && cmp(elem, (char *)vec->data + pos * vec->tamElem) > 0)
        pos++;
    
    memmove((char *)vec->data + (pos + 1) * vec->tamElem, 
            (char *)vec->data + pos * vec->tamElem, 
            (vec->cantElem - pos) * vec->tamElem);
    
    memcpy((char *)vec->data + pos * vec->tamElem, elem, vec->tamElem);
    vec->cantElem++;
    return 1;
}

/**
 * @brief Inserts an element into an ordered vector. If a duplicate is found,
 * it updates the existing element using the provided 'actualizar' function.
 *
 * @param vec           Pointer to the vector.
 * @param elem          Pointer to the element to be inserted/updated.
 * @param cmp           Comparison function to maintain order and find duplicates.
 * @param actualizar    Function to be called to update an existing element if a duplicate is found.
 * @return              1 on success, 0 on failure (e.g., memory allocation error).
 */
int vectorOrdInsertar_ALU(tVector* vec, const void* elem, int (*cmp)(const void*, const void*), void (*actualizar)(void*, const void*))
{
    // --- Step 1: Find the correct position ---
    // We iterate to find the first element that is NOT smaller than our new element.
    size_t pos = 0;
    char* data = (char*)vec->data; // Use char* for easy pointer arithmetic

    while (pos < vec->cantElem && cmp(elem, data + pos * vec->tamElem) > 0)
    {
        pos++;
    }

    // --- Step 2: Check for a duplicate at that position ---
    // If pos is still in bounds AND the element at pos is equal to our new element...
    if (pos < vec->cantElem && cmp(elem, data + pos * vec->tamElem) == 0)
    {
        // It's a duplicate. Call the update function and we're done.
        if (actualizar) // Make sure the update function is not NULL
        {
            actualizar(data + pos * vec->tamElem, elem);
        }
        return 1; // Success
    }

    // --- Step 3: Handle a new insertion ---
    // If we reached here, the element is new and needs to be inserted at 'pos'.

    // First, check if we have enough capacity.
    if (vec->cantElem == vec->totalLength)
    {
        size_t nuevaCap = vec->totalLength == 0 ? 1 : vec->totalLength * 2; // Start with 1, then double
        void* aux = realloc(vec->data, nuevaCap * vec->tamElem);

        if (!aux)
            return 0; // Failure: Could not allocate memory
        vec->data = aux;
        vec->cantElem = nuevaCap;
        data = (char*)vec->data; // The pointer might have changed after realloc
    }

    // Second, make room for the new element by shifting everything from 'pos' onwards.
    // memmove is safe for overlapping memory regions.
    memmove(data + (pos + 1) * vec->tamElem,  // Destination: one spot to the right
            data + pos * vec->tamElem,        // Source: the current position
            (vec->cantElem - pos) * vec->tamElem);  // Number of bytes to move

    // Third, copy the new element into the newly created space.
    memcpy(data + pos * vec->tamElem, elem, vec->tamElem);

    // Finally, update the element count.
    vec->cantElem++;

    return 1; // Success
}

int insertarEnVectorFinal(tVector *vec, void *elem)
{
    if (!vec->data)
        return 0;
    if (vec->cantElem >= vec->totalLength)
    {
        void *aux = realloc(vec->data, vec->totalLength * 2 * vec->tamElem);
        if (!aux)
            return 0;
        vec->data = aux;
        vec->totalLength *= 2;
    }
    memcpy((char *)vec->data + vec->cantElem * vec->tamElem, elem, vec->tamElem);
    vec->cantElem++;
    return 1;
}

int ordenarVectorInsercion(tVector* vec, int (*cmp)(const void*, const void*)) {
    if (!vec || !vec->data || vec->cantElem < 2)
        return 0;
    int i;
    char* data = (char*)vec->data;
    size_t tam = vec->tamElem;
    void* temp = malloc(tam);
    if (!temp)
        return 0;

    for (i = 1; i < vec->cantElem; i++) {
        memcpy(temp, data + i * tam, tam);
        int j = i - 1;
        while (j >= 0 && cmp(temp, data + j * tam) < 0) {
            memcpy(data + (j + 1) * tam, data + j * tam, tam);
            j--;
        }
        memcpy(data + (j + 1) * tam, temp, tam);
    }
    free(temp);
    return 1;
}

int myBSort(tVector *vec, int (*cmp)(const void *, const void *), const void *dato)
{
    int ini, med, fin, res;
    ini = 0;
    fin = vec->cantElem - 1;
    while(ini <= fin){
        med = (fin - ini) / 2 + ini;
        res = cmp(dato, vec->data + (med * vec->tamElem));
        if( res == 0 )
            return med;
        if( res < 0 )
            fin = med - 1;
        if( res > 0 )
            ini = med + 1;
    }
    return -1;
}

int cargarVectorDeArchivo(tVector *vec, const char *nombreArchivo, void *elem)
{
    if (!vec || !nombreArchivo)
        return 0;
    FILE *archivo = fopen(nombreArchivo, "rb");
    if (!archivo)
        return 0;
    
    while (fread(elem, vec->tamElem, 1, archivo) == 1)
    {
        if (!insertarEnVectorFinal(vec, elem))
        {
            fclose(archivo);
            return 0;
        }
    }
    return 1;
}

int grabarVectorEnArchivo(tVector *vec, const char *nombreArchivo, void *elem)
{
    if (!vec || !nombreArchivo)
        return 0;
    FILE *archivo = fopen(nombreArchivo, "wb");
    if (!archivo)
        return 0;
    for (int i = 0; i < vec->cantElem; i++)
    {
        memcpy(elem, (char *)vec->data + i * vec->tamElem, vec->tamElem);
        if (fwrite(elem, vec->tamElem, 1, archivo) != 1)
        {
            fclose(archivo);
            return 0;
        }
    }
    return 0;
}


void printEntero(void *elem)
{
    printf("[%d] ", *(int *)elem);
}

void printCadena(void *elem)
{
    printf("[%s] ", (char *)elem);
}

void mapVector(tVector *vec, void(*action)(void* elem)){
    if (!vec || !vec->data)
        return;
    for (int i = 0; i < vec->cantElem; i++)
        action((char *)vec->data + i * vec->tamElem);
}

int cmpInt(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

int cmpString(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

typedef struct {
    char codigo_cta[9]; // Ej: "123456/5"
    float saldo;;
} s_cuenta;

void printCuenta(void *elem)
{
    s_cuenta *cuenta = (s_cuenta *)elem;
    printf("Cuenta: %s, Saldo: %.2f\n", cuenta->codigo_cta, cuenta->saldo);
}

int cmpCuenta(const void *a, const void *b)
{
    return strcmp(((s_cuenta *)a)->codigo_cta, ((s_cuenta *)b)->codigo_cta);
}

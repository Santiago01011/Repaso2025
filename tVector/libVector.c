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
    void *data = vec->data;
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

    pos = myBSort(vec, cmp, elem);
    if( pos == -1 ){
        memcpy(data + (vec->tamElem * vec->cantElem), elem, vec->tamElem);
        vec->cantElem++;
        return 1;
    }
    memmove(data + (pos + 1)  * vec->tamElem,
            data + pos * vec->tamElem,
            (vec->cantElem - pos) * vec->tamElem);

    memcpy(data + pos * vec->tamElem, elem, vec->tamElem);
    vec->cantElem++;
    return 1;
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
        if (!insertarEnVectorOrdenado(vec, elem, cmpString))
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

int main(){

    // Ejemplo de uso con enteros
    printf("=== Ejemplo con enteros ===\n");
    tVector vec;
    if (crearVector(&vec, sizeof(int)))
    {
        int elem1 = 5, elem2 = 3, elem3 = 8, elem4 = 1, elem5 = 4, elem6 = 2, elem7 = 6, elem8 = 7;

        insertarEnVectorFinal(&vec, &elem1);
        insertarEnVectorFinal(&vec, &elem2);
        insertarEnVectorFinal(&vec, &elem3);
        insertarEnVectorFinal(&vec, &elem4);
        insertarEnVectorFinal(&vec, &elem5);
        insertarEnVectorFinal(&vec, &elem6);
        insertarEnVectorFinal(&vec, &elem7);
        insertarEnVectorFinal(&vec, &elem8);
        

        printf("Vector antes de ordenar:\n");
        mapVector(&vec, printEntero);

        printf("\n");
        ordenarVectorInsercion(&vec, cmpInt);
        printf("Vector despues de ordenar:\n");
       mapVector(&vec, printEntero);
       
        printf("\n\n");

        free(vec.data);
    }

    // Ejemplo de uso con cadenas de caracteres
    printf("=== Ejemplo con cadenas de caracteres ===\n");
    tVector vecString;
    if (crearVector(&vecString, 20 * sizeof(char))) // Suponemos maximo 20 caracteres por string
    {
        char elem1[] = "manzana";
        char elem2[] = "banana";
        char elem3[] = "zebra";
        char elem4[] = "abeja";
        char elem5[] = "abbeja";
        char elem6[] = "elefante";
        char elem7[] = "perro";
        char elem8[] = "tigre";

        insertarEnVectorFinal(&vecString, elem1);
        insertarEnVectorFinal(&vecString, elem2);
        insertarEnVectorFinal(&vecString, elem3);
        insertarEnVectorFinal(&vecString, elem4);
        insertarEnVectorFinal(&vecString, elem5);
        insertarEnVectorFinal(&vecString, elem6);
        insertarEnVectorFinal(&vecString, elem7);
        insertarEnVectorFinal(&vecString, elem8);

        printf("Vector de strings antes de ordenar:\n");
        mapVector(&vecString, printCadena);

        printf("\n");
        ordenarVectorInsercion(&vecString, cmpString);
        printf("Vector de strings despues de ordenar:\n");
        mapVector(&vecString, printCadena);
        printf("\n");

        vectorLiberar(&vecString);
    }

    // Ejemplo de uso con cargar y grabar vector desde un archivo
    printf("=== Ejemplo de cargar y grabar vector desde un archivo ===\n");

    return 0;
}
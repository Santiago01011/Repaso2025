/*****************************************************************************************************************
** Apellido/s, Nombre/s:                                      													**
**
** DNI:                                                       													**
**
******************************************************************************************************************
** Corrección																								   	**
** **********																								   	**
**
******************************************************************************************************************
** Nota:
*****************************************************************************************************************/

/*****************************************************************************************************************
** Descomente SOLUCION_ALU y comente SOLUCION_DOC, para probar su código: 										**
**																												*/
//	#define SOLUCION_DOC
	#define SOLUCION_ALU
/*
*****************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Comun.h>
#include <Cadena.h>
#include <Fecha.h>
#include <Vector.h>
#include <SolucionParcialCentroTeleTopicos.h>


#define PATH_CLI 1
#define PATH_PAGOS 2
#define PATH_LISTADO 3

void procesarPagos_ALU(const char* pathCli, const char* pathPagos, const char* pathListado);
void printIdx_ALU(void *elem, void* extra);
void printDeudores_ALU(void *elem, void* extra);
void mapVector_ALU(Vector *vec, Accion accion);
int Bsearch_ALU(Vector *vec, Cmp cmp, const void *dato);
int insertarEnVectorOrdenado_ALU(Vector *vec, void *elem, Cmp cmp);
int ordenarVectorInsercion(Vector* vec, Cmp cmp);
int cargarVectorDeArchivo_ALU(Vector *vec, const char *nombreArchivo, void *elem);
int insertarEnVectorFinal(Vector *vec, void *elem);
int cmpFechas_ALU(const void*, const void*);
int cmpDeudores_ALU(const void*, const void*);
int cmpCantFacturas_ALU(const void *d1, const void *d2);
void actualizarDeudor_ALU(void* actualizado, const void* actualizador);
int cmpNroClientes_ALU(const void *d1, const void *d2);
int vectorOrdInsertar_ALU(Vector* vec, const void* elem, Cmp cmp, Actualizar actualizar);
void vectorObtenerElemento(Vector* vec, int pos, void* elem);

int main(int argc, char* argv[])
{
	generarArchivoClientes(argv[PATH_CLI]);
	generarArchivoPagos(argv[PATH_PAGOS]);

	mostrarArchivoClientes(argv[PATH_CLI]);

	char nombreIdx[201];
	reemplazarSubcad(argv[PATH_CLI], ".dat", ".idx", nombreIdx, 201);
	mostrarArchivoClientesIdx(nombreIdx);

	mostrarArchivoPagos(argv[PATH_PAGOS]);

	#ifdef SOLUCION_DOC
		procesarPagos(argv[PATH_CLI], argv[PATH_PAGOS], argv[PATH_LISTADO]);
	#else
		procesarPagos_ALU(argv[PATH_CLI], argv[PATH_PAGOS], argv[PATH_LISTADO]);
	#endif

	return 0;
}

void procesarPagos_ALU(const char* pathCli, const char* pathPagos, const char* pathListado)
{
    FILE *pfc, *pfp, *pfl;
    Vector vecDeudores;
    Vector vecIdx;
    Cliente auxCliente;
    IndCli auxIdx;
    FacturasImpagasFueraTerm auxDeudor;
    Pago auxPago;
    Fecha fechaImpaga = {1, 1, 1900};
    char nombreIdx[201];
    int posIDX = 0;

    pfc = fopen(pathCli, "r+b");
    if(!pfc){
        printf("Error al abrir %s", pathCli);
        return;
    }
    pfp = fopen(pathPagos, "rb");
    if(!pfp){
        printf("Error al abrir %s", pathPagos);
        fclose(pfc);
        return;
    }
    pfl = fopen(pathListado, "wt");
    if(!pfl){
        printf("Error al abrir %s", pathListado);
        fclose(pfc);
        fclose(pfp);
        return;
    }
    //Cargamos el indice en un vector de mem dinamica
    reemplazarSubcad(pathCli, ".dat", ".idx", nombreIdx, 201);
    //vectorCrearDeArchivo(&vecIdx, sizeof(IndCli), nombreIdx);
    vectorCrear(&vecIdx, sizeof(IndCli));
    cargarVectorDeArchivo_ALU(&vecIdx, nombreIdx, &auxIdx);
    //mapVector_ALU(&vecIdx, printIdx_ALU);

    //Creamos un vecDeudores, donde vamos a cargar desde pathPagos solo los deudores
    vectorCrear(&vecDeudores, sizeof(FacturasImpagasFueraTerm));
     while(fread(&auxPago, sizeof(Pago), 1, pfp))
    {
        // ***** LÓGICA CLAVE CORREGIDA *****
        // Se considera deudor si:
        // 1. La factura está impaga (fechaPago == 1/1/1900)
        // O
        // 2. La factura se pagó fuera de término (fechaVencimiento < fechaPago)
        if ( (cmpFechas_ALU(&auxPago.fechaPago, &fechaImpaga) == 0) || (cmpFechas_ALU(&auxPago.fechaVencimiento, &auxPago.fechaPago) < 0) )
        {
            auxDeudor.nroCliente = auxPago.nroCliente;
            auxDeudor.cantFactImpagasFueraTerm = 1;
            vectorOrdInsertar_ALU(&vecDeudores, &auxDeudor, cmpDeudores_ALU, actualizarDeudor_ALU);
        }
    }
    ordenarVectorInsercion(&vecDeudores, cmpCantFacturas_ALU);
    mapVector_ALU(&vecDeudores, printDeudores_ALU);

    //grabamos el listado de deudores
    fprintf(pfl, "Listado de Deudores\n");
    printf("Listado de Deudores\n");
    fprintf(pfl,"===================================\n");
    printf("===================================\n");
    fprintf(pfl,"Nro Cliente     Nombre            Facturas en mora\n");
    printf("Nro Cliente     Nombre            Facturas en mora\n");

    for(int i = 0; i < vecDeudores.ce; i++){
        FacturasImpagasFueraTerm deudor;
        vectorObtenerElemento(&vecDeudores, i, &deudor);
        posIDX = Bsearch_ALU(&vecIdx, cmpNroClientes_ALU, &deudor);
        if(posIDX != -1){
            IndCli indice;
            vectorObtenerElemento(&vecIdx, posIDX, &indice);
            fseek(pfc, (long)indice.nroReg * sizeof(Cliente), SEEK_SET);
            fread(&auxCliente, sizeof(Cliente), 1, pfc);
        }
        fprintf(pfl, "%-15d\t%-20s\t%10d\n", deudor.nroCliente, auxCliente.nombre, deudor.cantFactImpagasFueraTerm);
        printf("%-15d\t%-20s\t%10d\n", deudor.nroCliente, auxCliente.nombre, deudor.cantFactImpagasFueraTerm);
    }

    fclose(pfc);
    fclose(pfp);
    fclose(pfl);
    vectorDestruir(&vecDeudores);
    vectorDestruir(&vecIdx);
}

void printIdx_ALU(void* elem, void* extra){
    IndCli *index = ((IndCli*)elem);
    printf("Nro reg:%d\tNro cli:%d\n", index->nroReg, index->nroCliente);
}

void printDeudores_ALU(void* elem, void* extra){
    FacturasImpagasFueraTerm *index = ((FacturasImpagasFueraTerm*)elem);
    printf("Nro cli:%d\tCant Fact:%d\n", index->nroCliente, index->cantFactImpagasFueraTerm);
}

void mapVector_ALU(Vector *vec, Accion accion){
    if(!vec->vec)
        return;
    for(int i=0; i < vec->ce; i++)
        accion(vec->vec + (i * vec->tamElem), NULL);
}

void vectorObtenerElemento(Vector* vec, int pos, void* elem){
    if (pos >= vec->ce) return;
    memcpy(elem, vec->vec + (pos * vec->tamElem), vec->tamElem);
}

int vectorOrdInsertar_ALU(Vector* vec, const void* elem, Cmp cmp, Actualizar actualizar)
{
    size_t pos = 0;
    char* data = (char*)vec->vec;

    while (pos < vec->ce && cmp(elem, data + pos * vec->tamElem) > 0)
        pos++;

    if (pos < vec->ce && cmp(elem, data + pos * vec->tamElem) == 0)
    {
        if (actualizar)
            actualizar(data + pos * vec->tamElem, elem);
        return 1;
    }

    if (vec->ce == vec->cap)
    {
        size_t nuevaCap = vec->cap == 0 ? 1 : vec->cap * 2;
        void* aux = realloc(vec->vec, nuevaCap * vec->tamElem);
        if (!aux)
            return 0; //alloc error
        vec->vec = aux;
        vec->cap = nuevaCap;
        data = (char*)vec->vec;
    }

    memmove(data + (pos + 1) * vec->tamElem,  // Destination: one spot to the right
            data + pos * vec->tamElem,        // Source: the current position
            (vec->ce - pos) * vec->tamElem);  // Number of bytes to move

    memcpy(data + pos * vec->tamElem, elem, vec->tamElem);
    vec->ce++;
    return 1;
}


int Bsearch_ALU(Vector *vec, int (*cmp)(const void *, const void *), const void *dato)
{
    int ini, med, fin, res;
    ini = 0;
    fin = vec->ce - 1;
    while(ini <= fin){
        med = (fin - ini) / 2 + ini;
        res = cmp(dato, vec->vec + (med * vec->tamElem));
        if( res == 0 )
            return med;
        if( res < 0 )
            fin = med - 1;
        if( res > 0 )
            ini = med + 1;
    }
    return -1;
}

int ordenarVectorInsercion(Vector* vec, Cmp cmp) {
    if (!vec || !vec->vec || vec->ce < 2)
        return 0;
    int i;
    char* data = (char*)vec->vec;
    size_t tam = vec->tamElem;
    void* temp = malloc(tam);
    if (!temp)
        return 0;

    for (i = 1; i < vec->ce; i++) {
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

int cargarVectorDeArchivo_ALU(Vector *vec, const char *nombreArchivo, void *elem)
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

int insertarEnVectorFinal(Vector *vec, void *elem)
{
    if (!vec->vec)
        return 0;
    if (vec->ce >= vec->cap)
    {
        void *aux = realloc(vec->vec, vec->cap * 2 * vec->tamElem);
        if (!aux)
            return 0;
        vec->vec = aux;
        vec->cap *= 2;
    }
    memcpy((char *)vec->vec + vec->ce * vec->tamElem, elem, vec->tamElem);
    vec->ce++;
    return 1;
}


int cmpDeudores_ALU(const void *d1, const void *d2){
    FacturasImpagasFueraTerm *deudor1 = ((FacturasImpagasFueraTerm*)d1);
    FacturasImpagasFueraTerm *deudor2 = ((FacturasImpagasFueraTerm*)d2);
    return deudor1->nroCliente - deudor2->nroCliente;
}

int cmpCantFacturas_ALU(const void *d1, const void *d2){
    const FacturasImpagasFueraTerm *deudor1 = d1;
    const FacturasImpagasFueraTerm *deudor2 = d2;
    if (deudor1->cantFactImpagasFueraTerm != deudor2->cantFactImpagasFueraTerm) {
        return deudor2->cantFactImpagasFueraTerm - deudor1->cantFactImpagasFueraTerm;
    }
    return deudor1->nroCliente - deudor2->nroCliente;
}

int cmpFechas_ALU(const void *f1, const void *f2){
    Fecha *fecha1 = ((Fecha*)f1);
    Fecha *fecha2 = ((Fecha*)f2);
    int diff = fecha1->anio - fecha2->anio;
    if (diff != 0) return diff;
    diff = fecha1->mes - fecha2->mes;
    if (diff != 0) return diff;
    return fecha1->dia - fecha2->dia;
}

//en el cmp tenemos primero deudor, que es de tipo FacturasImpagas, luego un registro de tipo IndCli
int cmpNroClientes_ALU(const void *d1, const void *d2){
    FacturasImpagasFueraTerm *deudor = ((FacturasImpagasFueraTerm*)d1);
    IndCli *registro = ((IndCli*)d2);
    return deudor->nroCliente - registro->nroCliente;
}

void actualizarDeudor_ALU(void* actualizado, const void* actualizador){
    FacturasImpagasFueraTerm *deudor = ((FacturasImpagasFueraTerm*)actualizado);
    deudor->cantFactImpagasFueraTerm += 1;
}



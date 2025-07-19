#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct {
    char codigo_prod[6]; // Ej: "PROD1"
    char descripcion[50];
    int cantidad;
} s_stock_item;

// Función merge genérica
/**
 * @brief Realiza un merge genérico entre dos archivos ordenados
 * 
 * @param archivo1 Path al primer archivo (archivo maestro)
 * @param archivo2 Path al segundo archivo (archivo de actualizaciones)  
 * @param archivo_salida Path al archivo de salida resultante
 * @param tam_elemento Tamaño en bytes de cada elemento
 * @param leer_archivo1 Función para leer elementos del archivo1 (retorna 1 si lee correctamente, 0 si EOF/error)
 * @param leer_archivo2 Función para leer elementos del archivo2 (retorna 1 si lee correctamente, 0 si EOF/error)
 * @param escribir_salida Función para escribir elementos al archivo de salida
 * @param comparar Función de comparación entre elementos (-1: elem1<elem2, 0: elem1==elem2, 1: elem1>elem2)
 * @param actualizar Función de actualización cuando los elementos son iguales (puede ser NULL)
 * @param procesar_no_encontrado Función para procesar elementos del archivo2 que no están en archivo1 (puede ser NULL)
 * @return Número de elementos procesados exitosamente, -1 si hay error
 */
int merge_generico(
    const char* archivo1,
    const char* archivo2, 
    const char* archivo_salida,
    size_t tam_elemento,
    int (*leer_archivo1)(void* elemento, FILE* archivo),
    int (*leer_archivo2)(void* elemento, FILE* archivo),
    int (*escribir_salida)(const void* elemento, FILE* archivo),
    int (*comparar)(const void* elem1, const void* elem2),
    void (*actualizar)(void* elem1, const void* elem2),
    void (*procesar_no_encontrado)(const void* elem2, FILE* archivo_errores)
);

int actualizar_stock(const char* ruta_stock, const char* ruta_operaciones, void (*actualizar)(s_stock_item*, char, int));
int actualizar_stock_v2(const char* ruta_stock, const char* ruta_operaciones, const char* ruta_salida);
int readOperacion(s_stock_item* op, FILE* arch);

// Funciones callback para diferentes tipos de merge
int leer_stock_binario(void* elemento, FILE* archivo);
int leer_operacion_texto(void* elemento, FILE* archivo);
int escribir_stock_binario(const void* elemento, FILE* archivo);
int comparar_por_codigo(const void* elem1, const void* elem2);
void actualizar_stock_con_operacion(void* stock, const void* operacion);
void manejar_producto_no_encontrado(const void* operacion, FILE* archivo_errores);
void actualizarStock(s_stock_item* item, char tipo_operacion, int cantidad);
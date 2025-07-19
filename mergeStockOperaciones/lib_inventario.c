#include "lib_inventario.h"

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
) {
    FILE *pf1, *pf2, *pf_salida, *pf_errores;
    void *elem1, *elem2;
    int hay_elem1, hay_elem2, resultado_cmp;
    int elementos_procesados = 0;
    
    // Validación de parámetros esenciales
    if (!archivo1 || !archivo2 || !archivo_salida || !leer_archivo1 || 
        !leer_archivo2 || !escribir_salida || !comparar || tam_elemento == 0) {
        return -1;
    }
    
    // Asignar memoria para los elementos
    elem1 = malloc(tam_elemento);
    elem2 = malloc(tam_elemento);
    if (!elem1 || !elem2) {
        free(elem1);
        free(elem2);
        return -1;
    }
    
    // Abrir archivos
    pf1 = fopen(archivo1, "rb");
    if (!pf1) {
        printf("Error al abrir archivo: %s\n", archivo1);
        free(elem1);
        free(elem2);
        return -1;
    }
    
    pf2 = fopen(archivo2, "rt");  // Asumiendo archivo de texto para el segundo
    if (!pf2) {
        printf("Error al abrir archivo: %s\n", archivo2);
        fclose(pf1);
        free(elem1);
        free(elem2);
        return -1;
    }
    
    pf_salida = fopen(archivo_salida, "wb");
    if (!pf_salida) {
        printf("Error al abrir archivo de salida: %s\n", archivo_salida);
        fclose(pf1);
        fclose(pf2);
        free(elem1);
        free(elem2);
        return -1;
    }
    
    // Archivo de errores opcional
    pf_errores = NULL;
    if (procesar_no_encontrado) {
        pf_errores = fopen("errores_merge.txt", "w");
        if (!pf_errores) {
            printf("Advertencia: No se pudo crear archivo de errores\n");
        }
    }
    
    // Leer primer elemento de cada archivo
    hay_elem1 = leer_archivo1(elem1, pf1);
    hay_elem2 = leer_archivo2(elem2, pf2);
    
    // Algoritmo de merge principal
    while (hay_elem1 || hay_elem2) {
        if (!hay_elem1) {
            // Solo quedan elementos en archivo2 - manejar como no encontrados
            if (procesar_no_encontrado && pf_errores) {
                procesar_no_encontrado(elem2, pf_errores);
            }
            hay_elem2 = leer_archivo2(elem2, pf2);
        }
        else if (!hay_elem2) {
            // Solo quedan elementos en archivo1 - copiar sin modificar
            if (escribir_salida(elem1, pf_salida)) {
                elementos_procesados++;
            }
            hay_elem1 = leer_archivo1(elem1, pf1);
        }
        else {
            // Comparar elementos de ambos archivos
            resultado_cmp = comparar(elem1, elem2);
            
            if (resultado_cmp == 0) {
                // Elementos iguales - aplicar actualización si existe
                if (actualizar) {
                    actualizar(elem1, elem2);
                }
                elementos_procesados++;
                hay_elem2 = leer_archivo2(elem2, pf2);
                
                // Escribir solo si no hay más elementos iguales
                if (!hay_elem2 || comparar(elem1, elem2) != 0) {
                    escribir_salida(elem1, pf_salida);
                    hay_elem1 = leer_archivo1(elem1, pf1);
                }
            }
            else if (resultado_cmp < 0) {
                // elem1 < elem2 - escribir elem1 y avanzar archivo1
                if (escribir_salida(elem1, pf_salida)) {
                    elementos_procesados++;
                }
                hay_elem1 = leer_archivo1(elem1, pf1);
            }
            else {
                // elem1 > elem2 - elemento de archivo2 no encontrado en archivo1
                if (procesar_no_encontrado && pf_errores) {
                    procesar_no_encontrado(elem2, pf_errores);
                }
                hay_elem2 = leer_archivo2(elem2, pf2);
            }
        }
    }
    
    // Cerrar archivos y liberar memoria
    fclose(pf1);
    fclose(pf2);
    fclose(pf_salida);
    if (pf_errores) {
        fclose(pf_errores);
    }
    free(elem1);
    free(elem2);
    
    return elementos_procesados;
}

// ===== FUNCIONES CALLBACK ESPECÍFICAS PARA STOCK =====

/**
 * @brief Lee un elemento de stock desde archivo binario
 */
int leer_stock_binario(void* elemento, FILE* archivo) {
    return fread(elemento, sizeof(s_stock_item), 1, archivo) == 1;
}

/**
 * @brief Lee una operación desde archivo de texto
 */
int leer_operacion_texto(void* elemento, FILE* archivo) {
    return readOperacion((s_stock_item*)elemento, archivo);
}

/**
 * @brief Escribe un elemento de stock a archivo binario
 */
int escribir_stock_binario(const void* elemento, FILE* archivo) {
    return fwrite(elemento, sizeof(s_stock_item), 1, archivo) == 1;
}

/**
 * @brief Compara dos elementos de stock por código de producto
 */
int comparar_por_codigo(const void* elem1, const void* elem2) {
    const s_stock_item* stock1 = (const s_stock_item*)elem1;
    const s_stock_item* stock2 = (const s_stock_item*)elem2;
    return strcmp(stock1->codigo_prod, stock2->codigo_prod);
}

/**
 * @brief Actualiza stock con operación
 */
void actualizar_stock_con_operacion(void* stock, const void* operacion) {
    s_stock_item* item = (s_stock_item*)stock;
    const s_stock_item* op = (const s_stock_item*)operacion;
    actualizarStock(item, op->descripcion[0], op->cantidad);
}

/**
 * @brief Maneja productos no encontrados en stock
 */
void manejar_producto_no_encontrado(const void* operacion, FILE* archivo_errores) {
    const s_stock_item* op = (const s_stock_item*)operacion;
    if (archivo_errores) {
        fprintf(archivo_errores, "Producto %s no encontrado en stock.dat\n", op->codigo_prod);
    }
}

/**
 * @brief Versión refactorizada usando merge genérico
 */
int actualizar_stock_v2(const char* ruta_stock, const char* ruta_operaciones, const char* ruta_salida) {
    return merge_generico(
        ruta_stock,
        ruta_operaciones,
        ruta_salida ? ruta_salida : "../archivos/nuevo_stock.dat",
        sizeof(s_stock_item),
        leer_stock_binario,
        leer_operacion_texto,
        escribir_stock_binario,
        comparar_por_codigo,
        actualizar_stock_con_operacion,
        manejar_producto_no_encontrado
    );
}

// ===== EJEMPLO DE USO CON DIFERENTES TIPOS DE DATOS =====

// Estructura genérica para demostrar flexibilidad
typedef struct {
    int id;
    char nombre[30];
    double valor;
} s_ejemplo;

/**
 * @brief Ejemplo de función de lectura para archivos binarios genéricos
 */
int leer_ejemplo_binario(void* elemento, FILE* archivo) {
    return fread(elemento, sizeof(s_ejemplo), 1, archivo) == 1;
}

/**
 * @brief Ejemplo de función de lectura para archivos de texto con formato CSV
 */
int leer_ejemplo_csv(void* elemento, FILE* archivo) {
    s_ejemplo* item = (s_ejemplo*)elemento;
    char buffer[128];
    
    if (fgets(buffer, sizeof(buffer), archivo) == NULL) {
        return 0;
    }
    
    return sscanf(buffer, "%d,%29[^,],%lf", &item->id, item->nombre, &item->valor) == 3;
}

/**
 * @brief Ejemplo de función de escritura para archivos binarios
 */
int escribir_ejemplo_binario(const void* elemento, FILE* archivo) {
    return fwrite(elemento, sizeof(s_ejemplo), 1, archivo) == 1;
}

/**
 * @brief Ejemplo de comparación por ID
 */
int comparar_por_id(const void* elem1, const void* elem2) {
    const s_ejemplo* e1 = (const s_ejemplo*)elem1;
    const s_ejemplo* e2 = (const s_ejemplo*)elem2;
    return (e1->id < e2->id) ? -1 : (e1->id > e2->id) ? 1 : 0;
}

/**
 * @brief Ejemplo de actualización que suma valores
 */
void actualizar_sumando_valores(void* elem1, const void* elem2) {
    s_ejemplo* e1 = (s_ejemplo*)elem1;
    const s_ejemplo* e2 = (const s_ejemplo*)elem2;
    e1->valor += e2->valor;
}

/**
 * @brief Ejemplo de merge para diferentes tipos de archivos
 * Demuestra la versatilidad de la función merge_generico
 */
int ejemplo_merge_flexible(const char* archivo_bin, const char* archivo_csv, const char* salida) {
    return merge_generico(
        archivo_bin,
        archivo_csv,
        salida,
        sizeof(s_ejemplo),
        leer_ejemplo_binario,
        leer_ejemplo_csv,
        escribir_ejemplo_binario,
        comparar_por_id,
        actualizar_sumando_valores,
        NULL  // Sin manejo especial de no encontrados
    );
}



/**
 * @brief Updates a stock file based on an operations file.
 *
 * This function merges operations from a text file into a binary stock file.
 * It reads both files, which are assumed to be sorted by product code.
 * It applies the operations to the corresponding stock items and writes the
 * updated stock to a new file. Products from the operations file that are not
 * found in the stock file are logged as errors.
 *
 * After processing, the original stock file is replaced with the newly
 * generated, updated stock file.
 *
 * @param ruta_stock Path to the binary stock file (e.g., "stock.dat").
 * @param ruta_operaciones Path to the text file with operations (e.g., "operaciones.txt").
 * @param actualizar A function pointer to the callback function that performs the
 *                   actual update on a stock item. This function receives the
 *                   stock item, the operation type, and the quantity.
 * @return The number of stock records that were successfully updated. Returns -1
 *         if there is an error opening any of the required files.
 */
int actualizar_stock(const char* ruta_stock, const char* ruta_operaciones, void (*actualizar)(s_stock_item*, char, int)){
    FILE *pfs, *pfo, *pfe, *pfn;
    s_stock_item aux1, aux2;
    int res = 0, registros_actualizados = 0;


    pfo = fopen(ruta_operaciones, "rt");
    if(!pfo){
        printf("Error al abrir el archivo: %s", ruta_operaciones);
        return -1;
    }

    pfs = fopen(ruta_stock, "r+b");
    if(!pfs){
        printf("Error al abrir el archivo: %s", ruta_stock);
        fclose(pfo);
        return -1;
    }
    pfe = fopen("../archivos/errores.txt", "w");
    if(!pfe){
        printf("Error al abrir el archivo: %s", "archivos/errores.txt");
        fclose(pfs);
        fclose(pfo);
        return -1;
    }
    pfn = fopen("../archivos/nuevo_stock.dat", "wb");
    if(!pfn){
        printf("Error al abrir el archivo: %s", "archivos/nuevo_stock.dat");
        fclose(pfs);
        fclose(pfo);
        fclose(pfe);
        return -1;
    }

    fread(&aux1, sizeof(s_stock_item), 1, pfs);
    int hay_stock = !feof(pfs);
    int hay_operacion = readOperacion(&aux2, pfo);
    
    while( hay_stock || hay_operacion ){
        if( !hay_stock ){
            // Solo quedan operaciones - reportar error
            fprintf(pfe, "Producto %s no encontrado en stock.dat\n", aux2.codigo_prod);
            hay_operacion = readOperacion(&aux2, pfo);
        }
        else if( !hay_operacion ){
            // Solo queda stock - copiar sin modificar
            fwrite(&aux1, sizeof(s_stock_item), 1, pfn);
            hay_stock = (fread(&aux1, sizeof(s_stock_item), 1, pfs) == 1);
        }
        else {
            res = strcmp(aux1.codigo_prod, aux2.codigo_prod);
            if( res == 0 ){
                actualizar(&aux1, aux2.descripcion[0], aux2.cantidad);
                registros_actualizados++;
                hay_operacion = readOperacion(&aux2, pfo);
                // Si no hay más operaciones o la siguiente es de otro producto, escribir y avanzar
                if( !hay_operacion || strcmp(aux1.codigo_prod, aux2.codigo_prod) != 0 ){
                    fwrite(&aux1, sizeof(s_stock_item), 1, pfn);
                    hay_stock = (fread(&aux1, sizeof(s_stock_item), 1, pfs) == 1);
                }
            }
            else if( res < 0 ){
                fwrite(&aux1, sizeof(s_stock_item), 1, pfn);
                hay_stock = (fread(&aux1, sizeof(s_stock_item), 1, pfs) == 1);
            }
            else {
                fprintf(pfe, "Producto %s no encontrado en stock.dat\n", aux2.codigo_prod);
                hay_operacion = readOperacion(&aux2, pfo);
            }
        }
    }

    fclose(pfs);
    fclose(pfo);
    fclose(pfe);
    fclose(pfn);
    remove(ruta_stock);
    rename("../archivos/nuevo_stock.dat", ruta_stock);

    return registros_actualizados;
}

int readOperacion(s_stock_item* op, FILE* arch){
    char buffer[128];
    char tipo;
    if(!arch)
        return 0;
    if( fgets(buffer, sizeof(buffer), arch) == NULL )
        return 0;
    sscanf(buffer, "%[^;];%c;%d", op->codigo_prod, &tipo, &op->cantidad);
    op->descripcion[0] = tipo;
    op->descripcion[1] = '\0';
    return 1;
}


void actualizarStock(s_stock_item* item, char tipo_operacion, int cantidad){
    if( !item )
        return;
    switch(tipo_operacion){
        case 'E':
            item->cantidad += cantidad;
            break;
        case 'S':
            item->cantidad -= cantidad;
            break;
        default:
            printf("Operacion desconocida: %c\n", tipo_operacion);
            return;
    }
    return;
}

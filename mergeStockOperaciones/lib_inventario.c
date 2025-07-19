#include "lib_inventario.h"

int merge_generico( const char* archivo1, const char* archivo2, const char* archivo_salida, size_t tam_elemento,
    int (*leer_archivo1)(void* elemento, FILE* archivo), int (*leer_archivo2)(void* elemento, FILE* archivo),
    int (*escribir_salida)(const void* elemento, FILE* archivo), int (*comparar)(const void* elem1, const void* elem2),
    void (*actualizar)(void* elem1, const void* elem2), void (*procesar_no_encontrado)(const void* elem2, FILE* archivo_errores) ){
    
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
 * @brief Realiza un apareo (merge) genérico de un archivo maestro con un archivo de novedades.
 *
 * @param config        Puntero a la estructura de configuración que define la lógica del merge.
 * @param pathMaestro   Ruta al archivo maestro (binario).
 * @param pathNovedades Ruta al archivo de novedades (puede ser texto o binario).
 * @param pathErrores   Ruta al archivo de log para novedades sin correspondencia (opcional, puede ser NULL).
 * @return              La cantidad de registros maestros escritos/actualizados, o -1 en caso de error.
 */
int mergeArchivos(const MergeConfig* config, const char* pathMaestro, const char* pathNovedades, const char* pathErrores) {
    // --- Validación y Setup ---
    if (!config || !pathMaestro || !pathNovedades) return -1;

    FILE *pfMaestro = fopen(pathMaestro, "rb");
    if (!pfMaestro) { /* ... manejo de error ... */ return -1; }

    // El archivo de novedades puede ser texto o binario, se usa "r" y la función leer se encarga.
    FILE *pfNovedades = fopen(pathNovedades, "r");
    if (!pfNovedades) { /* ... manejo de error ... */ fclose(pfMaestro); return -1; }

    // El archivo de salida temporal
    FILE *pfSalida = fopen("temp_merge.tmp", "wb");
    if (!pfSalida) { /* ... manejo de error ... */ fclose(pfMaestro); fclose(pfNovedades); return -1; }

    FILE *pfErrores = NULL;
    if (pathErrores && config->procesarNovedadSinMaestro) {
        pfErrores = fopen(pathErrores, "w"); // Si falla, podemos continuar sin loguear.
    }

    void* elemMaestro = malloc(config->tamElemento);
    void* elemNovedad = malloc(config->tamElemento);
    // ... validar que malloc no falló ...

    int leidoMaestro = config->leerMaestro(elemMaestro, pfMaestro);
    int leidoNovedad = config->leerNovedad(elemNovedad, pfNovedades);
    int count = 0;

    // --- Bucle Principal de Apareo ---
    while (leidoMaestro && leidoNovedad) {
        int cmp = config->comparar(elemMaestro, elemNovedad);

        if (cmp < 0) { // El maestro es menor, no tiene novedades. Se graba como está.
            config->escribirMaestro(elemMaestro, pfSalida);
            leidoMaestro = config->leerMaestro(elemMaestro, pfMaestro);
        } else if (cmp > 0) { // La novedad es menor, no tiene un maestro. Se informa el error.
            if (pfErrores) config->procesarNovedadSinMaestro(elemNovedad, pfErrores);
            leidoNovedad = config->leerNovedad(elemNovedad, pfNovedades);
        } else { // Las claves son iguales. Bucle de actualización.
            // **AQUÍ ESTÁ LA LÓGICA CORREGIDA**
            // Mientras la clave de la novedad siga siendo la misma, se actualiza el maestro.
            while (leidoNovedad && config->comparar(elemMaestro, elemNovedad) == 0) {
                if (config->actualizar) {
                    config->actualizar(elemMaestro, elemNovedad);
                }
                leidoNovedad = config->leerNovedad(elemNovedad, pfNovedades);
            }
            // Cuando la clave cambia (o se acaban las novedades), se escribe el maestro actualizado.
            config->escribirMaestro(elemMaestro, pfSalida);
            leidoMaestro = config->leerMaestro(elemMaestro, pfMaestro);
        }
    }

    // --- Drenaje de archivos remanentes ---
    // Si quedan maestros, se copian tal cual.
    while (leidoMaestro) {
        config->escribirMaestro(elemMaestro, pfSalida);
        leidoMaestro = config->leerMaestro(elemMaestro, pfMaestro);
    }
    // Si quedan novedades, se informan como error.
    while (leidoNovedad) {
        if (pfErrores) config->procesarNovedadSinMaestro(elemNovedad, pfErrores);
        leidoNovedad = config->leerNovedad(elemNovedad, pfNovedades);
    }

    // --- Limpieza y finalización ---
    free(elemMaestro);
    free(elemNovedad);
    fclose(pfMaestro);
    fclose(pfNovedades);
    fclose(pfSalida);
    if(pfErrores) fclose(pfErrores);

    // Reemplazar el archivo original con el temporal
    remove(pathMaestro);
    rename("temp_merge.tmp", pathMaestro);

    return count;
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

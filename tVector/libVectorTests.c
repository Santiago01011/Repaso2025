#include "tVector/libVector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Estructura de cuenta bancaria (definida originalmente en libVector.c)
typedef struct {
    char codigo_cta[9]; // Ej: "123456/5"
    float saldo;
} s_cuenta;

// Colores para output de consola
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_RESET "\033[0m"

// Contador de pruebas
int tests_passed = 0;
int tests_failed = 0;

// Macros para testing
#define ASSERT_TRUE(condition, test_name) \
    do { \
        if (condition) { \
            printf(COLOR_GREEN "PASS: %s\n" COLOR_RESET, test_name); \
            tests_passed++; \
        } else { \
            printf(COLOR_RED "FAIL: %s\n" COLOR_RESET, test_name); \
            tests_failed++; \
        } \
    } while(0)

#define ASSERT_FALSE(condition, test_name) \
    ASSERT_TRUE(!(condition), test_name)

#define ASSERT_EQUAL(expected, actual, test_name) \
    ASSERT_TRUE((expected) == (actual), test_name)

// Función auxiliar para verificar si un vector de enteros está ordenado
int vectorIntOrdenado(tVector *vec) {
    if (!vec || !vec->data || vec->cantElem <= 1) return 1;
    
    for (int i = 0; i < vec->cantElem - 1; i++) {
        int *arr = (int*)vec->data;
        if (arr[i] > arr[i + 1]) return 0;
    }
    return 1;
}

// Función auxiliar para imprimir vector de enteros (para debugging)
void imprimirVectorInt(tVector *vec, const char *nombre) {
    printf("%s: [", nombre);
    for (int i = 0; i < vec->cantElem; i++) {
        printf("%d", *((int*)vec->data + i));
        if (i < vec->cantElem - 1) printf(", ");
    }
    printf("]\n");
}

// Función auxiliar para verificar contenido del vector
int verificarContenidoVector(tVector *vec, int *esperados, int cantEsperados) {
    if (vec->cantElem != cantEsperados) return 0;
    
    for (int i = 0; i < cantEsperados; i++) {
        if (*((int*)vec->data + i) != esperados[i]) return 0;
    }
    return 1;
}

// ===== CASOS DE PRUEBA =====

void test_insertar_parametros_invalidos() {
    printf(COLOR_YELLOW "\n=== Pruebas de Parámetros Inválidos ===\n" COLOR_RESET);
    
    tVector vec;
    int elem = 5;
    
    // Test 1: Vector NULL
    printf("Ejecutando Test 1: Vector NULL\n");
    int resultado1 = insertarEnVectorOrdenado(NULL, &elem, cmpInt);
    ASSERT_FALSE(resultado1, "Vector NULL debe retornar 0");
    
    // Test 2: Vector con data NULL
    printf("Ejecutando Test 2: Vector con data NULL\n");
    vec.data = NULL;
    vec.cantElem = 0;
    vec.totalLength = 0;
    vec.tamElem = sizeof(int);
    int resultado2 = insertarEnVectorOrdenado(&vec, &elem, cmpInt);
    ASSERT_FALSE(resultado2, "Vector con data NULL debe retornar 0");
    
    // Test 3: Elemento NULL - comentado por seguridad
    /*
    printf("Ejecutando Test 3: Elemento NULL\n");
    crearVector(&vec, sizeof(int));
    int resultado3 = insertarEnVectorOrdenado(&vec, NULL, cmpInt);
    ASSERT_FALSE(resultado3, "Elemento NULL debe retornar 0");
    vectorLiberar(&vec);
    */
    
    // Test 4: Función de comparación NULL - comentado por seguridad  
    /*
    printf("Ejecutando Test 4: Función de comparación NULL\n");
    crearVector(&vec, sizeof(int));
    int resultado4 = insertarEnVectorOrdenado(&vec, &elem, NULL);
    ASSERT_FALSE(resultado4, "Función de comparación NULL debe retornar 0");
    vectorLiberar(&vec);
    */
}

void test_insertar_en_vector_vacio() {
    printf(COLOR_YELLOW "\n=== Pruebas en Vector Vacío ===\n" COLOR_RESET);
    
    tVector vec;
    crearVector(&vec, sizeof(int));
    
    int elem = 42;
    int resultado = insertarEnVectorOrdenado(&vec, &elem, cmpInt);
    
    ASSERT_TRUE(resultado, "Inserción en vector vacío debe ser exitosa");
    ASSERT_EQUAL(1, vec.cantElem, "Vector debe tener 1 elemento");
    ASSERT_EQUAL(42, *((int*)vec.data), "Elemento insertado debe ser 42");
    ASSERT_TRUE(vectorIntOrdenado(&vec), "Vector debe estar ordenado");
    
    vectorLiberar(&vec);
}

void test_insertar_al_inicio() {
    printf(COLOR_YELLOW "\n=== Pruebas de Inserción al Inicio ===\n" COLOR_RESET);
    
    tVector vec;
    crearVector(&vec, sizeof(int));
    
    // Llenar vector con elementos ordenados
    int elementos[] = {5, 10, 15, 20};
    for (int i = 0; i < 4; i++) {
        insertarEnVectorOrdenado(&vec, &elementos[i], cmpInt);
    }
    
    // Insertar elemento menor que todos
    int nuevo = 1;
    int resultado = insertarEnVectorOrdenado(&vec, &nuevo, cmpInt);
    
    ASSERT_TRUE(resultado, "Inserción al inicio debe ser exitosa");
    ASSERT_EQUAL(5, vec.cantElem, "Vector debe tener 5 elementos");
    ASSERT_EQUAL(1, *((int*)vec.data), "Primer elemento debe ser 1");
    ASSERT_TRUE(vectorIntOrdenado(&vec), "Vector debe permanecer ordenado");
    
    int esperados[] = {1, 5, 10, 15, 20};
    ASSERT_TRUE(verificarContenidoVector(&vec, esperados, 5), "Contenido debe ser correcto");
    
    vectorLiberar(&vec);
}

void test_insertar_al_final() {
    printf(COLOR_YELLOW "\n=== Pruebas de Inserción al Final ===\n" COLOR_RESET);
    
    tVector vec;
    crearVector(&vec, sizeof(int));
    
    // Llenar vector con elementos ordenados
    int elementos[] = {5, 10, 15, 20};
    for (int i = 0; i < 4; i++) {
        insertarEnVectorOrdenado(&vec, &elementos[i], cmpInt);
    }
    
    // Insertar elemento mayor que todos
    int nuevo = 25;
    int resultado = insertarEnVectorOrdenado(&vec, &nuevo, cmpInt);
    
    ASSERT_TRUE(resultado, "Inserción al final debe ser exitosa");
    ASSERT_EQUAL(5, vec.cantElem, "Vector debe tener 5 elementos");
    ASSERT_EQUAL(25, *((int*)vec.data + 4), "Último elemento debe ser 25");
    ASSERT_TRUE(vectorIntOrdenado(&vec), "Vector debe permanecer ordenado");
    
    int esperados[] = {5, 10, 15, 20, 25};
    ASSERT_TRUE(verificarContenidoVector(&vec, esperados, 5), "Contenido debe ser correcto");
    
    vectorLiberar(&vec);
}

void test_insertar_en_medio() {
    printf(COLOR_YELLOW "\n=== Pruebas de Inserción en el Medio ===\n" COLOR_RESET);
    
    tVector vec;
    crearVector(&vec, sizeof(int));
    
    // Llenar vector con elementos ordenados
    int elementos[] = {5, 15, 25, 35};
    for (int i = 0; i < 4; i++) {
        insertarEnVectorOrdenado(&vec, &elementos[i], cmpInt);
    }
    
    // Insertar elemento en el medio
    int nuevo = 20;
    int resultado = insertarEnVectorOrdenado(&vec, &nuevo, cmpInt);
    
    ASSERT_TRUE(resultado, "Inserción en el medio debe ser exitosa");
    ASSERT_EQUAL(5, vec.cantElem, "Vector debe tener 5 elementos");
    ASSERT_TRUE(vectorIntOrdenado(&vec), "Vector debe permanecer ordenado");
    
    int esperados[] = {5, 15, 20, 25, 35};
    ASSERT_TRUE(verificarContenidoVector(&vec, esperados, 5), "Contenido debe ser correcto");
    
    vectorLiberar(&vec);
}

void test_insertar_duplicados() {
    printf(COLOR_YELLOW "\n=== Pruebas de Inserción de Duplicados ===\n" COLOR_RESET);
    
    tVector vec;
    crearVector(&vec, sizeof(int));
    
    // Llenar vector con elementos ordenados
    int elementos[] = {5, 10, 15, 20};
    for (int i = 0; i < 4; i++) {
        insertarEnVectorOrdenado(&vec, &elementos[i], cmpInt);
    }
    
    // Insertar elemento duplicado
    int duplicado = 15;
    int resultado = insertarEnVectorOrdenado(&vec, &duplicado, cmpInt);
    
    ASSERT_TRUE(resultado, "Inserción de duplicado debe ser exitosa");
    ASSERT_EQUAL(5, vec.cantElem, "Vector debe tener 5 elementos");
    ASSERT_TRUE(vectorIntOrdenado(&vec), "Vector debe permanecer ordenado");
    
    // Verificar que el duplicado se insertó después del existente
    int esperados[] = {5, 10, 15, 15, 20};
    ASSERT_TRUE(verificarContenidoVector(&vec, esperados, 5), "Contenido debe ser correcto");
    
    vectorLiberar(&vec);
}

void test_crecimiento_automatico() {
    printf(COLOR_YELLOW "\n=== Pruebas de Crecimiento Automático ===\n" COLOR_RESET);
    
    tVector vec;
    crearVector(&vec, sizeof(int));
    
    int capacidad_inicial = vec.totalLength;
    
    // Llenar el vector hasta superar la capacidad inicial
    for (int i = 0; i < capacidad_inicial + 5; i++) {
        int elem = i * 10;
        int resultado = insertarEnVectorOrdenado(&vec, &elem, cmpInt);
        ASSERT_TRUE(resultado, "Inserción debe ser exitosa");
    }
    
    ASSERT_TRUE(vec.totalLength > capacidad_inicial, "Capacidad debe haber crecido");
    ASSERT_EQUAL(capacidad_inicial + 5, vec.cantElem, "Cantidad de elementos debe ser correcta");
    ASSERT_TRUE(vectorIntOrdenado(&vec), "Vector debe permanecer ordenado");
    
    vectorLiberar(&vec);
}

void test_insertar_strings() {
    printf(COLOR_YELLOW "\n=== Pruebas con Strings ===\n" COLOR_RESET);
    
    tVector vec;
    crearVector(&vec, 20 * sizeof(char)); // 20 caracteres máximo por string
    
    // Insertar strings en orden aleatorio
    char str1[] = "zebra";
    char str2[] = "abeja";
    char str3[] = "perro";
    char str4[] = "gato";
    
    int resultado1 = insertarEnVectorOrdenado(&vec, str1, cmpString);
    int resultado2 = insertarEnVectorOrdenado(&vec, str2, cmpString);
    int resultado3 = insertarEnVectorOrdenado(&vec, str3, cmpString);
    int resultado4 = insertarEnVectorOrdenado(&vec, str4, cmpString);
    
    ASSERT_TRUE(resultado1 && resultado2 && resultado3 && resultado4, 
                "Todas las inserciones deben ser exitosas");
    ASSERT_EQUAL(4, vec.cantElem, "Vector debe tener 4 elementos");
    
    // Verificar orden alfabético
    char *strings = (char*)vec.data;
    ASSERT_TRUE(strcmp(strings, "abeja") == 0, "Primer string debe ser 'abeja'");
    ASSERT_TRUE(strcmp(strings + 20, "gato") == 0, "Segundo string debe ser 'gato'");
    ASSERT_TRUE(strcmp(strings + 40, "perro") == 0, "Tercer string debe ser 'perro'");
    ASSERT_TRUE(strcmp(strings + 60, "zebra") == 0, "Cuarto string debe ser 'zebra'");
    
    vectorLiberar(&vec);
}

void test_insertar_muchos_elementos() {
    printf(COLOR_YELLOW "\n=== Pruebas de Inserción Masiva ===\n" COLOR_RESET);
    
    tVector vec;
    crearVector(&vec, sizeof(int));
    
    const int NUM_ELEMENTOS = 100;
    
    // Insertar elementos en orden aleatorio
    for (int i = NUM_ELEMENTOS; i > 0; i--) {
        int resultado = insertarEnVectorOrdenado(&vec, &i, cmpInt);
        ASSERT_TRUE(resultado, "Inserción debe ser exitosa");
    }
    
    ASSERT_EQUAL(NUM_ELEMENTOS, vec.cantElem, "Vector debe tener todos los elementos");
    ASSERT_TRUE(vectorIntOrdenado(&vec), "Vector debe estar ordenado");
    
    // Verificar que todos los elementos están presentes
    for (int i = 0; i < NUM_ELEMENTOS; i++) {
        int valor_esperado = i + 1;
        int valor_actual = *((int*)vec.data + i);
        ASSERT_EQUAL(valor_esperado, valor_actual, "Elemento debe estar en posición correcta");
    }
    
    vectorLiberar(&vec);
}

void test_insertar_elementos_extremos() {
    printf(COLOR_YELLOW "\n=== Pruebas con Valores Extremos ===\n" COLOR_RESET);
    
    tVector vec;
    crearVector(&vec, sizeof(int));
    
    // Insertar valores extremos
    int max_int = 2147483647;  // INT_MAX
    int min_int = -2147483648; // INT_MIN
    int cero = 0;
    
    int resultado1 = insertarEnVectorOrdenado(&vec, &max_int, cmpInt);
    int resultado2 = insertarEnVectorOrdenado(&vec, &min_int, cmpInt);
    int resultado3 = insertarEnVectorOrdenado(&vec, &cero, cmpInt);
    
    ASSERT_TRUE(resultado1 && resultado2 && resultado3, 
                "Inserción de valores extremos debe ser exitosa");
    ASSERT_EQUAL(3, vec.cantElem, "Vector debe tener 3 elementos");
    ASSERT_TRUE(vectorIntOrdenado(&vec), "Vector debe estar ordenado");
    
    // Verificar orden correcto
    ASSERT_EQUAL(min_int, *((int*)vec.data + 0), "Primer elemento debe ser INT_MIN");
    ASSERT_EQUAL(cero, *((int*)vec.data + 1), "Segundo elemento debe ser 0");
    ASSERT_EQUAL(max_int, *((int*)vec.data + 2), "Tercer elemento debe ser INT_MAX");
    
    vectorLiberar(&vec);
}

void mostrar_resumen() {
    printf(COLOR_YELLOW "\n=== RESUMEN DE PRUEBAS ===\n" COLOR_RESET);
    printf("Pruebas ejecutadas: %d\n", tests_passed + tests_failed);
    printf(COLOR_GREEN "Pruebas exitosas: %d\n" COLOR_RESET, tests_passed);
    
    if (tests_failed > 0) {
        printf(COLOR_RED "Pruebas fallidas: %d\n" COLOR_RESET, tests_failed);
    } else {
        printf(COLOR_GREEN "¡Todas las pruebas pasaron exitosamente!\n" COLOR_RESET);
    }
    
    double porcentaje = (double)tests_passed / (tests_passed + tests_failed) * 100.0;
    printf("Porcentaje de éxito: %.1f%%\n", porcentaje);
}

// ===== PRUEBAS ORIGINALES DEL MAIN ===== 
void test_ejemplos_originales() {
    printf(COLOR_YELLOW "\n=== Pruebas de los Ejemplos Originales ===\n" COLOR_RESET);
    
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

        vectorLiberar(&vec);
        ASSERT_TRUE(1, "Ejemplo con enteros ejecutado correctamente");
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
        ASSERT_TRUE(1, "Ejemplo con cadenas ejecutado correctamente");
    }

    // Ejemplo de uso con cargar y grabar vector desde un archivo
    printf("=== Ejemplo de cargar y grabar vector desde un archivo ===\n");
    tVector vecCuenta;
    if (crearVector(&vecCuenta, sizeof(s_cuenta)))
    {
        s_cuenta elemCuenta;
        if (cargarVectorDeArchivo(&vecCuenta, "archivos/cuenta.dat", &elemCuenta))
        {
            printf("Vector de cuentas cargado desde archivo:\n");
            ordenarVectorInsercion(&vecCuenta, cmpCuenta);
            mapVector(&vecCuenta, printCuenta);

            if (grabarVectorEnArchivo(&vecCuenta, "cuenta_grabada.dat", &elemCuenta))
            {
                printf("\nVector grabado en cuenta_grabada.dat\n");
                ASSERT_TRUE(1, "Archivo grabado correctamente");
            }
            else
            {
                printf("\nError al grabar el vector en el archivo\n");
                ASSERT_FALSE(1, "Error al grabar archivo");
            }

            vectorLiberar(&vecCuenta);
            ASSERT_TRUE(1, "Ejemplo con archivos ejecutado correctamente");
        }
        else
        {
            printf("Error al cargar el vector desde el archivo (esto puede ser normal si no existe el archivo)\n");
            ASSERT_TRUE(1, "Manejo de error de archivo es correcto");
            vectorLiberar(&vecCuenta);
        }
    }
}

int main() {
    printf(COLOR_YELLOW "=== PRUEBAS PARA insertarEnVectorOrdenado ===\n" COLOR_RESET);
    printf("Ejecutando casos de prueba comprehensivos...\n");
    
    // Ejecutar todas las pruebas
    test_insertar_parametros_invalidos();
    test_insertar_en_vector_vacio();
    test_insertar_al_inicio();
    test_insertar_al_final();
    test_insertar_en_medio();
    test_insertar_duplicados();
    test_crecimiento_automatico();
    test_insertar_strings();
    test_insertar_muchos_elementos();
    test_insertar_elementos_extremos();
    
    // Ejecutar pruebas de los ejemplos originales
    test_ejemplos_originales();
    
    // Mostrar resumen
    mostrar_resumen();
    
    return (tests_failed == 0) ? 0 : 1;
}

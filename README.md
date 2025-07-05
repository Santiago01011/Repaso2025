# Tópicos de Programación - Examen Parcial - Repaso 2025

## Información General

| **Comisión:** | Ing. en Informática |
| :--- | :--- |
| **Fecha:** | 04/07/2025 |
| **Duración:** | 3 horas |
| **Apellido y Nombre:** | Zapata Santiago_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_ |
| **DNI:** | \_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_ |
| **Calificación:** | \_\_\_\_\_\_\_\_\_ |

---

## Instrucciones Generales

- El examen consta de **2 (dos) ejercicios obligatorios**.
- El código debe ser escrito en **C** y compilar con **GCC**.
- Se debe entregar un único archivo `.zip` nombrado `DNI-ApellidoNombre.zip`, que contenga carpetas separadas para cada ejercicio (`ejercicio1`, `ejercicio2`).
- La **claridad del código**, el buen uso de la **memoria** y el **diseño modular** serán parte de la evaluación.

---

## Ejercicio 1: Gestión de Inventario de Almacén (70 Puntos)

La empresa "Logística Express" necesita un programa para actualizar el inventario de su almacén. El estado actual del stock se encuentra en un archivo binario `stock.dat`, ordenado por código de producto (`codigo_prod`).

#### Estructura del Registro de Stock

```c
typedef struct {
    char codigo_prod[6]; // Ej: "PROD1"
    char descripcion[50];
    int cantidad;
} s_stock_item;
```

Diariamente, se recibe un archivo de texto `operaciones.txt` con las novedades del día, también ordenado por `codigo_prod`. Cada línea del archivo de texto representa una operación y tiene el formato `codigo_prod;tipo;cantidad`.

**Ejemplo de `operaciones.txt`:**

```
PROD2;E;20
PROD5;S;5
PROD9;E;100
```

El **tipo de operación** puede ser:
- `'E'` (Entrada): Se debe sumar la cantidad al stock del producto.
- `'S'` (Salida): Se debe restar la cantidad al stock del producto.

### Se Pide:

1.  **Diseño Modular**: Crear los archivos `lib_inventario.h` y `lib_inventario.c`. Toda la lógica de actualización debe residir en una única función dentro de estos archivos con la siguiente firma:

    ```c
    int actualizar_stock(const char* ruta_stock, const char* ruta_operaciones, void (*actualizar)(s_stock_item*, char, int));
    ```
    - Esta función implementará el **apareo de archivos (merge)** para actualizar `stock.dat`.
    - Debe devolver la cantidad de registros de stock que fueron actualizados.

2.  **Lógica de Actualización Genérica (Uso de Puntero a Función)**: La función `actualizar_stock` no debe contener la lógica de suma/resta directamente. En su lugar, debe invocar al puntero a función `actualizar` que recibe como parámetros:
    - Un puntero al `s_stock_item` a modificar.
    - El tipo de operación (`'E'` o `'S'`).
    - La cantidad de la operación.
    En su archivo `main.c`, usted deberá definir la función que implemente la lógica de suma/resta y pasarla como argumento a `actualizar_stock`.

3.  **Manejo de Novedades**:
    - El archivo de operaciones puede contener códigos de producto que no existen en `stock.dat`. Estos deben ser ignorados y reportados en un archivo de log llamado `log_errores.txt`, indicando el código no encontrado y la operación.
    - El stock de un producto puede ser negativo si salen más unidades de las que hay.

4.  **Programa Principal (`main.c`)**: Crear un `main.c` que invoque a la función `actualizar_stock` y, al finalizar, muestre por pantalla un mensaje indicando: `"Proceso finalizado. Se actualizaron X productos."`

---

## Ejercicio 2: Implementación de Función de Memoria (30 Puntos)

Desarrolle su propia versión de la función `memcpy` de la biblioteca estándar de C. La función **no debe utilizar ninguna función de la biblioteca `<string.h>`**.

#### Firma de la función:

```c
void* mi_memcpy(void* destino, const void* origen, size_t n);
```

#### Descripción del Comportamiento:

- La función debe copiar `n` bytes desde el área de memoria apuntada por `origen` al área de memoria apuntada por `destino`.
- Las áreas de memoria no deben superponerse (no es necesario que implemente la lógica de `memmove`).
- La función debe devolver un puntero al área de memoria de `destino`.

#### Prueba:

Incluir en un `main.c` de este ejercicio un caso de prueba simple que demuestre que su función copia correctamente un array de `int` y un array de `char`.
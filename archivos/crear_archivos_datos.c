#include <stdio.h>
#include <string.h>

typedef struct {
    char codigo_cta[9]; // Ej: "123456/5"
    float saldo;;
} s_cuenta;

int main() {
    // Crear archivo cuenta.dat con datos de ejemplo
    FILE* archivo_cuenta = fopen("cuenta.dat", "wb");
    if (!archivo_cuenta) {
        printf("Error al crear cuenta.dat\n");
        return 1;
    }

    // Datos de cuenta inicial (ordenados por codigo_prod)
    s_cuenta items[] = {
        {"123456/5", 1500.50},
        {"179235/2", 2000.00},
        {"382156/1", 500.75},
        {"637043/2", 1000.00},
        {"642315/6", 750.25},
        {"653214/9", 300.00},
        {"873152/3", 1200.00},
        {"123456/7", 800.00}
    };

    int cantidad_items = sizeof(items) / sizeof(s_cuenta);
    
    for (int i = 0; i < cantidad_items; i++) {
        fwrite(&items[i], sizeof(s_cuenta), 1, archivo_cuenta);
    }
    
    fclose(archivo_cuenta);
    printf("Archivo cuenta.dat creado con %d productos.\n", cantidad_items);

    // Crear archivo movimientos.txt
    FILE* archivo_movimientos = fopen("movimientos.txt", "w");
    if (!archivo_movimientos) {
        printf("Error al crear movimientos.txt\n");
        return 1;
    }
    /**
     * Claro! Analicemos en detalle la validación del número de cuenta. Es un algoritmo ingenioso que combina aritmética y 
     * manipulación de dígitos.

La consigna nos dice que un número de cuenta completo tiene el formato 

NNNNNN/D, donde NNNNNN es el número secuencial de 6 dígitos y D es el dígito verificador. 

El objetivo es calcular D a partir de NNNNNN. Aquí te explico el proceso paso a paso, usando el mismo ejemplo del parcial: 

637043.

## Paso 1: Separar los dígitos en posiciones pares e impares
Primero, tomamos el número secuencial de 6 dígitos y lo dividimos en dos grupos: los dígitos que están en posiciones impares (1ª, 3ª, 5ª) 
y los que están en posiciones pares (2ª, 4ª, 6ª).

Para el número 637043:

Dígitos en posiciones impares: 6, 7, 4

Dígitos en posiciones pares: 3, 0, 3

## Paso 2: Calcular la diferencia absoluta
Ahora, formamos dos números nuevos juntando los dígitos de cada grupo y calculamos la diferencia absoluta (el valor sin signo negativo) 
entre ellos. 

Número formado por dígitos impares: 674

Número formado por dígitos pares: 303

Diferencia absoluta: |674 - 303| = 371 

## Paso 3: Sumar las cifras del resultado hasta obtener un solo dígito
Este es el paso recursivo.  Tomamos el resultado de la resta (

371) y sumamos sus dígitos. Si el resultado de esa suma tiene más de un dígito, repetimos el proceso hasta que nos quede un único dígito.

Primera suma:

Resultado de la resta: 371

Suma de sus dígitos: 

3 + 7 + 1 = 11 

Segunda suma (porque 11 tiene dos dígitos):

Resultado de la suma anterior: 11

Suma de sus dígitos: 

1 + 1 = 2 

Como 2 es un solo dígito, hemos terminado. El dígito verificador es 2.

## Resultado Final
Finalmente, unimos el número secuencial original con el dígito verificador que calculamos, separados por una barra.

Número de cuenta completo: 

637043/2 

En resumen, el proceso es: separar por posición → restar → sumar cifras recursivamente. Para implementar esto en tu código, necesitarás manipular el número (probablemente con operaciones de módulo % y división /) para extraer cada dígito individualmente.
     */   //123456/5;D;1500.50
    // movimientos del día (ordenadas por codigo_prod)
    fprintf(archivo_movimientos, "637043/2;D;20\n");
    fprintf(archivo_movimientos, "123456/5;D;20\n");
    fprintf(archivo_movimientos, "642315/6;C;10\n");
    fprintf(archivo_movimientos, "873152/3;C;5\n");
    fprintf(archivo_movimientos, "653214/9;D;12\n");
    fprintf(archivo_movimientos, "382156/1;D;100\n");
    fprintf(archivo_movimientos, "179235/2;D;50\n");
    fprintf(archivo_movimientos, "123456/7;C;25\n");

    fclose(archivo_movimientos);
    printf("Archivo movimientos.txt creado con movimientos de ejemplo.\n");

    return 0;
}

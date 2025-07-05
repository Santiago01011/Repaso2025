#include "mergeStockOperaciones/lib_inventario.h"

/**
T�picos de Programaci�n - Simulaci�n de Examen Parcial
Comisi�n: Ing. en Inform�tica

Fecha: 04/07/2025

Duraci�n: 3 horas

Apellido y Nombre: _________________________

DNI: _________________________

Calificaci�n: _________

Instrucciones Generales:

El examen consta de 2 (dos) ejercicios obligatorios.

El c�digo debe ser escrito en C y compilar con GCC.

Se debe entregar un �nico archivo .zip nombrado DNI-ApellidoNombre.zip, que contenga carpetas separadas para cada ejercicio (ejercicio1, ejercicio2).

La claridad del c�digo, el buen uso de la memoria y el dise�o modular ser�n parte de la evaluaci�n.

M�s informaci�n en README.md

*/

#define RUTA_OPERACIONES "archivos/operaciones.txt"
#define RUTA_STOCK "archivos/stock.dat"

void actualizarStock(s_stock_item* item, char tipo_operacion, int cantidad);

int main()
{
    actualizar_stock(RUTA_STOCK, RUTA_OPERACIONES, actualizarStock);
    return 0;
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

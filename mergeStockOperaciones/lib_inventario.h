#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct {
    char codigo_prod[6]; // Ej: "PROD1"
    char descripcion[50];
    int cantidad;
} s_stock_item;

int actualizar_stock(const char* ruta_stock, const char* ruta_operaciones, void (*actualizar)(s_stock_item*, char, int));
int readOperacion(s_stock_item* op, FILE* arch);

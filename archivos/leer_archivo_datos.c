#include <stdio.h>
#include <string.h>

typedef struct {
    char codigo_cta[9]; // Ej: "123456/5"
    float saldo;;
} s_cuenta;



int main(){
    FILE* archivo_cuenta = fopen("cuentas.dat", "rb");
    if (!archivo_cuenta) {
        printf("Error al crear cuentas.dat\n");
        return 1;
    }

    s_cuenta item;
    int cantidad_items = 0;
    while (fread(&item, sizeof(s_cuenta), 1, archivo_cuenta) == 1) {
        printf("Cuenta %d: Codigo: %s, Saldo: %.2f\n", cantidad_items + 1, item.codigo_cta, item.saldo);
        cantidad_items++;
    }
    fclose(archivo_cuenta);
    printf("Archivo cuenta.dat contiene %d cuentas.\n", cantidad_items);

    return 0;
}
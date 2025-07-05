#include "lib_bancaria.h"
#include <ctype.h>

#define RUTA_MOVIMIENTOS "../archivos/movimientos.txt"
#define RUTA_CUENTAS "../archivos/cuentas.dat"

int calcularDigitoVerificador(const char* numero_cuenta) {
    if (strlen(numero_cuenta) != 6) {
        return -1;
    }
    
    int suma_pares = 0, suma_impares = 0;
    
    for (int i = 0; i < 6; i++) {        
        int digito = numero_cuenta[i] - '0';
        if (i % 2 == 0)
            suma_pares += digito;
        else
            suma_impares += digito;
    }
    
    int diferencia = abs(suma_impares - suma_pares);
    
    while (diferencia >= 10) {
        int nueva_suma = 0;
        while (diferencia > 0) {
            nueva_suma += diferencia % 10;
            diferencia /= 10;
        }
        diferencia = nueva_suma;
    }
    
    return diferencia;
}

int validarCuentaBancaria(const char* cuenta_completa) {
    char numero[7];
    int verificador_leido, verificador_calculado;
    
    if (sscanf(cuenta_completa, "%6[^/]/%d", numero, &verificador_leido) != 2) {
        return 0;
    }
    
    verificador_calculado = calcularDigitoVerificador(numero);
    if (verificador_calculado == -1) {
        return 0;
    }
    
    return verificador_leido == verificador_calculado;
}

int leerMovimiento(s_cuenta *registro, char *mov, FILE *arch){
    char buffer[128];
    char numero_cuenta[7];
    int validador_leido;
    
    if (fgets(buffer, sizeof(buffer), arch) == NULL)
        return 0;
    
    if (sscanf(buffer, "%[^/]/%d;%c;%f", numero_cuenta, &validador_leido, mov, &registro->saldo) != 4) {
        return 0;
    }
    
    sprintf(registro->codigo_cta, "%s/%d", numero_cuenta, validador_leido);
    
    int validador_calculado = calcularDigitoVerificador(numero_cuenta);
    if (validador_calculado == -1 || validador_calculado != validador_leido) {
        return -1;
    }
    
    return 1;
}

int cmpCuenta(const void *a, const void *b) {
    const s_cuenta *cuentaA = (const s_cuenta *)a;
    const s_cuenta *cuentaB = (const s_cuenta *)b;
    
    // Extraer solo el número de cuenta (sin dígito verificador) de cuentaB
    char numero_cuentaB[7];
    sscanf(cuentaB->codigo_cta, "%6[^/]", numero_cuentaB);
    
    // Comparar el código completo de A con solo el número de B
    return strcmp(cuentaA->codigo_cta, numero_cuentaB);
}

void actualizarSaldo(s_cuenta *cuenta, char tipo, float monto) {
    if (tipo == 'D') { // Débito: retirar dinero
        cuenta->saldo -= monto;
    } else if (tipo == 'C') { // Crédito: depositar dinero
        cuenta->saldo += monto;
    }
}

int gestionar_cuenta(const char* nombreCuentas, const char* nombreMovimientos, 
    void(*actualizar)(s_cuenta*, char, float), int(*cmp)(const void*, const void*))
{
    FILE *pfc, *pfm, *pfe, *pfa;
    char mov;
    int lectura_cuenta, lectura_movimiento, resCmp;
    s_cuenta aux1, aux2;

    pfc = fopen(nombreCuentas, "rb");
    if(!pfc){
        printf("No se pudo abrir el archivo %s\n", nombreCuentas);
        return 0;
    }
    pfm = fopen(nombreMovimientos, "rt");
    if(!pfm){
        printf("No se pudo abrir el archivo %s\n", nombreMovimientos);
        fclose(pfc);
        return 0;
    }

    pfe = fopen("../archivos/error_nro_cta.txt", "wt");
    if(!pfe){
        puts("No se pudo crear el archivo error_nro_cta.txt \n");
        fclose(pfc);
        fclose(pfm);
        return 0;
    }

    pfa = fopen("../archivos/cuentas.aux", "wb");
    if(!pfa){
        puts("No se pudo crear el archivo cuentas.aux\n");
        fclose(pfc);
        fclose(pfm);
        fclose(pfe);
        return 0;
    }

    lectura_cuenta = fread(&aux1, sizeof(s_cuenta), 1, pfc);
    lectura_movimiento = leerMovimiento(&aux2, &mov, pfm);
    
    printf("Debug: Primer registro leído - Cuenta: '%s', Saldo: %.2f\n", aux1.codigo_cta, aux1.saldo);
    if (lectura_movimiento > 0) {
        printf("Debug: Primer movimiento - Cuenta: '%s', Tipo: %c, Monto: %.2f\n", aux2.codigo_cta, mov, aux2.saldo);
    }
    
    int contador_procesados = 0;
    
    while(lectura_cuenta && lectura_movimiento) {
        if(lectura_movimiento == -1){
            fprintf(pfe, "El registro no fue procesado, numero de cuenta invalido: %s\n", aux2.codigo_cta);
            lectura_movimiento = leerMovimiento(&aux2, &mov, pfm);
            continue;            
        }
        resCmp = cmp(&aux1, &aux2);
        printf("Debug: Comparando '%s' vs '%s' = %d\n", aux1.codigo_cta, aux2.codigo_cta, resCmp);
        
        if(resCmp == 0){
            printf("Debug: ¡Coincidencia! Actualizando cuenta %s (%.2f) con %c %.2f\n", 
                   aux1.codigo_cta, aux1.saldo, mov, aux2.saldo);
            actualizar(&aux1, mov, aux2.saldo);
            printf("Debug: Nuevo saldo: %.2f\n", aux1.saldo);
            fwrite(&aux1, sizeof(s_cuenta), 1, pfa);
            lectura_cuenta = fread(&aux1, sizeof(s_cuenta), 1, pfc);
            lectura_movimiento = leerMovimiento(&aux2, &mov, pfm);
            contador_procesados++;
        }
        if(resCmp<0){
            fwrite(&aux1, sizeof(s_cuenta), 1, pfa);
            lectura_cuenta = fread(&aux1, sizeof(s_cuenta), 1, pfc);
        }

        if(resCmp>0){
            lectura_movimiento = leerMovimiento(&aux2, &mov, pfm);
        }
    }

    while(lectura_cuenta){
        fwrite(&aux1, sizeof(s_cuenta), 1, pfa);
        lectura_cuenta = fread(&aux1, sizeof(s_cuenta), 1, pfc);
    }

    while(lectura_movimiento){
        if(lectura_movimiento == -1){
            fprintf(pfe, "El registro no fue procesado, numero de cuenta invalido: %s\n", aux2.codigo_cta);
            lectura_movimiento = leerMovimiento(&aux2, &mov, pfm);
            continue;            
        }        
        lectura_movimiento = leerMovimiento(&aux2, &mov, pfm);
    }
    fclose(pfc);
    fclose(pfm);
    fclose(pfe);
    fclose(pfa);

    printf("Debug: Movimientos procesados exitosamente: %d\n", contador_procesados);

    remove(nombreCuentas);
    rename("../archivos/cuentas.aux", nombreCuentas);
    return 1;
}


int main() {
    printf("=== Debug: Iniciando gestión de cuentas ===\n");
    
    if (gestionar_cuenta(RUTA_CUENTAS, RUTA_MOVIMIENTOS, actualizarSaldo, cmpCuenta)) {
        printf("Gestión de cuentas completada con éxito.\n");
    } else {
        printf("Error en la gestión de cuentas.\n");
    }
    return 0;
}

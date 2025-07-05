#ifndef LIB_BANCARIA_H
#define LIB_BANCARIA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char codigo_cta[10]; // Para permitir "123456/5" + '\0'
    float saldo;
} s_cuenta;

int leerMovimiento(s_cuenta *registro, char *mov, FILE *arch);
int gestionar_cuenta(const char* nombreCuentas, const char* nombreMovimientos, void(*actualizar)(s_cuenta*, char, float), int(*cmp)(const void*, const void*));
int calcularDigitoVerificador(const char* numero_cuenta);
int validarCuentaBancaria(const char* cuenta_completa);
int cmpCuenta(const void *a, const void *b);
void actualizarSaldo(s_cuenta *cuenta, char tipo, float monto);

#endif // LIB_BANCARIA_H
#include "main.h"
#include "menu/menu.h"
/** Consigna:
Ejercicio Tiempo: 60 minutos
 Desarrollar una aplicaci�n que cuente con un men� para:
 - Cargar una matriz cuadrada de valores reales de orden �N�. La matriz debe cargarse a
 partir de un archivo texto, �datos.txt�. Su primer registro contiene la dimensi�n �N� de la
 matriz a cargar. Los registros siguientes, corresponden a un registro por fila, y cada valor
 est� separado por �|�. Este archivo no contiene espacios en blanco.
 Ej:
 2
 1.1|2.2
 3.1|1.2
 - generar archivo: esta opci�n debe generar el archivo
 �triangulo_inf_entre_diags.txt�. Este archivo contiene el tri�ngulo
 inferior que se forma por el cruce de la diagonal principal y la
 diagonal secundaria. NO incluye los valores de las diagonales.
 La idea es que los elementos de este archivo respeten la forma que tienen los elementos
 en la matriz original. Como primera instancia cree el archivo con todos los elementos
 solicitados. Cuanto mejor pueda reproducir su ubicaci�n original, mejor.
 - Mostrar la matriz completa.
 - Mostrar el archivo generado.
 El valor �N� es definido en tiempo de compilaci�n es de 20.
*/
int main()
{
    mainMenu("\n\nSELECT AN OPTION\nC- Charge matrix.txt.\nG- Generate triangle files\nM- Show full matrix\nI- Show inferior triangle\nS- Show superior triangle\nR- Show right triangle\nL- Show left triangle\nD- Show main diagonal\nE- Show inverse diagonal\nF- finish.\noption: ");
    return 0;
}

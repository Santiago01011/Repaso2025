#include "lib_inventario.h"



/**
 * @brief Updates a stock file based on an operations file.
 *
 * This function merges operations from a text file into a binary stock file.
 * It reads both files, which are assumed to be sorted by product code.
 * It applies the operations to the corresponding stock items and writes the
 * updated stock to a new file. Products from the operations file that are not
 * found in the stock file are logged as errors.
 *
 * After processing, the original stock file is replaced with the newly
 * generated, updated stock file.
 *
 * @param ruta_stock Path to the binary stock file (e.g., "stock.dat").
 * @param ruta_operaciones Path to the text file with operations (e.g., "operaciones.txt").
 * @param actualizar A function pointer to the callback function that performs the
 *                   actual update on a stock item. This function receives the
 *                   stock item, the operation type, and the quantity.
 * @return The number of stock records that were successfully updated. Returns -1
 *         if there is an error opening any of the required files.
 */
int actualizar_stock(const char* ruta_stock, const char* ruta_operaciones, void (*actualizar)(s_stock_item*, char, int)){
    FILE *pfs, *pfo, *pfe, *pfn;
    s_stock_item aux1, aux2;
    int res = 0, registros_actualizados = 0;


    pfo = fopen(ruta_operaciones, "rt");
    if(!pfo){
        printf("Error al abrir el archivo: %s", ruta_operaciones);
        return -1;
    }

    pfs = fopen(ruta_stock, "r+b");
    if(!pfs){
        printf("Error al abrir el archivo: %s", ruta_stock);
        fclose(pfo);
        return -1;
    }
    pfe = fopen("../archivos/errores.txt", "w");
    if(!pfe){
        printf("Error al abrir el archivo: %s", "archivos/errores.txt");
        fclose(pfs);
        fclose(pfo);
        return -1;
    }
    pfn = fopen("../archivos/nuevo_stock.dat", "wb");
    if(!pfn){
        printf("Error al abrir el archivo: %s", "archivos/nuevo_stock.dat");
        fclose(pfs);
        fclose(pfo);
        fclose(pfe);
        return -1;
    }

    fread(&aux1, sizeof(s_stock_item), 1, pfs);
    int hay_stock = !feof(pfs);
    int hay_operacion = readOperacion(&aux2, pfo);
    
    while( hay_stock || hay_operacion ){
        if( !hay_stock ){
            // Solo quedan operaciones - reportar error
            fprintf(pfe, "Producto %s no encontrado en stock.dat\n", aux2.codigo_prod);
            hay_operacion = readOperacion(&aux2, pfo);
        }
        else if( !hay_operacion ){
            // Solo queda stock - copiar sin modificar
            fwrite(&aux1, sizeof(s_stock_item), 1, pfn);
            hay_stock = (fread(&aux1, sizeof(s_stock_item), 1, pfs) == 1);
        }
        else {
            res = strcmp(aux1.codigo_prod, aux2.codigo_prod);
            if( res == 0 ){
                actualizar(&aux1, aux2.descripcion[0], aux2.cantidad);
                registros_actualizados++;
                hay_operacion = readOperacion(&aux2, pfo);
                // Si no hay más operaciones o la siguiente es de otro producto, escribir y avanzar
                if( !hay_operacion || strcmp(aux1.codigo_prod, aux2.codigo_prod) != 0 ){
                    fwrite(&aux1, sizeof(s_stock_item), 1, pfn);
                    hay_stock = (fread(&aux1, sizeof(s_stock_item), 1, pfs) == 1);
                }
            }
            else if( res < 0 ){
                fwrite(&aux1, sizeof(s_stock_item), 1, pfn);
                hay_stock = (fread(&aux1, sizeof(s_stock_item), 1, pfs) == 1);
            }
            else {
                fprintf(pfe, "Producto %s no encontrado en stock.dat\n", aux2.codigo_prod);
                hay_operacion = readOperacion(&aux2, pfo);
            }
        }
    }

    fclose(pfs);
    fclose(pfo);
    fclose(pfe);
    fclose(pfn);
    remove(ruta_stock);
    rename("../archivos/nuevo_stock.dat", ruta_stock);

    return registros_actualizados;
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

int main()
{   
    int registrosActualizados = actualizar_stock("../archivos/stock.dat", "../archivos/operaciones.txt", actualizarStock);
    if (registrosActualizados < 0) {
        printf("Error al actualizar el stock.\n");
        return 1;
    }
    printf("Stock actualizado correctamente, %d registros modificados.\n", registrosActualizados);
    return 0;
}


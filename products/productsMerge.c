#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// =================================================================
// DEFINITIONS
// =================================================================

#define MASTER_FILE "productos.dat"
#define TRANSACTION_FILE "novedades.txt"
#define ERROR_LOG_FILE "errores_stock.txt"

// Struct for the master file (binary)
typedef struct {
    char codigo[7];     // e.g., "PROD01"
    char nombre[30];
    float precio;
    int stock;
} Producto;

// Struct for the transaction file (text)
typedef struct {
    char codigo[7];
    int cantidad; // Can be positive (entry) or negative (exit)
} NovedadStock;


// =================================================================
// FUNCTION PROTOTYPES
// =================================================================

// --- Core Merge Logic ---
int actualizarStock(const char* pathMaestro, const char* pathNovedades, const char* pathErrores);

// --- Helper Functions ---
void generarArchivosDePrueba();
void mostrarProductos(const char* path);
int leerNovedad(FILE* f, NovedadStock* nov);
int cmpProductos(const void* p1, const void* p2);


// =================================================================
// MAIN FUNCTION
// =================================================================

int main() {
    printf("### UPDATE MERGE EXAMPLE ###\n\n");

    // 1. Create dummy files for our test case
    generarArchivosDePrueba();

    // 2. Show the initial state of the master file
    printf("--- 1. Initial State of %s ---\n", MASTER_FILE);
    mostrarProductos(MASTER_FILE);
    printf("----------------------------------------\n\n");

    // 3. Run the update merge process
    printf("--- 2. Running the update merge... ---\n");
    int actualizados = actualizarStock(MASTER_FILE, TRANSACTION_FILE, ERROR_LOG_FILE);
    if (actualizados >= 0) {
        printf("Merge successful. %d master records were written.\n", actualizados);
    } else {
        printf("Merge failed.\n");
    }
    printf("----------------------------------------\n\n");

    // 4. Show the final state of the master file
    printf("--- 3. Final State of %s ---\n", MASTER_FILE);
    mostrarProductos(MASTER_FILE);
    printf("----------------------------------------\n");

    return 0;
}


// =================================================================
// CORE MERGE IMPLEMENTATION
// =================================================================

/**
 * @brief Updates a master product file with stock transactions.
 * This is a classic "Update Merge" pattern.
 */
int actualizarStock(const char* pathMaestro, const char* pathNovedades, const char* pathErrores) {
    // --- Setup Phase ---
    FILE *pfMaestro, *pfNovedades, *pfErrores, *pfTemp;
    Producto prod;
    NovedadStock nov;

    pfMaestro = fopen(pathMaestro, "rb");
    if (!pfMaestro) {
        perror("Error opening master file");
        return -1;
    }

    pfNovedades = fopen(pathNovedades, "rt");
    if (!pfNovedades) {
        perror("Error opening transaction file");
        fclose(pfMaestro);
        return -1;
    }
    
    // We write to a temporary file. This is crucial for safety.
    // If the program crashes mid-merge, the original master file is untouched.
    pfTemp = fopen("productos.tmp", "wb");
    if (!pfTemp) {
        perror("Error opening temporary file");
        fclose(pfMaestro);
        fclose(pfNovedades);
        return -1;
    }

    pfErrores = fopen(pathErrores, "wt");
    if (!pfErrores) {
        printf("Warning: Could not open error log file. Continuing without logging.\n");
    }

    // --- Priming Read ---
    int leidoMaestro = fread(&prod, sizeof(Producto), 1, pfMaestro);
    int leidoNovedad = leerNovedad(pfNovedades, &nov);
    int registrosEscritos = 0;

    // --- Main Merge Loop ---
    while (leidoMaestro && leidoNovedad) {
        int cmp = strcmp(prod.codigo, nov.codigo);

        if (cmp < 0) {
            // Master record's code is smaller. It has no transaction.
            // Write it to the temp file as-is and read the next master.
            fwrite(&prod, sizeof(Producto), 1, pfTemp);
            registrosEscritos++;
            leidoMaestro = fread(&prod, sizeof(Producto), 1, pfMaestro);
        }
        else if (cmp > 0) {
            // Transaction's code is smaller. It's a transaction for a non-existent product.
            // Log it as an error and read the next transaction.
            if (pfErrores) {
                fprintf(pfErrores, "ERROR: Product code '%s' from transaction file not found in master file.\n", nov.codigo);
            }
            leidoNovedad = leerNovedad(pfNovedades, &nov);
        }
        else { // cmp == 0: Keys match. This is where the update happens.
            
            // Inner loop to process ALL transactions for the current product.
            while (leidoNovedad && strcmp(prod.codigo, nov.codigo) == 0) {
                // Apply the update to the master record in memory.
                prod.stock += nov.cantidad;
                // Read the next transaction to see if it's also for this product.
                leidoNovedad = leerNovedad(pfNovedades, &nov);
            }
            
            // After all transactions for this product are processed, write the
            // final, updated master record to the temp file.
            fwrite(&prod, sizeof(Producto), 1, pfTemp);
            registrosEscritos++;
            // And now, read the next master record.
            leidoMaestro = fread(&prod, sizeof(Producto), 1, pfMaestro);
        }
    }

    // --- Draining Phase ---
    // If there are leftover records in the master file, copy them as-is.
    while (leidoMaestro) {
        fwrite(&prod, sizeof(Producto), 1, pfTemp);
        leidoMaestro = fread(&prod, sizeof(Producto), 1, pfMaestro);
    }
    // If there are leftover transactions, log them as errors.
    while (leidoNovedad) {
        if (pfErrores) {
            fprintf(pfErrores, "ERROR: Product code '%s' from transaction file not found in master file.\n", nov.codigo);
        }
        leidoNovedad = leerNovedad(pfNovedades, &nov);
    }

    // --- Cleanup and Finalization ---
    fclose(pfMaestro);
    fclose(pfNovedades);
    fclose(pfTemp);
    if (pfErrores) fclose(pfErrores);

    // Atomically replace the old master file with the new one.
    remove(pathMaestro);
    rename("productos.tmp", pathMaestro);

    return registrosEscritos;
}


// =================================================================
// HELPER FUNCTIONS
// =================================================================

/**
 * @brief Reads a line from the text-based transaction file.
 * Returns 1 on success, 0 on EOF or error.
 */
int leerNovedad(FILE* f, NovedadStock* nov) {
    char line[100];
    if (!fgets(line, sizeof(line), f)) {
        return 0;
    }
    // sscanf returns the number of items successfully parsed.
    return sscanf(line, "%6[^,],%d", nov->codigo, &nov->cantidad) == 2;
}

/**
 * @brief Comparison function for qsort.
 */
int cmpProductos(const void* p1, const void* p2) {
    return strcmp(((Producto*)p1)->codigo, ((Producto*)p2)->codigo);
}

/**
 * @brief Creates dummy files for testing the merge logic.
 */
void generarArchivosDePrueba() {
    FILE *f;

    // --- Create Master File: productos.dat ---
    f = fopen(MASTER_FILE, "wb");
    if (!f) return;
    Producto productos[] = {
        {"PROD01", "Laptop 14 inch", 1200.50, 50},
        {"PROD02", "Mouse Inalambrico", 25.00, 200}, // Will have multiple updates
        {"PROD04", "Teclado Mecanico", 75.99, 100},
        {"PROD05", "Monitor 24 inch", 350.00, 80}  // Will not be updated
    };
    // Ensure master file is sorted by code for the merge to work
    qsort(productos, sizeof(productos)/sizeof(Producto), sizeof(Producto), cmpProductos);
    fwrite(productos, sizeof(Producto), sizeof(productos)/sizeof(Producto), f);
    fclose(f);

    // --- Create Transaction File: novedades.txt ---
    f = fopen(TRANSACTION_FILE, "wt");
    if (!f) return;
    // Note: This file must also be sorted by product code.
    fprintf(f, "PROD01,-5\n");      // Sell 5 laptops
    fprintf(f, "PROD02,100\n");     // Receive 100 mice
    fprintf(f, "PROD02,-15\n");     // Sell 15 mice
    fprintf(f, "PROD03,50\n");      // Transaction for a non-existent product
    fprintf(f, "PROD04,-20\n");     // Sell 20 keyboards
    fclose(f);
}

/**
 * @brief Displays the content of the binary product file.
 */
void mostrarProductos(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        perror("Could not open product file for display");
        return;
    }
    Producto p;
    printf("%-8s %-20s %-10s %-6s\n", "Codigo", "Nombre", "Precio", "Stock");
    printf("----------------------------------------------------\n");
    while(fread(&p, sizeof(Producto), 1, f)) {
        printf("%-8s %-20s %-10.2f %-6d\n", p.codigo, p.nombre, p.precio, p.stock);
    }
    fclose(f);
}

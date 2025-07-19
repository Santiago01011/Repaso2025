#include "socios.h"
#define STAM 48


int title(){

    printf("   _                   __\n");
    printf("  |_     _ o  _  ._   (_   _   _ o  _   _\n");
    printf("  | |_| _> | (_) | |  __) (_) (_ | (_) _>\n");
    printf("\n");
    return 1;
}

//
//int compara_fechas_res(const t_fecha *f1, const t_fecha *f2)
//
////compara dos estructuras t_fecha, devolviendo:
//FECHA_MENOR  --->  f1  < f2
//FECHA_IGUAL  --->  f1 == f2
//FECHA_MAYOR  --->  f1  > f2

/**
* Compares 2 dates and returns:
* Positive if the first is greater than the second,
* Negative if the first is not greater than the second.
* Zero if the dates are equal.
*/
int cmpDate(const void* d1, const void* d2){
    t_date* fDate = (t_date*)d1;
    t_date* sDate = (t_date*)d2;
    if ( fDate->y != sDate->y )
        return fDate->y - sDate->y;
    else if ( fDate->m != sDate->m )
        return fDate->m - sDate->m;
    else if ( fDate->d != sDate->d )
        return fDate->d - sDate->d;
    else
        return 0;
}

/**
* Compares 2 t_socios by DNI and returns:
* Positive if the first is greater than the second.
* Negative if the first is not greater than the second.
* Zero if they are equal.
*/
int cmpSocio(const void* d1, const void* d2){
    return ((t_socio*)d1)->dni - ((t_socio*)d2)->dni;
}


/**
* Reads a register from a text archive and returns a t_socio structure.
*/
int readSocioFromTXT(FILE* arch, t_socio* dest){
    char buffer[STAM];
    if (!arch){
        puts("File pointer is NULL");
        return 0;
    }
    if ( fgets(buffer, STAM, arch) ){
        if( sscanf(buffer, "%ld|%29[^|]|%d/%d/%d",
            &dest->dni, dest->name,
            &dest->inscription_date.d, &dest->inscription_date.m,
            &dest->inscription_date.y) )
                return 1;
            else
                return 0;
    }
    else
        return 0;
}

/**
 * Reads a register from a text archive robustly, handling lines that are
 * longer than the buffer without breaking the stream.
 */
int readSocioFromTXT_Robust(FILE* arch, t_socio* dest) {
    char buffer[STAM];
    if (!arch) {
        puts("File pointer is NULL");
        return 0;
    }
    if (fgets(buffer, sizeof(buffer), arch) == NULL)
        return 0;

    char* newline_pos = strchr(buffer, '\n');
    if (newline_pos == NULL) {
        int ch;
        while ((ch = fgetc(arch)) != '\n' && ch != EOF)
        
        return -1;
    }
    if (
        sscanf(buffer, "%ld|%29[^|]|%d/%d/%d", &dest->dni, dest->name,
        &dest->inscription_date.d, &dest->inscription_date.m, &dest->inscription_date.y) == 5)
        return 1;
    return -2;
}


void printSocio(const void* elem){
    printf("Socio: %ld|%s|%d/%d/%d\n",
           ((t_socio*)elem)->dni, ((t_socio*)elem)->name,
           ((t_socio*)elem)->inscription_date.d, ((t_socio*)elem)->inscription_date.m, ((t_socio*)elem)->inscription_date.y);
}

int mergeClubesAyB_Refactored(const int option) {
    FILE *archA, *archB, *archF;
    t_socio auxA, auxB, socio_a_escribir;
    t_date today = {18, 7, 2025};

    // Open files and handle errors
    archA = fopen(FILE_PATH_A, "rb");
    if (!archA) {
        perror("Error opening FILE_PATH_A");
        return 0;
    }
    archB = fopen(FILE_PATH_B, "r");
    if (!archB) {
        perror("Error opening FILE_PATH_B");
        fclose(archA);
        return 0;
    }
    archF = fopen(FILE_PATH_F, "wt");
    if (!archF) {
        perror("Error opening FILE_PATH_F");
        fclose(archA);
        fclose(archB);
        return 0;
    }
    int readA = fread(&auxA, sizeof(t_socio), 1, archA);
    int readB = readSocioFromTXT(archB, &auxB);
    while (readA && readB) {
        int res = cmpSocio(&auxA, &auxB);
        if (res < 0) { // Record A is smaller
            socio_a_escribir = auxA;
            readA = fread(&auxA, sizeof(t_socio), 1, archA);
        }
        if (res > 0) { // Record B is smaller
            socio_a_escribir = auxB;
            readB = readSocioFromTXT(archB, &auxB);
        }
        if (res == 0) { // Keys match
            socio_a_escribir = auxA; // Start with master record A's data

            // Apply conflict-resolution logic for dates (options 1 and 2)
            if (option == 1) { // Keep the newer date
                if (cmpDate(&auxB.inscription_date, &socio_a_escribir.inscription_date) > 0) {
                    socio_a_escribir.inscription_date = auxB.inscription_date;
                }
            } else if (option == 2) { // Keep the older date
                if (cmpDate(&auxB.inscription_date, &socio_a_escribir.inscription_date) < 0) {
                    socio_a_escribir.inscription_date = auxB.inscription_date;
                }
            }
            // Consume both records since they matched
            readA = fread(&auxA, sizeof(t_socio), 1, archA);
            readB = readSocioFromTXT(archB, &auxB);
        }

        // --- Centralized Transformation and Writing ---
        // Apply the global transformation for option 3, if selected.
        if (option == 3) {
            socio_a_escribir.inscription_date = today;
        }
        // Write the final, processed record.
        fprintf(archF, "%ld|%s|%d/%d/%d\n", socio_a_escribir.dni, socio_a_escribir.name,
                socio_a_escribir.inscription_date.d, socio_a_escribir.inscription_date.m, socio_a_escribir.inscription_date.y);
    }

    // --- Draining Phase ---
    while (readA) {
        socio_a_escribir = auxA;
        if (option == 3) {
            socio_a_escribir.inscription_date = today;
        }
        fprintf(archF, "%ld|%s|%d/%d/%d\n", socio_a_escribir.dni, socio_a_escribir.name,
                socio_a_escribir.inscription_date.d, socio_a_escribir.inscription_date.m, socio_a_escribir.inscription_date.y);
        readA = fread(&auxA, sizeof(t_socio), 1, archA);
    }
    while (readB) {
        socio_a_escribir = auxB;
        if (option == 3) {
            socio_a_escribir.inscription_date = today;
        }
        fprintf(archF, "%ld|%s|%d/%d/%d\n", socio_a_escribir.dni, socio_a_escribir.name,
                socio_a_escribir.inscription_date.d, socio_a_escribir.inscription_date.m, socio_a_escribir.inscription_date.y);
        readB = readSocioFromTXT(archB, &auxB);
    }

    // --- Cleanup ---
    fclose(archA);
    fclose(archB);
    fclose(archF);

    puts("Merge completed successfully.");
    return 1;
}
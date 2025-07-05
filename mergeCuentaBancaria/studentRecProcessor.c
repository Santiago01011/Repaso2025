#include "main.h"




int mergeStudentRecord(const char* filePath1, const char* filePath2, const char* filePathMerged){
    tStudent student1, student2;
    int res;
    FILE* arch1 = fopen(filePath1, "r"), *arch2 = fopen(filePath2, "r"), *archMerge = fopen(filePathMerged, "w");
    if( !arch1 )
        return 0;
    if( !arch2 ){
        fclose(arch1);
        return 0;
    }
    if( !archMerge ){
        fclose(arch1);
        fclose(arch2);
        return 0;
    }
    if (readStudentFromTXT(arch1, &student1) == 0 && !feof(arch1)) {
        fclose(arch1); fclose(arch2); fclose(archMerge);
        return 0;
    }
    if (readStudentFromTXT(arch2, &student2) == 0 && !feof(arch2)) {
        fclose(arch1); fclose(arch2); fclose(archMerge);
        return 0;
    }

    while (!feof(arch1) && !feof(arch2)) {
        res = cmpStudents(&student1, &student2);
        if (res == 0) {
            student1.grade = (student1.grade + student2.grade) / 2;
            fprintf(archMerge, "%d|%s|%.2f\n", student1.id, student1.name, student1.grade);

            // Read from *both* files when IDs match:
            if (readStudentFromTXT(arch1, &student1) == 0 && !feof(arch1)) {
                fclose(arch1); fclose(arch2); fclose(archMerge);
                return 0;
            }
            if (readStudentFromTXT(arch2, &student2) == 0 && !feof(arch2)) {
                fclose(arch1); fclose(arch2); fclose(archMerge);
                return 0;
            }
        } else if (res < 0) {
            fprintf(archMerge, "%d|%s|%.2f\n", student1.id, student1.name, student1.grade);
            if (readStudentFromTXT(arch1, &student1) == 0 && !feof(arch1)) {
                fclose(arch1); fclose(arch2); fclose(archMerge);
                return 0;
            }
        } else { // res > 0
            fprintf(archMerge, "%d|%s|%.2f\n", student2.id, student2.name, student2.grade);
            if (readStudentFromTXT(arch2, &student2) == 0 && !feof(arch2)) {
                fclose(arch1); fclose(arch2); fclose(archMerge);
                return 0;
            }
        }
    }

 // Correctly handle remaining records:
    while (1) {  // Loop until break
        if (feof(arch1)) break; // Exit loop if at end of file 1
        fprintf(archMerge, "%d|%s|%.2f\n", student1.id, student1.name, student1.grade);
        if (readStudentFromTXT(arch1, &student1) == 0 && !feof(arch1)) break; // Exit if read fails
    }

    while (1) { // Loop until break
        if (feof(arch2)) break; // Exit loop if at end of file 2
        fprintf(archMerge, "%d|%s|%.2f\n", student2.id, student2.name, student2.grade);
        if (readStudentFromTXT(arch2, &student2) == 0 && !feof(arch2)) break; // Exit if read fails
    }
    fclose(arch1);
    fclose(arch2);
    fclose(archMerge);
    return 1;
}

void promStudent(void* elem, void* elem2){
    tStudent* a = (tStudent*)elem;
    tStudent* b = (tStudent*)elem2;
    a->grade = (a->grade + b->grade) / 2;
}

int fusionarArchivosGen_ALU(const char* nombreArchivo1, const char* nombreArchivo2, const char* nombreArchivoFus, size_t tamReg, Cmp cmp){
    FILE* arch1 = fopen(nombreArchivo1, "r");
    int resultado;
    if(!arch1){
        printf("Error al abrir el archivo %s", nombreArchivo1);
        return 0;
    }
    FILE* arch2 = fopen(nombreArchivo2, "r");
    if(!arch2){
        fclose(arch1);
        printf("Error al abrir el archivo %s", nombreArchivo2);
        return 0;
    }
    FILE* archFus = fopen(nombreArchivoFus, "wb");
    if(!archFus){
        fclose(arch1);
        fclose(arch2);
        printf("Error al abrir el archivo %s", nombreArchivoFus);
        return 0;
    }
    void* elem = malloc(tamReg);
    if(!elem){
        fclose(arch1);
        fclose(arch2);
        fclose(archFus);
        printf("Error al reservar memoria para %s", nombreArchivoFus);
        return 0;
    }
    void* elem2 = malloc(tamReg);
    if(!elem2){
        fclose(arch1);
        fclose(arch2);
        fclose(archFus);
        free(elem);
        printf("Error al reservar memoria para %s", nombreArchivoFus);
        return 0;
    }

    while( readStudentFromTXT(arch1, elem) && readStudentFromTXT(arch2, elem2) ){
        resultado = cmp(elem, elem2);
        if(resultado == 0){
            promStudent(elem, elem2);
            fwrite(elem, tamReg, 1, archFus);
        }
        if(resultado < 0){
            fwrite(elem, tamReg, 1, archFus);
        }
        if(resultado > 0){
            fwrite(elem2, tamReg, 1, archFus);
        }
    }
    while( readStudentFromTXT(arch1, elem) ){
        fwrite(elem, tamReg, 1, archFus);
    }
    while( readStudentFromTXT(arch2, elem2) ){
        fwrite(elem2, tamReg, 1, archFus);

    }
    fclose(arch1);
    fclose(arch2);
    fclose(archFus);
    free(elem);
    free(elem2);
    return 1;
}



/**
* Reads a register from a text archive and returns a tStudent structure.
*/
int readStudentFromTXT(FILE* arch, tStudent* dest){
    char buffer[200];
    if (!arch){
        puts("File pointer is NULL");
        return 0;
    }
    if ( fgets(buffer, 200, arch) ){
        if( sscanf(buffer, "%d|%49[^|]|%f",
            &dest->id, dest->name,
            &dest->grade) )
                return 1;
            else
                return 0;
    }
    else
        return 0;
}

void printStudent(const void* elem){
    tStudent* auxStudent = (tStudent*)elem;
    printf("%d|%s|%.2f\n", auxStudent->id, auxStudent->name, auxStudent->grade);

}

int cmpStudents(const void* a, const void* b){
    return ((tStudent*)a)->id - ((tStudent*)b)->id;
}


int mostrarArchivoGen_ALU(const char* nombreArch, size_t tamElem, print_callback print){
    FILE* arch = fopen(nombreArch, "rb");
    if(!arch){
        printf("Error al abrir el archivo %s", nombreArch);
        return 0;
    }
    void* elem = malloc(tamElem);
    if(!elem){
        printf("Error al asignar memoria");
        fclose(arch);
        return -21;
    }
    printf("\n----------Archivo %s----------\n", nombreArch);
    while(fread(elem, tamElem, 1, arch)){
        print(elem);
    }
    puts("----------Fin de archivo----------\n");
    fclose(arch);
    free(elem);
    return 1;
}

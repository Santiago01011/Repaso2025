#include "menu.h"



int mainMenu(const char* msg){
    char opt;
    int flag = 1, dim;
    float* mat = NULL;
    do{
        puts(msg);
        fflush(stdin);
        opt = getchar();
        opt = toupper(opt);
        printf("You entered -%c\t", opt);

        switch(opt){
            case 'C':
                puts("Loading from archive...");
                loadMatFromArch("results/matrix.txt", &mat, &dim);
                break;
            case 'G':
                puts("Creating triangle results...");
                if(!printMatTriInf(&mat, dim)){
                    puts("Be sure to load the matrix before selecting this oprion");
                    break;
                }
                if(!printMatTriSup(&mat, dim))
                    break;
                if(!printMatTriR(&mat, dim))
                    break;
                if(!printMatTriL(&mat, dim))
                    break;
                if(!printMatMDiag(&mat, dim))
                    break;
                if(!printMatIDiag(&mat, dim))
                    break;
                break;
            case 'M':
                puts("Showing matrix.txt...");
                printMatFromArch("results/matrix.txt");
                break;
            case 'I':
                puts("Showing inferior triangle results...");
                if(!printMatFromArch("results/MatTriInf.txt"))
                    puts("Create the file before opening");
                break;
            case 'S':
                puts("Showing superior triangle results...");
                if(!printMatFromArch("results/MatTriSup.txt"))
                    puts("Create the file before opening");
                break;
            case 'R':
                puts("Showing right triangle results...");
                if(!printMatFromArch("results/MatTriR.txt"))
                    puts("Create the file before opening");
                break;
            case 'L':
                puts("Showing left triangle results...");
                if(!printMatFromArch("results/MatTriL.txt"))
                    puts("Create the file before opening");
                break;
            case 'D':
                puts("Showing diagonal results...");
                if(!printMatFromArch("results/MatMDiag.txt"))
                    puts("Create the file before opening");
                break;
            case 'E':
                puts("Showing inverse diagonal results...");
                if(!printMatFromArch("results/MatIDiag.txt"))
                    puts("Create the file before opening");
                break;
            case 'F':
                puts("Finishing program...");
                remove("results/MatTriInf.txt");
                remove("results/MatTriSup.txt");
                remove("results/MatTriR.txt");
                remove("results/MatTriL.txt");
                remove("results/MatMDiag.txt");
                remove("results/MatIDiag.txt");
                break;
            default:
                puts("Not supported option");
                break;

        }
    }while(opt != 'F');

    return 1;
}

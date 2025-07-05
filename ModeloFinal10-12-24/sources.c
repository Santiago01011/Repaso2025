#include "main.h"


int loadMatFromArch(const char* filePath, float** mat, int* dim){
    char buffer[100];
    char *ptrBuffer, *nextSep;
    float actual = 0;
    int row = 0, col;
    FILE* arch = fopen(filePath, "rt");
    if(!arch)
        return 0;
    fgets(buffer, sizeof(buffer), arch);
    sscanf(buffer, "%d", dim);
    if((*dim) <= 0) {
        puts("Invalid matrix dimension");
        return 0;
    }
    printf("Dimentions: %d\n", *dim);
    *mat = (float*)calloc( (*dim) * (*dim) , sizeof(float));
    if(!(*mat)) {
        puts("Error allocating memory");
        fclose(arch);
        return 0;
    }
    while(fgets(buffer, sizeof(buffer), arch) && row < (*dim)){
        ptrBuffer = buffer;
        nextSep = buffer;
        for(col = 0; col < (*dim); col++) {
            nextSep != NULL ?
                ( sscanf(ptrBuffer, "%f|", &actual)  )
            :
                ( actual = 0 ); // if the matrix is greater than values in the txt, fill with 0
            (*mat)[row * (*dim) + col] = actual;
            /*
                (*mat) dereferences mat, so we have a float* to the beginning of the mat
                (row * tam) gives the index to the start of the row
            */
            nextSep = strchr(ptrBuffer, '|');
            if(nextSep != NULL)
                ptrBuffer = nextSep + 1;
        }
        row ++;
    }
    puts("mat loaded succesfully");
    fclose(arch);
    return 1;
}

int printMatFromArch(const char* filePath){
    char buffer[100];
    FILE* arch = fopen(filePath, "r");
    if ( !arch ) {
        printf( "Error opening file: %s\n", filePath );
        return 0;
    }
    while(fgets(buffer, sizeof(buffer), arch))
        printf("%s", buffer);
    fclose(arch);
    putchar('\n');
    puts("end matrix");
    putchar('\n');
    return 1;
}

int printMat(float** mat, int dim){
    if ( !(*mat) ){
        puts("mat pointer is not a valid one");
        return 0;
    }
    puts("print matrix");
    for(int row = 0; row < dim; row++){
        putchar('\n');
        for(int col = 0; col < dim; col++){
            printf("%4.1f|", (*mat)[row * dim + col]);
        }
    }
    putchar('\n');
    putchar('\n');
    puts("end matrix");
    return 1;
}

int printMatDiagAndTri(float** mat, int dim){
    if ( !(*mat) ){
        puts("mat pointer is not a valid one");
        return 0;
    }
    puts("print matrix");
    for(int row = 0; row < dim; row++){
        putchar('\n');
        putchar('|');
        for(int col = 0; col < dim; col++)
        {
            if ( row == col )
                printf("%-6s|", "D");      // Main Diagonal
            else if ( dim - 1 - row == col )
                printf("%-6s|", "I");      // Inverse Diagonal
            else if ( row < col && (row + col) < dim )
                printf("%-6s|", "UT");     // Upper Triangle
            else if ( row > col && (row + col) >= dim )
                printf("%-6s|", "LT");     // Lower Triangle
            else if ( (row + col) < (dim - 1) )
                printf("%-6s|", "LTID"); // Left Triangle of Inverse Diagonal
            else if ( (row + col) > (dim - 1) )
                printf("%-6s|", "RTID"); // Right Triangle of Inverse Diagonal
        }
    }
    putchar('\n');
    puts("end matrix");
    putchar('\n');
    return 1;
}

int printMatTriInf(void** mat, int dim){
    if ( !(*mat) ){
        puts("mat pointer is not a valid one");
        return 0;
    }
    FILE* arch = fopen("results/MatTriInf.txt", "wb");
    if(!arch)
        return 0;
    puts("inferior triangle");
    fprintf(arch, "inferior triangle\n");
    puts("row > col && (row + col) >= dim");
    fprintf(arch, "row > col && (row + col) >= dim\n");
    for(int row = 0; row < dim; row++){
        fprintf(arch, "\n");
        fprintf(arch, "|");
        for(int col = 0; col < dim; col++)
        {
            if ( row > col && (row + col) >= dim )
                fprintf(arch, "%-6.1f|", ((float*)*mat)[row * dim + col]);
            else
                fprintf(arch, "%-6s|", " ");
        }
    }
    putchar('\n');
    fclose(arch);
    puts("end matrix");
    puts("mat writed succesfully");
    putchar('\n');
    return 1;
}

int printMatTriSup(void** mat, int dim){
    if ( !(*mat) ){
        puts("mat pointer is not a valid one");
        return 0;
    }
    FILE* arch = fopen("results/MatTriSup.txt", "wb");
    if(!arch)
        return 0;
    puts("superior triangle");
    fprintf(arch, "superior triangle\n");
    puts("row < col && (row + col) < dim - 1");
    fprintf(arch, "row < col && (row + col) < dim - 1\n");
    for(int row = 0; row < dim; row++){
        fprintf(arch, "\n");
        fprintf(arch, "|");
        for(int col = 0; col < dim; col++)
        {
            if ( row < col && (row + col) < dim - 1 )
                fprintf(arch, "%-6.1f|", ((float*)*mat)[row * dim + col]);
            else
                fprintf(arch, "%-6s|", " ");
        }
    }
    putchar('\n');
    puts("end matrix");
    putchar('\n');
    fclose(arch);
    return 1;
}

int printMatTriR(void** mat, int dim){
    if ( !(*mat) ){
        puts("mat pointer is not a valid one");
        return 0;
    }
    FILE* arch = fopen("results/MatTriR.txt", "wb");
    if(!arch)
        return 0;
    puts("right triangle");
    fprintf(arch, "right triangle\n");
    puts("row < col && (row + col) >= dim");
    fprintf(arch, "row < col && (row + col) >= dim\n");
    for(int row = 0; row < dim; row++){
        fprintf(arch, "\n");
        fprintf(arch, "|");
        for(int col = 0; col < dim; col++)
        {
            if ( row < col && (row + col) >= dim )
                fprintf(arch, "%-6.1f|", ((float*)*mat)[row * dim + col]);
            else
                fprintf(arch, "%-6s|", " ");
        }
    }
    putchar('\n');
    puts("end matrix");
    putchar('\n');
    fclose(arch);
    return 1;
}

int printMatTriL(void** mat, int dim){
    if ( !(*mat) ){
        puts("mat pointer is not a valid one");
        return 0;
    }
    FILE* arch = fopen("results/MatTriL.txt", "wb");
    if(!arch)
        return 0;
    puts("left triangle");
    fprintf(arch, "left triangle\n");
    puts("row > col && (row + col) < dim - 1");
    fprintf(arch, "row > col && (row + col) < dim - 1\n");
    for(int row = 0; row < dim; row++){
        fprintf(arch, "\n");
        fprintf(arch, "|");
        for(int col = 0; col < dim; col++)
        {
            if ( row > col && (row + col) < dim - 1 )
                fprintf(arch, "%-6.1f|", ((float*)*mat)[row * dim + col]);
            else
                fprintf(arch, "%-6s|", " ");
        }
    }
    putchar('\n');
    puts("end matrix");
    putchar('\n');
    fclose(arch);
    return 1;
}

int printMatMDiag(void** mat, int dim){
    if ( !(*mat) ){
        puts("mat pointer is not a valid one");
        return 0;
    }
    FILE* arch = fopen("results/MatMDiag.txt", "wb");
    if(!arch)
        return 0;
    puts("main diagonal");
    fprintf(arch, "main diagonal\n");
    puts("row == col");
    fprintf(arch, "row == col\n");
    for(int row = 0; row < dim; row++){
        fprintf(arch, "\n");
        fprintf(arch, "|");
        for(int col = 0; col < dim; col++)
        {
            if ( row == col )
                fprintf(arch, "%-6.1f|", ((float*)*mat)[row * dim + col]);
            else
                fprintf(arch, "%-6s|", " ");
        }
    }
    putchar('\n');
    puts("end matrix");
    putchar('\n');
    fclose(arch);
    return 1;
}

int printMatIDiag(void** mat, int dim){
    if ( !(*mat) ){
        puts("mat pointer is not a valid one");
        return 0;
    }
    FILE* arch = fopen("results/MatIDiag.txt", "wb");
    if(!arch)
        return 0;
    puts("inverse diagonal");
    fprintf(arch, "inverse diagonal\n");
    puts("row - 1 - dim == col");
    fprintf(arch, "row - 1 - dim == col\n");
    for(int row = 0; row < dim; row++){
        fprintf(arch, "\n");
        fprintf(arch, "|");
        for( int col = 0; col < dim; col++ ) {
            if ( dim - 1 - row == col )
                fprintf(arch, "%-6.1f|", ((float*)*mat)[row * dim + col]);
            else
                fprintf(arch, "%-6s|", " ");
        }
    }
    putchar('\n');
    puts("end matrix");
    putchar('\n');
    fclose(arch);
    return 1;
}


/*practice matrix
                      0  1  2  3  4  5  6  7  8
 here we finish---> 0[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    1[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    2[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    3[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    4[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    5[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    6[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    7[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    8[ ][ ][ ][ ][ ][ ][ ][ ][ ]<--- here we start ---> for each pixel [B][G][R]
*/


/* rotate left  (x, y) => (x2 = h - y1 - 1, y2 = x1) = (x2, y2) -> 90° left
        (8,8) -> (0,8) => (x = 9 - 8 - 1 , y = x) = (0,8)
        (7,7) -> (7, 1) => (x = 9 - 7 - 1, y = x) = (1, 7)
        (6,3) => (x2 = 9 - 3 - 1, y2 = 6) = (5, 6)

                      0  1  2  3  4  5  6  7  8
 here we finish---> 0[ ][ ][ ][ ][ ][ ][ ][ ][l]
                    1[ ][ ][ ][ ][ ][ ][ ][l][ ]
                    2[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    3[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    4[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    5[ ][ ][ ][ ][ ][ ][l][ ][ ]
                    6[ ][ ][ ][a][ ][ ][ ][ ][ ]
                    7[ ][ ][ ][ ][ ][ ][ ][a][ ]
                    8[ ][ ][ ][ ][ ][ ][ ][ ][a]<--- here we start ---> for each pixel [B][G][R]
*/
/* rotate right (x, y) => (x2 = y1, y2 = h - x1 - 1) = (x2, y2) -> 90° right
        (x1 = 8 , y1 = 8) => (x2 = 8, y2 = 9 - 8 - 1) = (8, 0)
        (x1 = 7 , y1 = 7) => (x2 = 7, y2 = 9 - 7 - 1) = (7, 1)
        (x1 = 6 , y1 = 3) => (x2 = 3, y2 = 9 - 6 - 1) = (3, 2)
        (x1 = 4 , y1 = 5) => (x2 = 5, y2 = 9 - 4 - 1) = (5, 4)
                      0  1  2  3  4  5  6  7  8
 here we finish---> 0[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    1[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    2[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    3[ ][ ][r][ ][ ][ ][ ][ ][ ]
                    4[ ][ ][ ][ ][ ][a][ ][ ][ ]
                    5[ ][ ][ ][ ][r][ ][ ][ ][ ]
                    6[ ][ ][ ][a][ ][ ][ ][ ][ ]
                    7[ ][r][ ][ ][ ][ ][ ][a][ ]
                    8[r][ ][ ][ ][ ][ ][ ][ ][a]<--- here we start ---> for each pixel [B][G][R]
*/

/* diagonal practice

                    a = row == col --> main diagonal
                    b = dim - 1 - row == col --> inverse diagonal
                    r = row < col --> right triangle respect the main diagonal
                    l = row > col --> left triangle respect the main diagonal
                    sum(col + row) < dim

                       0  1  2  3  4  5  6  7  8
 here we finish---> 0 [a][s][s][s][s][s][s][s][b]
                    1 [i][a][s][s][s][s][s][b][s]
                    2 [i][i][a][s][s][s][b][s][s]
                    3 [i][i][i][a][s][b][s][s][s]
                    4 [i][i][i][i][a][s][s][s][s]
                    5 [i][i][i][b][i][a][s][s][s]
                    6 [i][i][b][i][i][i][a][s][s]
                    7 [i][b][i][i][i][i][i][a][s]
                    8 [b][i][i][i][i][i][i][i][a]<--- here we start ---> float**
*/

/* diagonal practice 2
                    D = row == col                   --> Main Diagonal
                    I = dim - 1 - row == col         --> Inverse Diagonal
                    UT = row < col                   --> Upper Triangle (above Main Diagonal)
                    LT = row > col                   --> Lower Triangle (below Main Diagonal)
                    LefT = row > col && (row + col) < (dim - 1)--> Left Triangle (both diagonals)
                    RighT = row < col && (row + col) >= (dim)  --> Right Triangle (both diagonals)
                    Note: dim - 1 is linked to inverse diagonal and so it does dim to main diagonal
                      0  1  2  3  4  5  6  7  8
 here we start--->  0[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    1[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    2[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    3[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    4[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    5[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    6[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    7[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    8[ ][ ][ ][ ][ ][ ][ ][ ][ ]<--- here we finish ---> float**
*/

/*practice matrix floats alloc

    tam = 8 --> 256 bytes
    mat = (void*)malloc(tam * tam * sizeof(elem));
            [row = 1][col = 0]
                            0       1       2       3       4       5       6       7       8
    here we start---> 0[  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00]
                      1[  0.00][  1.20][  2.20][  3.50][  4.00][  0.00][  0.00][  0.00][  0.00]
                      2[  4.70][  8.90][  4.60][  7.20][  0.50][  0.00][  0.00][  0.00][  0.00]
                      3[  4.90][  9.10][  8.30][  5.50][  6.60][  0.00][  0.00][  0.00][  0.00]
                      4[  4.70][  8.90][  4.60][  7.20][  0.50][  0.00][  0.00][  0.00][  0.00]
                      5[  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00]
                      6[  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00]
                      7[  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00]
                      8[  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00][  0.00]<--- here we finish ---> for each pixel [float]
*/
























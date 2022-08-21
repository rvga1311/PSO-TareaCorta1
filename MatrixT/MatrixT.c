#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int n;
int **matrix1;
int **matrix2;
int **matrixResult;

void printMatrix(int **matrix) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void *multiplyRow(void * row){
    int *rowNumber = (int *)row;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrixResult[*rowNumber][i] += matrix1[*rowNumber][j] * matrix2[j][i];
        }
    }
}

void freeMemory(){
    for (int i = 0; i < n; i++){
        free(matrix1[i]);
        free(matrix2[i]);
        free(matrixResult[i]);
    }
    free(matrix1);
    free(matrix2);
    free(matrixResult);
}

void fillMatrix(){
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix1[i][j] = rand() % 1000;
            matrix2[i][j] = rand() % 1000;
            matrixResult [i][j] = 0;
        }
    }
}

void saveResult_Stats(int repetition, double stats){
    char filename[20];
    FILE *file;

    if (repetition < 10){
        sprintf(filename, "Matrices/Mat_0%d.txt", repetition);
    } else {
        sprintf(filename, "Matrices/Mat_%d.txt", repetition);
    }

    file = fopen(filename, "w");
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            fprintf(file, "%d ", matrixResult[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);

    file = fopen("Stats.txt", "a");
    fprintf(file, "Repeticion: %d, ", repetition);
    fprintf(file, "Duracion: %f\n", stats);
    fclose(file);
}

int main(){   
    // Cambiar seed del random
    srand(time(NULL));

    printf("Indique el valor de N: ");
    scanf("%d", &n);

    // Alocar memoria para las matrices
    matrix1 = (int**)malloc(n * sizeof(int*));
    matrix2 = (int**)malloc(n * sizeof(int*));
    matrixResult = (int**)malloc(n * sizeof(int*));

    for (int i = 0; i < n; i++){
        matrix1[i] = (int*)malloc(n * sizeof(int));
        matrix2[i] = (int*)malloc(n * sizeof(int));
        matrixResult[i] = (int*)malloc(n * sizeof(int));
    }

    double averageTime = 0;

    // Rellenar las matrices con numeros aleatorios
    fillMatrix();


    // ============================================================

    // Crear hilos
    pthread_t threadsID;

    for (int repetitions = 0; repetitions < 100; repetitions++){

        clock_t start = clock(); // Inicio de la medicion de tiempo
        for (int i = 0; i < n; i++){
            int *row = malloc(sizeof(int));
            *row = i;        
            // Crea hilo por cada fila de matriz resultante
            pthread_create(&threadsID, NULL, (void *) multiplyRow, (void *) row);
        }
        pthread_join(threadsID, NULL);

        clock_t end = clock(); // Fin de la medicion de tiempo
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC; // Tiempo de ejecucion de la multiplicacion
        averageTime += time_spent;
        // Guardar resultados en archivo
        saveResult_Stats(repetitions, time_spent);

        // Limpiar matrices
        fillMatrix();
    }    

    // ============================================================
    FILE * file = fopen("Stats.txt", "a");
    fprintf(file, "\nTiempo promedio: %f", averageTime / 100);
    fclose(file);

    // Liberar memoria
    freeMemory();


    return 0;

}

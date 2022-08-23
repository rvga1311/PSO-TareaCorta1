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
            matrix1[i][j] = rand() % 100;
            matrix2[i][j] = rand() % 100;
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
    struct timespec start, finish;
    double time_spent;


    // Crear hilos
    pthread_t threadsID[n];

    for (int repetitions = 0; repetitions < 100; repetitions++){

        clock_gettime(CLOCK_MONOTONIC, &start); // Inicio de la medicion de tiempo
        for (int i = 0; i < n; i++){
            int *row = malloc(sizeof(int));
            *row = i;        
            // Crea hilo por cada fila de matriz resultante
            pthread_create(&threadsID[i], NULL, (void *) multiplyRow, (void *) row);
        }
        
        // Esperar a que terminen los hilos
        for (int i = 0; i < n; i++){
            pthread_join(threadsID[i], NULL);
        }

        clock_gettime(CLOCK_MONOTONIC, &finish); // Fin de la medicion de tiempo
        time_spent = (finish.tv_sec - start.tv_sec);
        time_spent += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
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

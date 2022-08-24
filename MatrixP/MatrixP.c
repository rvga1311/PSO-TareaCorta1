#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>

int n;
int **matrix1;
int **matrix2;
int **matrixResult;

pid_t originalProcess;

void printMatrix(int **matrix)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void *multiplyRow(void *row)
{
    int *rowNumber = (int *)row;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            matrixResult[*rowNumber][i] += matrix1[*rowNumber][j] * matrix2[j][i];
        }
    }
}

void freeMemory()
{
    for (int i = 0; i < n; i++)
    {
        free(matrix1[i]);
        free(matrix2[i]);
        munmap(matrixResult[i], sizeof(int) * n);
    }
    free(matrix1);
    free(matrix2);
    munmap(matrixResult, sizeof(int *) * n);
}

void fillMatrix()
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            matrix1[i][j] = rand() % 100;
            matrix2[i][j] = rand() % 100;
            matrixResult[i][j] = 0;
        }
    }
}

void saveResult_Stats(int repetition, double stats)
{
    char filename[20];
    FILE *file;

    if (repetition < 10)
    {
        sprintf(filename, "Matrices/Mat_0%d.txt", repetition);
    }
    else
    {
        sprintf(filename, "Matrices/Mat_%d.txt", repetition);
    }

    file = fopen(filename, "w");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
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

int main()
{
    // Cambiar seed del random
    srand(time(NULL));
    originalProcess = getpid();

    printf("Indique el valor de N: ");
    scanf("%d", &n);

    // Alocar memoria para las matrices
    matrix1 = (int **)malloc(n * sizeof(int *));
    matrix2 = (int **)malloc(n * sizeof(int *));
    matrixResult = (int **)mmap(NULL, sizeof(int *) * (n * n), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    for (int i = 0; i < n; i++)
    {
        matrix1[i] = (int *)malloc(n * sizeof(int));
        matrix2[i] = (int *)malloc(n * sizeof(int));
        matrixResult[i] = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    }

    double averageTime = 0;

    // Rellenar las matrices con numeros aleatorios
    fillMatrix();

    // ============================================================

    struct timespec start, finish;
    double time_spent;
    for (int repetitions = 0; repetitions < 100; repetitions++)
    {

        clock_gettime(CLOCK_MONOTONIC, &start); // Inicio de la medicion de tiempo
        for (size_t i = 0; i < n; i++)
        {
            int *row = malloc(sizeof(int));
            *row = i;

            fork();

            if (getpid() == originalProcess)
            {
                continue;
            }
            else
            {
                multiplyRow(row);

                exit(EXIT_SUCCESS);
            }
        }
        while(wait(NULL) > 0);
        if (getpid() != originalProcess)
        {
            exit(EXIT_SUCCESS);
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
    FILE *file = fopen("Stats.txt", "a");
    fprintf(file, "\nTiempo promedio: %f", averageTime / 100);
    fclose(file);

    // Liberar memoria
    freeMemory();

    return 0;
}

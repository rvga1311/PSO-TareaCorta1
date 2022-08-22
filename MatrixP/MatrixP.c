#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <sys/wait.h>

int n;
int **matrix1;
int **matrix2;
int **matrixResult;
pid_t childPID;

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
        munmap(matrix1[i], sizeof(int) * n);
        munmap(matrix2[i], sizeof(int) * n);
        munmap(matrixResult[i], sizeof(int) * n);
    }
    munmap(matrix1, sizeof(int *) * n);
    munmap(matrix2, sizeof(int *) * n);
    munmap(matrixResult, sizeof(int *) * n);
}

void fillMatrix()
{
    matrix1 = (int **)mmap(NULL, sizeof(int *) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    matrix2 = (int **)mmap(NULL, sizeof(int *) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    matrixResult = (int **)mmap(NULL, sizeof(int *) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // fill the matrix with random numbers
    for (int i = 0; i < n; i++)
    {
        matrix1[i] = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        matrix2[i] = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        matrixResult[i] = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
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

    printf("Indique el valor de N: ");
    scanf("%d", &n);

    // Alocar memoria para las matrices
    matrix1 = (int **)mmap(NULL, sizeof(int *) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    matrix2 = (int **)mmap(NULL, sizeof(int *) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    matrixResult = (int **)mmap(NULL, sizeof(int *) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    for (int i = 0; i < n; i++)
    {
        matrix1[i] = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        matrix2[i] = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        matrixResult[i] = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    }

    double averageTime = 0;

    // Rellenar las matrices con numeros aleatorios
    fillMatrix();

    // ============================================================

  

    for (int repetitions = 0; repetitions < 100; repetitions++)
    {
       

        clock_t start = clock(); // Inicio de la medicion de tiempo
        for (int i = 0; i < n; i++)
        {
            int *row = malloc(sizeof(int));
            *row = i;
            // Crea Subproceso por cada fila de matriz resultante
            childPID = fork();
            if (childPID >= 0) // fork was successful
            {
                if (childPID == 0) // child process
                {
                    
                     multiplyRow(row);

                    exit(EXIT_SUCCESS);
                }
                else
                {
                    wait(NULL);
                    
                    
                }
            }
            else // fork failed
            {
                printf("\n Fork failed, quitting!!!!!!\n");
                return 1;
            }

            // Fin de Suprocesos
        }

        clock_t end = clock();                                      // Fin de la medicion de tiempo
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC; // Tiempo de ejecucion de la multiplicacion
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

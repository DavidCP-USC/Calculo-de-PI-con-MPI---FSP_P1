#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>


// Definicion de la funcion de la funcion matematica a evaluar
long double funcionIntegrar(long double x)
{
	return 1/sqrt(1 - x*x);
}

// Funcion método del trapacio
long double trapecio (long double limiteInferior, long double limiteSuperior, long long tramos)
{
    /* ---------------------------------- */
    int i;
    long double dx, area, x;
    long double fa, fb, fx;
    /* ---------------------------------- */
    
    fa = funcionIntegrar(limiteInferior);
    fb = funcionIntegrar(limiteSuperior);
    dx = (limiteSuperior - limiteInferior)/(long double)tramos;
    fx = 0.0;
    
    for (i = 1; i < tramos; i++)
    {
        x = limiteInferior + i * dx;
        fx += funcionIntegrar(x);
    }
    
    area = (fx + (fa + fb) / 2) * dx;
    
    return area;
}

void getCurrentTime(char *buffer, size_t size){
    time_t timer;
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);

    int world_rank;
    int world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Tiempos
    char timeBuffer[25];

    long long numeroTrapecios = 10000000;

    // Calculo del salto para cada proceso
    long double baseTrapecio = (2 - (1 - 0.9999999)) / numeroTrapecios;

    long double resultadoProceso;

    long double resultadoTotal = 0;

    if (world_rank != 0) {
        // Ejecuta la funcion
        for (int inicio = world_rank; inicio < world_size; inicio+=world_size) {
            long double resultado = trapecio(-1 + inicio * baseTrapecio, -1 + inicio * baseTrapecio + baseTrapecio, 10000000);
            getCurrentTime(timeBuffer, 25);
            // Imprimir tiempo
            printf("%s | Proceso %d | %Lf\n ", timeBuffer, world_rank, resultado);
            MPI_Send(&resultado, 1, MPI_LONG_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
    }


    if (world_rank == 0) {
        for (int numeroProceso = 1; numeroProceso < world_size; numeroProceso++) {
            MPI_Recv(&resultadoProceso, 1, MPI_LONG_DOUBLE , numeroProceso, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            getCurrentTime(timeBuffer, 25);
            printf("%s | Proceso %d | %Lf\n ", timeBuffer, world_rank, resultadoProceso);
            resultadoTotal += resultadoProceso;
        }
        getCurrentTime(timeBuffer, 25);
        printf("%s | Proceso %d | %Lf\n ", timeBuffer, world_rank, resultadoTotal);
    }

    MPI_Finalize();
    return 0;
}
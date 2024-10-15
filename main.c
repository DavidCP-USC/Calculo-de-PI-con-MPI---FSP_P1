#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

#define NUM_TRAPECIOS 1000000000000
#define PI_REF 3.14

double funcion(double x) {
	return 1.0 / sqrt(1.0 - x * x);
}

double area(double inferior, double superior) {

    double base = superior - inferior;

    double lim_inf = funcion(inferior);
    double lim_sup = funcion(superior);

    double diff = fabs(lim_inf - lim_sup);

    // Area triangulo
    double area = base * diff / 2.0;

    // Area trapecio
    if (lim_inf >= lim_sup) {
        area += base * lim_sup;
    } else {
        area += base * lim_inf;
    }

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

    // Buffer para almacenar el string del tiempo
    char timeBuffer[25];

    // Calculo del salto para cada proceso
    double limiteSuperior = 1.0 - DBL_EPSILON;
    double limiteInferior = -1.0 + DBL_EPSILON;
    double baseTrapecio = (double)((limiteSuperior - limiteInferior) / NUM_TRAPECIOS);

    double local = 0.0;
    double total = 0.0;

    for(double inicio = limiteInferior + (world_rank * baseTrapecio); inicio + baseTrapecio <= limiteSuperior; inicio += baseTrapecio * world_size) {

        local += area(inicio, inicio + baseTrapecio);

    }

    MPI_Reduce(&local, &total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        printf("Res: %e\n", total);
    }

    MPI_Finalize();

}
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <sys/time.h>

#define PI_REF 3.1415926535897932384626433832795028841971693993751058209749446

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

int main(int argc, char** argv) {

	int numTrapecios = atoi(argv[1]);

	struct timeval start;
	struct timeval start2;
	struct timeval end;

	// Calculo del salto para cada proceso
	double limiteSuperior = 1.0 - DBL_EPSILON;
	double limiteInferior = -1.0 + DBL_EPSILON;
	double baseTrapecio = (double)((limiteSuperior - limiteInferior) / numTrapecios);

	double local = 0.0;
	double total = 0.0;

	int world_rank;
	int world_size;

	MPI_Init(NULL, NULL);

	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	gettimeofday(&start, NULL);
	gettimeofday(&start2, NULL);

	for(double inicio = limiteInferior + (world_rank * baseTrapecio); inicio + baseTrapecio <= limiteSuperior; inicio += baseTrapecio * world_size) {

		local += area(inicio, inicio + baseTrapecio);

	}

	gettimeofday(&end, NULL);

	MPI_Reduce(&local, &total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	float overhead = (start2.tv_sec - start.tv_sec) + (start2.tv_usec - start.tv_usec) / 1e6;
	float time = (start2.tv_sec - start.tv_sec) + (start2.tv_usec - start.tv_usec) / 1e6 - overhead;

	if (world_rank == 0) {

		float diff = PI_REF - total;

		printf("'DATA_FSP_V1',%e,%e,%d,%d,%e\n", total, diff, numTrapecios, world_size, time);

	} else {

		printf("'DATA_FSP_V1',%d,%d,%d,%d,%e\n", -1, -1, numTrapecios, world_size, time);

	}

	MPI_Finalize();

	return 0;

}
#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <sys/time.h>

#define PI_REF 3.1415926535897932384626433832795028841971693993751058209749446

#define TAG 0

double funcion(double x) {
	return 1.0 / sqrt(1.0 - x * x);
}

double area(double inferior, double superior) {

	double base = superior - inferior;

	double lim_inf = funcion(inferior);
	double lim_sup = funcion(superior);

	double diff = fabs(lim_inf - lim_sup);

	double area = base * diff / 2.0;

	if (lim_inf >= lim_sup) {
		area += base * lim_sup;
	} else {
		area += base * lim_inf;
	}

	return area;

}

int main(int argc, char** argv) {

	char* endptr;

	long numTrapecios = strtol(argv[1], &endptr, 10);

	double limiteSuperior = 1.0 - (2.0 * DBL_EPSILON);
	double limiteInferior = -1.0 + (2.0 * DBL_EPSILON);
	double baseTrapecio = (limiteSuperior - limiteInferior) / numTrapecios;

	double local = 0.0;
	double total = 0.0;

	int world_rank;
	int world_size;

	struct timeval start;
	struct timeval start2;
	struct timeval end;

	MPI_Init(NULL, NULL);

	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	gettimeofday(&start, NULL);
	gettimeofday(&start2, NULL);

	for (long inicio = world_rank; inicio < numTrapecios; inicio += world_size) {

		double inicioTrapecio = limiteInferior + (inicio * baseTrapecio);
		local += area(inicioTrapecio, inicioTrapecio + baseTrapecio);

	}

	// El codigo siguiente podria adaptarse para usar MPI_Reduce
	// MPI_Reduce(&local, &total, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	// No se implementa por las restricciones del enunciado de la practica

	if (world_rank == 0) {

		total += local;

		for (int i = 1; i < world_size; i++) {

			MPI_Recv(&local, 1, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			total += local;

		}

	} else {

		MPI_Send(&local, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);

	}

	gettimeofday(&end, NULL);

	double overhead = (start2.tv_sec - start.tv_sec) + (start2.tv_usec - start.tv_usec) / 1e6;
	double time = (end.tv_sec - start2.tv_sec) + (end.tv_usec - start2.tv_usec) / 1e6 - overhead;

	if (world_rank == 0) {

		double diff = PI_REF - total;

		printf("'DATA_FSP_V1',%.15f,%.15f,%ld,%d,%.15f\n", total, diff, numTrapecios, world_size, time);

	} else {

		printf("'DATA_FSP_V1',%d,%d,%ld,%d,%.15f\n", -1, -1, numTrapecios, world_size, time);

	}

	MPI_Finalize();

	return 0;

}
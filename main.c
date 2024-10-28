#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <sys/time.h>

#define PI_REF 3.1415926535897932384626433832795028841971693993751058209749446

long double funcion(long double x) {
	return 1.0L / sqrt(1.0L - x * x);
}

long double area(long double inferior, long double superior) {

	long double base = superior - inferior;

	long double lim_inf = funcion(inferior);
	long double lim_sup = funcion(superior);

	long double diff = fabs(lim_inf - lim_sup);

	long double area = base * diff / 2.0L;

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

	long double limiteSuperior = 1.0L - (2.0L * DBL_EPSILON);
	long double limiteInferior = -1.0L + (2.0L * DBL_EPSILON);
	long double baseTrapecio = (limiteSuperior - limiteInferior) / numTrapecios;

	long double local = 0.0L;
	long double total = 0.0L;

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

		long double inicioTrapecio = limiteInferior + (inicio * baseTrapecio);
		local += area(inicioTrapecio, inicioTrapecio + baseTrapecio);

	}

	gettimeofday(&end, NULL);

	MPI_Reduce(&local, &total, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	double overhead = (start2.tv_sec - start.tv_sec) + (start2.tv_usec - start.tv_usec) / 1e6;
	double time = (end.tv_sec - start2.tv_sec) + (end.tv_usec - start2.tv_usec) / 1e6 - overhead;

	if (world_rank == 0) {

		long double diff = PI_REF - total;

		printf("'DATA_FSP_V1',%.18Lf,%.18Lf,%ld,%d,%.15f\n", total, diff, numTrapecios, world_size, time);

	} else {

		printf("'DATA_FSP_V1',%d,%d,%ld,%d,%.15f\n", -1, -1, numTrapecios, world_size, time);

	}

	MPI_Finalize();

	return 0;

}
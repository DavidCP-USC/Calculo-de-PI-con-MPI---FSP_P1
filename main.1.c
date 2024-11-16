#include <mpi.h>      // Biblioteca MPI para comunicación paralela
#include <unistd.h>   // Para funciones relacionadas con el sistema operativo
#include <stdio.h>    // Para entrada y salida estándar
#include <stdlib.h>   // Para conversión de datos y gestión de memoria
#include <math.h>     // Biblioteca matemática para cálculos como sqrt
#include <float.h>    // Define límites de precisión para tipos de datos de punto flotante
#include <time.h>     // Biblioteca para manejo del tiempo
#include <sys/time.h> // Para medir tiempos de ejecución de alta precisión

// Definición de una constante precisa del valor de π (para calcular el error)
#define PI_REF 3.1415926535897932384626433832795028841971693993751058209749446

// Etiqueta para comunicaciones MPI
#define TAG 0

// Función matemática que calcula f(x) = 1 / sqrt(1 - x^2)
double funcion(double x) {
	return 1.0 / sqrt(1.0 - x * x);
}

// Función que calcula el área de un trapecio dados sus límites inferior y superior
double area(double inferior, double superior) {

	// Calcula la base del trapecio
	double base = superior - inferior;

	// Evalúa la función en los límites
	double lim_inf = funcion(inferior);
	double lim_sup = funcion(superior);

	// Diferencia absoluta entre los valores en los límites
	double diff = fabs(lim_inf - lim_sup);

	// Calcula el área del triángulo formado por la diferencia
	double area = base * diff / 2.0;

	// Ajusta el área dependiendo de cuál de los valores límite es mayor
	if (lim_inf >= lim_sup) {
		area += base * lim_sup;
	} else {
		area += base * lim_inf;
	}

	return area; // Devuelve el área calculada
}

int main(int argc, char** argv) {

	// Puntero para almacenar errores en la conversión de argumentos
	char* endptr;

	// Convierte el número de trapecios recibido como argumento
	long numTrapecios = strtol(argv[1], &endptr, 10);

	// Define los límites del intervalo de integración
	double limiteSuperior = 1.0 - (2.0 * DBL_EPSILON); // Ajuste para evitar problemas de precisión
	double limiteInferior = -1.0 + (2.0 * DBL_EPSILON);
	double baseTrapecio = (limiteSuperior - limiteInferior) / numTrapecios; // Ancho de cada trapecio

	// Variables para almacenar los cálculos locales y totales
	double local = 0.0;
	double total = 0.0;

	// Variables para identificar el rango del proceso y el tamaño del grupo
	int world_rank;
	int world_size;

	// Variables para medir tiempos de ejecución
	struct timeval start;
	struct timeval start2;
	struct timeval end;

	// Inicializa MPI
	MPI_Init(NULL, NULL);

	// Obtiene el rango del proceso actual
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	// Obtiene el número total de procesos
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// Marca el inicio de la medición del tiempo
	gettimeofday(&start, NULL);
	gettimeofday(&start2, NULL);

	// Ciclo principal: divide los trapecios entre procesos de forma cíclica
	for (long inicio = world_rank; inicio < numTrapecios; inicio += world_size) {

		// Calcula los límites del trapecio actual
		double inicioTrapecio = limiteInferior + (inicio * baseTrapecio);
		// Suma el área calculada al acumulador local
		local += area(inicioTrapecio, inicioTrapecio + baseTrapecio);

	}

	// Nota: El siguiente bloque podría usar `MPI_Reduce` para optimizar
	// MPI_Reduce(&local, &total, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	// Sin embargo, no se utiliza debido a las restricciones del enunciado.

	if (world_rank == 0) { // Si el proceso es el maestro (rank 0)

		// Suma los resultados locales al total
		total += local;

		// Recibe los resultados locales de los procesos restantes
		for (int i = 1; i < world_size; i++) {
			MPI_Recv(&local, 1, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			total += local;
		}

	} else { // Si el proceso no es el maestro

		// Envía los resultados locales al proceso maestro
		MPI_Send(&local, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);

	}

	// Marca el fin de la medición del tiempo
	gettimeofday(&end, NULL);

	// Calcula el tiempo de sobrecarga antes del inicio real del cálculo
	double overhead = (start2.tv_sec - start.tv_sec) + (start2.tv_usec - start.tv_usec) / 1e6;
	// Calcula el tiempo total del cálculo excluyendo la sobrecarga
	double time = (end.tv_sec - start2.tv_sec) + (end.tv_usec - start2.tv_usec) / 1e6 - overhead;

	if (world_rank == 0) { // Si el proceso es el maestro

		// Calcula la diferencia entre el valor de referencia de π y el calculado
		double diff = PI_REF - total;

		// Imprime los resultados en formato CSV
		printf("'DATA_FSP_V1',%.15f,%.15f,%ld,%d,%.15f\n", total, diff, numTrapecios, world_size, time);

	} else { // Para otros procesos

		// Imprime valores de proceso no maestro en formato CSV (indicando que no calcula π)
		printf("'DATA_FSP_V1',%d,%d,%ld,%d,%.15f\n", -1, -1, numTrapecios, world_size, time);

	}

	// Finaliza MPI
	MPI_Finalize();

	return 0;
}

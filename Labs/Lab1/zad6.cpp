/* 
  ┌─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
  │ Napisati MPI program koji realizuje množenje matrica A i B reda n, čime se dobija                               │
  │ rezultujuća matrica C. Nakon toga, u matrici C pronaći maksimalnu vrednost elemenata                            │
  │ svake kolone. Množenje se obavlja tako što master proces šalje svakom procesu radniku                           │
  │ po jednu vrstu prve matrice i celu drugu matricu. Svaki proces računa po jednu vrstu                            │
  │ rezultujuće matrice i šalje master procesu. Svi procesi učestvuju u izračunavanju.                              │
  │ Štampati dobijenu matricu kao i maksimalne vrednosti elemenata svake kolone. Zadatak                            │
  │ rešiti korišćenjem grupnih operacija.                                                                           │
  └─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MASTER_RANK 0

int main(int argc, char** argv) {
    int n = 6; // red matrica
    int size, rank;
    int *A, *B, *C, *local_A, *local_C;
    int max_vals[n]; // niz koji sadrži maksimalne vrednosti elemenata svake kolone

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size < 2) {
        printf("Potrebno je najmanje 2 procesa.\n");
        MPI_Finalize();
        return 1;
    }

    int rows_per_proc = n / size;

    if (n % size != 0) {
        printf("Broj redova matrica nije deljiv brojem procesa.\n");
        MPI_Finalize();
        return 1;
    }

    // Inicijalizacija matrica
    if (rank == MASTER_RANK) {
        A = (int *) malloc(n * n * sizeof(int));
        B = (int *) malloc(n * n * sizeof(int));
        C = (int *) malloc(n * n * sizeof(int));
        for (int i = 0; i < n * n; i++) {
            A[i] = i;
            B[i] = i;
        }
    }

    // Pravljenje izvedenog tipa za slanje jedne vrste matrice A i cele matrice B
    MPI_Datatype rowtype, column;
    MPI_Type_vector(n, 1, n, MPI_INT, &column);
    MPI_Type_create_resized(column, 0, sizeof(int), &rowtype);
    MPI_Type_commit(&rowtype);
    MPI_Type_free(&column);

    // Slanje matrice A i delova matrice B svakom procesu
    local_A = (int *) malloc(n * sizeof(int));
    MPI_Scatter(A, n * rows_per_proc, MPI_INT, local_A, n * rows_per_proc, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
    MPI_Bcast(B, n * n, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);

    // Množenje matrica
    local_C = (int *) malloc(n * rows_per_proc * sizeof(int));
    for (int i = 0; i < rows_per_proc; i++) {
        for (int j = 0; j < n; j++) {
            int sum = 0;
            for (int k = 0; k < n; k++) {
                sum += local_A[i * n + k] * B[k * n + j];
            }
            local_C[i * n + j] = sum;
        }
    }

    // Slanje delova matrice C master procesu
    MPI_Gather(local_C, n * rows_per_proc, MPI_INT, C, n * rows_per_proc, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);

    // Pronalaženje maksimalnih vrednosti po kolonama
    for (int i = 0; i < n; i++) {
        int max_val = C[i];
        for (int j = 1; j < n / rows_per_proc; j++) {
            if (C[j * n + i] > max_val) {
                max_val = C[j * n + i];
            }

/* 
  ┌─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
  │ Proces 0 kreira matricu reda n i šalje i-om procesu po dve kvazidijagonale matrice, obe                         │
  │ na udaljenosti i od glavne dijagonale. Proces i kreira svoju matricu tako što smešta                            │
  │ primljene dijagonale u prvu i drugu kolonu matrice a ostala mesta popunjava nulama.                             │
  │ Napisati MPI program koji realizuje opisanu komunikaciju korišćenjem izvedenih tipova                           │
  │ podataka i prikazuje vrednosti odgovarajućih kolona.                                                            │
  └─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
 */

#include <stdio.h>
#include <mpi.h>

#define n 8

int main(int argc, char** argv) {
    int rank, size;
    MPI_Status status;
    MPI_Datatype diagtype;

    int mat[n][n];
    int recvbuf[n];

    // Inicijalizacija MPI okruzenja
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Inicijalizacija izvedenog tipa podataka za slanje dijagonalnih elemenata
    MPI_Type_vector(n, 1, n+1, MPI_INT, &diagtype);
    MPI_Type_commit(&diagtype);

    // Proces 0 inicijalizuje matricu i salje dijagonale procesima
    if (rank == 0) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                mat[i][j] = i*n + j;
            }
        }

        for (int i = 1; i < size; i++) {
            MPI_Send(&mat[i][i-1], 2, diagtype, i, 0, MPI_COMM_WORLD);
        }
    }

    // Svaki proces prima dijagonale i smesta ih u matricu
    MPI_Recv(recvbuf, n, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    for (int i = 0; i < n; i++) {
        mat[i][0] = (i - rank) % 2 == 0 ? recvbuf[i] : 0;
    }

    MPI_Recv(recvbuf, n, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    for (int i = 0; i < n; i++) {
        mat[i][1] = (i - rank) % 2 == 0 ? recvbuf[i] : 0;
    }

    // Prikaz vrednosti odgovarajucih kolona
    for (int i = 0; i < size; i++) {
        if (i == rank) {
            printf("Proces %d, kolone 1 i 2:\n", rank);
            for (int j = 0; j < n; j++) {
                printf("%d %d\n", mat[j][0], mat[j][1]);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Oslobadjanje zauzetih resursa i zavrsavanje programa
    MPI_Type_free(&diagtype);
    MPI_Finalize();
    return 0;
}

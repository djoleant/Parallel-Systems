/* 
  ┌─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
  │ Napisati MPI program koji pronalazi minimalnu vrednost u delu matrice reda n (n-parno)                          │
  │ koga čine kolone matrice sa parnim indeksom (j=0,2,4,...). Matrica je inicijalizovana u                         │
  │ master procesu (P0). Svaki proces treba da dobije elemente kolona sa parnim indeksom iz                         │
  │ odgovarajućih n/p vrsta (p-broj procesa, n deljivo sa p) i nađe lokalni minimum. Na taj                         │
  │ način, P0 dobija elemente kolona sa parnim indeksom iz prvih n/p vrsta i nalazi lokalni                         │
  │ minimum, P1 dobija elemente kolona sa parnim indeksom iz sledećih n/p vrsta i nalazi                            │
  │ lokalni minimum itd. Nakon toga, u master procesu se izračunava i na ekranu prikazuje                           │
  │ globalni minimum u traženom delu matrice. Zadatak realizovati korišćenjem isključivo                            │
  │ grupnih operacija i izvedenih tipova podataka.                                                                  │
  └─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define n 8 // Red matrice (n-paran)

int main(int argc, char** argv) {
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int np = size; // Broj procesa u kojima će biti podeljena matrica

    if (n % np != 0) {
        printf("Broj procesa mora biti deljiv sa redom matrice.");
        MPI_Finalize();
        return 0;
    }

    int local_n = n / np; // Broj redova u svakoj podmatrici
    int local_m = n / 2; // Broj kolona u svakoj podmatrici (samo sa parnim indeksom)
    int local_size = local_n * local_m; // Broj elemenata u svakoj podmatrici

    int A[n][n];
    int* local_A = (int*)malloc(local_size * sizeof(int));

    if (rank == 0) { // Inicijalizacija matrice u master procesu (P0)
        printf("Matrica A:\n");
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i][j] = rand() % 10;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
    }

    // Podela matrice na podmatrice koje će biti procesuirane od strane pojedinačnih procesa
    MPI_Datatype column;
    MPI_Type_vector(n, 1, 2, MPI_INT, &column);
    MPI_Type_commit(&column);
    MPI_Scatter(A, local_n, column, local_A, local_n, column, 0, MPI_COMM_WORLD);

    int local_min;
    MPI_Reduce_scatter_block(local_A, &local_min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

    int global_min;
    MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    //if (
        //TODO: complete do kraja
/* 
  ┌─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
  │ Napisati MPI program koji pronalazi maksimalnu vrednost u delu matrice reda n (nparno) koga čine vrste matrice  │
  │ sa parnim indeksom (i=0,2,4,...). Matrica je                                                                    │
  │ inicijalizovana                                                                                                 │
  │ u master procesu (P0). Svaki proces treba da dobije elemente vrsta sa parnim indeksom iz                        │
  │ odgovarajućih n/p kolona (p-broj procesa, n deljivo sa p) i nađe lokalni maksimum. Na                           │
  │ taj način, P0 dobija elemente vrsta sa parnim indeksom iz prvih n/p kolona i nalazi                             │
  │ lokalni maksimum, P1 dobija elemente kolona sa parnim indeksom iz sledećih n/p kolona                           │
  │ i nalazi lokalni maksimum itd. Nakon toga, u master procesu se izračunava i na ekranu                           │
  │ prikazuje globalni maksimum u traženom delu matrice. Zadatak realizovati korišćenjem                            │
  │ isključivo grupnih operacija i izvedenih tipova podataka.                                                       │
  └─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 10

int main(int argc, char **argv)
{
    int rank, size;
    int n = N;
    int mat[n][n];
    int i, j, k;
    int local_max, global_max;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Inicijalizacija matrice u master procesu
    if (rank == 0)
    {
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
            {
                mat[i][j] = rand() % 100;
            }
        }
    }

    // Slanje delova matrice sa parnim indeksom vrsta odgovarajucim procesima
    int sendcounts[size];
    int displs[size];
    for (i = 0; i < size; i++)
    {
        int startrow = i * n / size;
        int endrow = (i + 1) * n / size;
        int rowcount = endrow - startrow;
        sendcounts[i] = rowcount * n / 2;
        displs[i] = startrow * n / 2;
    }
    int local_mat[n / size][n / 2];
    MPI_Scatterv(mat, sendcounts, displs, MPI_INT, local_mat, sendcounts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    // Pronalazenje lokalnog maksimuma
    local_max = local_mat[0][0];
    for (i = 0; i < n / size; i++)
    {
        for (j = 0; j < n / 2; j++)
        {
            if (local_mat[i][j] > local_max)
            {
                local_max = local_mat[i][j];
            }
        }
    }

    // Slanje lokalnih maksimuma master procesu
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    // Prikaz globalnog maksimuma u master procesu
    if (rank == 0)
    {
        printf("Globalni maksimum je %d\n", global_max);
    }

    MPI_Finalize();
    return 0;
}

/*
  ┌─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
  │ Proces 0 kreira matricu reda n i šalje i-om procesu po dve kvazidijagonale matrice, obe                         │
  │ na udaljenosti i od sporedne dijagonale. Proces i kreira svoju matricu tako što smešta                          │
  │ primljene dijagonale u prvu i drugu vrstu matrice a ostala mesta popunjava nulama.                              │
  │ Napisati MPI program koji realizuje opisanu komunikaciju korišćenjem izvedenih tipova                           │
  │ podataka i prikazuje vrednosti odgovarajućih kolona.                                                            │
  └─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
*/

#include <mpi.h>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int n = 8;
    const int num_diagonals = 2;

    // kreiranje matrice na procesu 0
    int matrix[n][n];
    if (rank == 0) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                matrix[i][j] = i*n + j;
            }
        }
    }

    // kreiranje izvedenih tipova podataka za slanje kvazi-dijagonala
    MPI_Datatype column_type, row_type;
    MPI_Type_vector(n, 1, n+1, MPI_INT, &column_type);
    MPI_Type_commit(&column_type);
    MPI_Type_vector(n, 1, n-1, MPI_INT, &row_type);
    MPI_Type_commit(&row_type);

    // slanje kvazi-dijagonala od procesa 0 do ostalih procesa
    vector<int> diagonals[num_diagonals];
    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            int start_index = i - n + 1;
            if (start_index < 0) {
                start_index = 0;
            }
            int end_index = i + n - 1;
            if (end_index > 2*n-2) {
                end_index = 2*n-2;
            }
            int num_elements = end_index - start_index + 1;
            diagonals[0].resize(num_elements);
            diagonals[1].resize(num_elements);
            for (int j = start_index; j <= end_index; j++) {
                int row = j - i + n - 1;
                if (j % 2 == 0) {
                    diagonals[0][j-start_index] = matrix[row][j];
                } else {
                    diagonals[1][j-start_index] = matrix[row][j];
                }
            }
            MPI_Send(diagonals[0].data(), 1, column_type, i, 0, MPI_COMM_WORLD);
            MPI_Send(diagonals[1].data(), 1, row_type, i, 1, MPI_COMM_WORLD);
        }
    } else {
        diagonals[0].resize(n);
        diagonals[1].resize(n);
        MPI_Recv(diagonals[0].data(), n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(diagonals[1].data(), n, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // ispisivanje primljenih kvazi-dijagonala
        cout << "Rank " << rank << ":" << endl;
        cout << "First diagonal: ";
        for (int i = 0; i < n; i++) {
            cout << diagonals[0][i] << " ";
        }
        cout << endl;
        cout << "Second diagonal: ";
        for (int i = 0; i < n; i++) {
            cout << diagonals[1][i] << " ";
       

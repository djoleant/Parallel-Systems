/* 
  ┌─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
  │ Napisati MPI program koji realizuje množenje matrice Amxn i matrice Bnxk, čime se                               │
  │ dobija rezultujuća matrica Cmxk. Množenje se obavlja tako što master proces šalje svakom                        │
  │ procesu celu matricu A i po k/p kolona matrice B (p-broj procesa, k je deljivo sa p). Svi                       │
  │ procesi učestvuju u izračunavanju. Konačni rezultat množenja se nalazi u master procesu                         │
  │ koji ga i prikazuje. Predvideti da se slanje k/p kolona matrice B svakom procesu obavlja                        │
  │ odjednom i to direktno iz matrice B. Zadatak rešiti korišćenjem grupnih operacija i                             │
  │ izvedenih tipova podataka.                                                                                      │
  └─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
 */

#include <iostream>
#include <mpi.h>
#include <vector>
#include <cstring>

using namespace std;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int world_rank;
    int world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    const int A_ROWS = 4;
    const int A_COLS = 3;
    const int B_COLS = 2;

    // Inicijalizacija matrica u master procesu
    double A[A_ROWS][A_COLS] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9},
        {10, 11, 12}
    };
    double B[A_COLS][B_COLS] = {
        {13, 14},
        {15, 16},
        {17, 18}
    };
    double C[A_ROWS][B_COLS];

    // Podela kolona matrice B među procesima
    const int cols_per_proc = B_COLS / world_size;
    const int B_block_size = cols_per_proc * A_COLS;
    double B_block[B_COLS][cols_per_proc];
    MPI_Scatter(B, B_block_size, MPI_DOUBLE, B_block, B_block_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Slanje matrice A svakom procesu
    MPI_Bcast(A, A_ROWS * A_COLS, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Izračunavanje delova matrice C u svakom procesu
    double C_block[A_ROWS][cols_per_proc];
    memset(C_block, 0, sizeof(C_block));
    for (int i = 0; i < A_ROWS; i++) {
        for (int j = 0; j < cols_per_proc; j++) {
            for (int k = 0; k < A_COLS; k++) {
                C_block[i][j] += A[i][k] * B_block[k][j];
            }
        }
    }

    // Sakupljanje delova matrice C u master procesu
    MPI_Gather(C_block, A_ROWS * cols_per_proc, MPI_DOUBLE, C, A_ROWS * cols_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Prikazivanje rezultujuće matrice C u master procesu
    if (world_rank == 0) {
        cout << "C = " << endl;
        for (int i = 0; i < A_ROWS; i++) {
            for (int j = 0; j < B_COLS; j++) {
                cout << C[i][j] << " ";
            }
            cout << endl;
        }
    }

    MPI_Finalize();
    return 0;
}

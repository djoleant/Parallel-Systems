#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size, n;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        cout << "Unesite velicinu matrice n: ";
        cin >> n;

        // inicijalizacija matrice
        int A[n][n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i][j] = rand() % 100;
            }
        }

        // slanje podataka svakom procesu
        for (int i = 1; i < size; i++) {
            MPI_Datatype kvazidiagonal;
            MPI_Type_vector(n, 1, n + 1, MPI_INT, &kvazidiagonal);
            MPI_Type_commit(&kvazidiagonal);

            MPI_Send(&A[0][n - 1 - i], 1, kvazidiagonal, i, 0, MPI_COMM_WORLD);

            MPI_Datatype kvazidiagonal2;
            MPI_Type_vector(n, 1, n - 1, MPI_INT, &kvazidiagonal2);
            MPI_Type_commit(&kvazidiagonal2);

            MPI_Send(&A[i][1], 1, kvazidiagonal2, i, 0, MPI_COMM_WORLD);

            MPI_Type_free(&kvazidiagonal);
            MPI_Type_free(&kvazidiagonal2);
        }

        // ispis matrice
        cout << "Matrica:\n";
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << A[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
    else {
        // primanje podataka
        int A[n][n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i][j] = 0;
            }
        }

        MPI_Datatype kvazidiagonal;
        MPI_Type_vector(n, 1, n + 1, MPI_INT, &kvazidiagonal);
        MPI_Type_commit(&kvazidiagonal);

        MPI_Recv(&A[0][rank], 1, kvazidiagonal, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Datatype kvazidiagonal2;
        MPI_Type_vector(n, 1, n - 1, MPI_INT, &kvazidiagonal2);
        MPI_Type_commit(&kvazidiagonal2);

        MPI_Recv(&A[rank][1], 1, kvazidiagonal2, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Type_free(&kvazidiagonal);
        MPI_Type_free(&kvazidiagonal2);

        // ispis kolona
        cout << "Proces " << rank << " - kolona 0:\n";
        //for (int
    //TODO: DOVRSI ISPIS KOLONA
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
#include <iostream>
#include <cstdlib>
#include <ctime>

#define N 6 // red matrica
#define M 4 // kolona matrica

using namespace std;

int main(int argc, char** argv) {
    int rank, size;
    int matA[N][M], matB[M][N], matC[N][N], rowC[N];
    
    srand(time(NULL));
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        // popunjavanje matrica A i B
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                matA[i][j] = rand() % 10;
            }
        }
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                matB[i][j] = rand() % 10;
            }
        }
        
        // slanje vrsti matrice A svakom procesu
        for (int i = 1; i < size; i++) {
            MPI_Send(&matA[i-1], M, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        
        // slanje cele matrice B svakom procesu
        for (int i = 1; i < size; i++) {
            MPI_Send(&matB[0], M*N, MPI_INT, i, 1, MPI_COMM_WORLD);
        }
        
        // master proces racuna svoju vrstu matrice C
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matC[i][j] = 0;
                for (int k = 0; k < M; k++) {
                    matC[i][j] += matA[i][k] * matB[k][j];
                }
            }
        }
        
        // prikaz matrica A i B
        cout << "Matrica A:" << endl;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                cout << matA[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
        cout << "Matrica B:" << endl;
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                cout << matB[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
        
        // prikaz matrice C
        cout << "Matrica C:" << endl;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cout << matC[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
        
        // prikaz maksimalnih vrednosti po kolonama
        cout << "Maksimalne vrednosti po kolonama:" << endl;
        //for (int j = 0; j < N; j++) {
            //int maxVal = 0;
            //for (int i

//TODO: dovrsi!!! i proveri!!!
'''
  ┌─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
  │ Proces 0 kreira matricu reda n i šalje i-om procesu po dve kvazidijagonale matrice, obe                         │
  │ na udaljenosti i od sporedne dijagonale. Proces i kreira svoju matricu tako što smešta                          │
  │ primljene dijagonale u prvu i drugu vrstu matrice a ostala mesta popunjava nulama.                              │
  │ Napisati MPI program koji realizuje opisanu komunikaciju korišćenjem izvedenih tipova                           │
  │ podataka i prikazuje vrednosti odgovarajućih kolona.                                                            │
  └─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
'''

from mpi4py import MPI
import numpy as np

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

n = 6  # red matrice
k = 2  # udaljenost dijagonala od sporedne dijagonale

# Kreiranje izvedenog tipa podataka za dijagonale matrice
diag_type = MPI.Type_vector(n - k, 1, n + 1)
diag_type.Commit()

# Proces 0 kreira matricu i šalje dijagonale ostalim procesima
if rank == 0:
    # Kreiranje matrice
    mat = np.zeros((n, n))
    for i in range(n - k):
        mat[i+k, i] = 1.0
        mat[i, i+k] = 2.0
    
    # Slanje dijagonala ostalim procesima
    for i in range(1, size):
        comm.Send([mat[i+k:, i], 1, diag_type], dest=i, tag=0)
        comm.Send([mat[i:, i+k], 1, diag_type], dest=i, tag=1)

# Ostali procesi primaju dijagonale i kreiraju svoje matrice
else:
    # Kreiranje prazne matrice
    mat = np.zeros((n, n))

    # Primanje dijagonala od procesa 0
    recv_buf = np.zeros(n - k, dtype=np.float64)
    comm.Recv([recv_buf, 1, diag_type], source=0, tag=0)
    mat[k:, 0] = recv_buf
    
    recv_buf = np.zeros(n - k, dtype=np.float64)
    comm.Recv([recv_buf, 1, diag_type], source=0, tag=1)
    mat[0, k:] = recv_buf
    
    # Ispisivanje odgovarajućih vrsta
    if rank == 1:
        print("Vrsta 0: ", mat[0,:])
    elif rank == 2:
        print("Vrsta 1: ", mat[1,:])
        
diag_type.Free()

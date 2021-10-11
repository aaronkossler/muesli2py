import muesli
from mpi4py import MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank()

print("Starting Muesli Library ... ")
muesli.start_timer()
muesli.fib(30)
muesli.test_openmp()
muesli.test_mpi();
print(muesli.version())
# print("took: " + str(muesli.end_timer()/1000) + " miliseconds\n")

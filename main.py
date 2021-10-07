import muesli
from mpi4py import MPI

#comm = MPI.COMM_WORLD
#rank = comm.Get_rank()

#print("Hello, World! \nI am process " + str(rank) + "\n")
print("Starting Muesli Library")
muesli.startTimer()
muesli.fib(50)
print(muesli.version())
print("took: " + str(muesli.endTimer()/1000) + " miliseconds\n")


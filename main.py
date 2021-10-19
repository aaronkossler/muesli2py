from muesli import muesli_utils
from mpi4py import MPI
import builtins as __builtin__

comm = MPI.COMM_WORLD
rank = comm.Get_rank()

print("Starting Muesli Library ... ")
muesli_utils.start_timer()
#muesli.fib(30)
#muesli.test_openmp()
#muesli.test_mpi()
#print(muesli.version())
#da.printSize()
# print("took: " + str(muesli.end_timer()/1000) + " miliseconds\n")

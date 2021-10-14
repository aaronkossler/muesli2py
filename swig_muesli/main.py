from muesli.muesli_utils import muesli_utils
from muesli.da import da
from mpi4py import MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank()

print("Starting Muesli Library ... ")
print(muesli_utils.start_timer())
print(muesli_utils.fib(50))
muesli_utils.test_openmp()
muesli_utils.test_mpi()
#print(muesli.version())
myda = da.dista()
myda.setSize(5)
print(myda.getSize())
print("took: " + str(muesli_utils.end_timer() / 1000) + " miliseconds\n")

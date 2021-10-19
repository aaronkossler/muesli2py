from muesli.muesli_utils import muesli_utils
from muesli.da import da
from mpi4py import MPI
import numpy as np

comm = MPI.COMM_WORLD
rank = comm.Get_rank()

print("Starting Muesli Library ... ")
print(muesli_utils.start_timer())
print(muesli_utils.fib(50))
muesli_utils.test_openmp()
muesli_utils.test_mpi()
myda = da.da(5)
mydada = da.da(8, 7)
myda.fill(6)
myda.printarray()
mydada.printarray()
print("took: " + str(muesli_utils.end_timer() / 1000) + " miliseconds\n")

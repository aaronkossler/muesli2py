import numpy as np
from build.da import *
from build.dm import *
# from build.muesli import *
# from build.test import *


def square(i):
    return i*i


def mult(i, j):
    return i*j

bruh = Pyda(5, 3)
bruh.printarray()
bruh.setElements(np.array([1,3,5,7,9]))
bruh.printarray()
top = bruh.mapIndex(mult, 2)
bruh.printarray()
top.printarray()


# def test(i):
#     return i * 25
#
#
# def two(i, j):
#     return i*j
#
# mate = Pydm(3,3,3)
# mate.printmatrix()
# mate.setElements(np.array([1,2,3,4,5,6,7,8,9]))
# mate.printmatrix()
# sup = mate.mapIndex(two, 2, 1)
# # mate.mapDimensionsInPlace(two, 2, 1)
# mate.printmatrix()
# sup.printmatrix()

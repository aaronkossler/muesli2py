import numpy as np
from build.da import *
from build.dm import *
# from build.muesli import *
# from build.test import *


# def square(i):
#     return i * i
#
#
# bruh = Pyda(5, 3)
# bruh.printarray()
# bruh.map(square)
# bruh.printarray()


def test(i):
    return i * 25


mate = Pydm(3,3,3)
mate.printmatrix()
# mate.setElements(np.array([1,2,3,4,5,6,7,8,9]))
# mate.printmatrix()
sup = mate.map(test)
mate.printmatrix()
sup.printmatrix()

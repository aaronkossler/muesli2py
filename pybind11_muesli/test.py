from build.da import *
from build.dm import *
from build.muesli import *


def square(i):
    return i * i


bruh = Pyda(5, 3)
bruh.printarray()
bruh.map(square)
bruh.printarray()


def test(i):
    return i * 25


mate = Pydm(5,3,3)
mate.printmatrix()
mate.map(test)
mate.printmatrix()
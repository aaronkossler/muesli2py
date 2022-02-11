import numpy as np
import time
from build.muesli import *


initSkeletons(False)


def hest(row, col, value):
    return row * col * value


def mest(index, value):
    return index * value


dm = intDM(4, 4, 3)
test = dm.mapIndex(mest)
test.show()
# lol = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16])
# test.setMatrix(lol)
# test.show()

t = test.gather()
if isRootProcess():
    print(t)
    print(t.shape)

terminateSkeletons()

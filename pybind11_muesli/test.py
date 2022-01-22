import numpy as np
import time
from build.muesli import *
# from build.test import *

# time.sleep(10)

initSkeletons(False)
#setNumRuns(4)
#
# def two(i, j):
#     return i*j
#
mate = Pydm(10, 10, 5)
# mate.show()


def test(i):
    return i * i


mate.show()
mate.mapInPlace(test)
mate.show()
terminateSkeletons()

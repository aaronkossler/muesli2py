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


class Example:
    def __init__(self):
        self.x = 10
        self.y = 20

    def test(self, i, j):
        return i * j * self.x * self.y


mate.show()
e = Example()
mate.mapIndexInPlace(e.test)
mate.show()
terminateSkeletons()

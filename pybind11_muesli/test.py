import numpy as np
import time
from build.muesli import *
# from build.test import *

# time.sleep(10)

initSkeletons(False)


def test(row, col, value):
    return row * col * value


def mest(index, value):
    return index * value


dm = intDM(6, 6, 2)
test = dm.mapIndex(test)
test.show()

terminateSkeletons()

import numpy as np
from build.muesli import *

initSkeletons(False)

if isRootProcess():
    print("Testing DAs...")
none = intDA()
one = intDA(10)
two = intDA(10, 2)

array = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
one.setArray(array)

print("Element at Index 8: " + str(one.get(8)))

two.set(2, 9)
one.setLocal(3, 8)


print("Size: " + str(two.getSize()))
print("Local Size: " + str(two.getLocalSize()))
print("First Index: " + str(two.getFirstIndex()))
if two.isLocal(9):
    print("This should only appear once")
print("Local Element at Index 3: " + str(one.getLocal(3)))


def test(i):
    return i*10


def itest(i, j):
    return i*j


two.showLocal()
three = one.map(test)
four = two.mapIndex(itest)
one.mapInPlace(test)
two.mapIndexInPlace(itest)
one.show()
two.show()
three.show()
four.show()

five = one.gather()
print(five)

terminateSkeletons()

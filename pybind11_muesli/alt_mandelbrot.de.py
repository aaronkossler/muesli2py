import sys
import time
from io import StringIO
import shutil
import numpy as np
import array
from build.muesli import *


def ppm(width, height, maxval, image):
    ppm_header = f'P6 {width} {height} {maxval}\n'
    with open('mandelbrot.ppm', 'wb') as f:
        f.write(bytearray(ppm_header, 'ascii'))
        image.tofile(f)


def convert(dm):
    rows = dm.getRows()
    cols = dm.getCols()
    image = array.array('B', [0] * 3 * cols * rows)
    index = 0

    for row in range(0, rows):
        for col in range(0, cols):
            p = dm.get(row * cols + col)
            image[index] = p.r
            image[index+1] = p.g
            image[index+2] = p.b
            index += 3

    return image


class Iterate:
    def __init__(self, iter, cx, cy, z, rows, cols):
        self.iter = iter
        self.zoom = z
        self.center_x = cx
        self.center_y = cy
        self.l = self.center_x - (cols / (self.zoom * 2))
        self.r = self.center_x + (cols / (self.zoom * 2))
        self.t = self.center_y - (rows / (self.zoom * 2))
        self.b = self.center_y + (rows / (self.zoom * 2))
        self.dx = (self.r - self.l) / cols
        self.dy = (self.b - self.t) / rows

    def cal_pixel(self, row, col, p):
        iters = 0
        real = self.l + col * self.dx
        imag = self.t + row * self.dy
        tmpReal = real
        tmpImag = imag

        while ((real * real) + (imag * imag) <= 4.0) and (iters < self.iter):
            nextReal = (real * real) - (imag * imag) + tmpReal
            imag = (2 * real * imag) + tmpImag
            real = nextReal
            iters += 1

        if iters < self.iter:
            p.r = ((iters & 63) << 1)
            p.g = ((iters << 3) & 255)
            p.b = ((iters >> 8) & 255)
        return p


def test_mandelbrot(rows, cols, max_iters, center_x, center_y, zoom, output):
    p = Pixel()
    # p.g = 255

    mandelbrot = Mandelbrot(rows, cols, p)

    iterate = Iterate(max_iters, center_x, center_y, zoom, rows, cols)
    mandelbrot.mapIndexInPlaceM(iterate.cal_pixel)

    image = convert(mandelbrot)

    if output:
        ppm(cols, rows, 255, image)


# main
if __name__ == "__main__":

    initSkeletons(False)

    rows, cols, n_runs, n_gpus = 1000, 1000, 2, 1
    max_iters, zoom = 1000, 800
    output, warmup = 1, 0

    # if isRootProcess():
    #     print("rows;cols;maxIters;zoom;nRuns;nGPUS;averagetime;")

    if len(sys.argv) < 7:
        if isRootProcess():
            print("Usage: " + sys.argv[0] + " #rows #cols #maxIters #zoom #nRuns #nGPUs")
            string = "Default values: rows = " + str(rows) + \
                     ", cols = " + str(cols) + \
                     ", maxIters = " + str(max_iters) + \
                     ", zoom = " + str(zoom) + \
                     ", n_runs = " + str(n_runs) + \
                     ", n_gpus = " + str(n_gpus)
            print(string)
    else:
        rows = int(sys.argv[1])
        cols = int(sys.argv[2])
        max_iters = int(sys.argv[3])
        zoom = int(sys.argv[4])
        n_runs = int(sys.argv[5])
        n_gpus = int(sys.argv[6])
        output = True

    setNumRuns(n_runs)
    center_x = -0.73
    center_y = 0.0

    if warmup:
        test_mandelbrot(rows, cols, max_iters, center_x, center_y, zoom, False)

    t = Timer()
    runs = getNumRuns()
    for run in range(runs):
        test_mandelbrot(rows, cols, max_iters, center_x, center_y, zoom, output)
    time = t.stop()
    print(str(rows) + ";" + str(cols) + ";" + str(max_iters) + ";" + str(zoom) + ";" + str(n_runs) + ";" + str(
        n_gpus) + ";" + str(time/n_runs) + ";")

    terminateSkeletons()

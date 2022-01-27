from PIL import Image
import colorsys
import math
import os
import sys
import timeit
from build.muesli import *


def log_color(distance, base, const, scale):
    color = -1 * math.log(distance, base)
    rgb = colorsys.hsv_to_rgb(const + scale * color, 0.8, 0.9)
    return tuple(round(i * 255) for i in rgb)


def power_color(distance, exp, const, scale):
    color = distance ** exp
    rgb = colorsys.hsv_to_rgb(const + scale * color, 1 - 0.6 * color, 0.9)
    return tuple(round(i * 255) for i in rgb)


def write_img(out_img, filename):
    rows = out_img.getRows()
    cols = out_img.getCols()
    img = Image.new('RGB', (cols, rows), color='black')
    pixels = img.load()

    for y in range(rows):
        for x in range(cols):
            distance = out_img.get(y*cols+x)
            rgb = power_color(distance, 0.2, 0.27, 1.0)
            pixels[x, y] = rgb

    img.save(filename)
    os.system('open ' + filename)


class Iterate:
    def __init__(self, precision, cx, cy, xrange, rows, cols):
        self.precision = precision
        self.center_x = cx
        self.center_y = cy
        self.maxAbs = 4.0
        self.xRange, self.yRange = xrange, xrange
        self.minX = self.center_x - self.xRange / 2
        self.maxX = self.center_x + self.xRange / 2
        self.minY = self.center_y - self.yRange / 2
        self.maxY = self.center_y + self.yRange / 2
        self.rows = rows
        self.cols = cols

    def operate(self, row, col, w):
        x = self.minX + col * self.xRange / self.cols
        y = self.maxY - row * self.yRange / self.rows
        oldX = x
        oldY = y
        distance = precision  # necessary to avoid error
        for i in range(self.precision + 1):
            a = x * x - y * y  # real component of z^2
            b = 2 * x * y  # imaginary component of z^2
            x = a + oldX  # real component of new z
            y = b + oldY  # imaginary component of new z
            if x * x + y * y > self.maxAbs:
                break
        if i < self.precision:
            distance = (i + 1) / (self.precision + 1)
        return distance


def test_mandelbrot(rows, cols, precision, center_x, center_y, xrange, output):
    mandelbrot = floatDM(rows, cols, 0.0)
    iterate = Iterate(precision, center_x, center_y, xrange, rows, cols)
    mandelbrot.mapIndexInPlaceM(iterate.operate)
    if output:
        write_img(mandelbrot, "mandelbrot.png")


# main
if __name__ == "__main__":
    initSkeletons(False)

    rows, cols, n_runs, n_gpus = 1000, 1000, 1, 1
    precision, xrange = 500, 3.4
    output, warmup = 1, 0

    if len(sys.argv) < 7:
        if isRootProcess():
            print("Usage: " + sys.argv[0] + " #rows #cols #precision #xrange #nRuns #nGPUs")
            string = "Default values: rows = " + str(rows) + \
                     ", cols = " + str(cols) + \
                     ", precision = " + str(precision) + \
                     ", xrange and yrange = " + str(xrange) + \
                     ", n_runs = " + str(n_runs) + \
                     ", n_gpus = " + str(n_gpus)
            print(string)
    else:
        rows = int(sys.argv[1])
        cols = int(sys.argv[2])
        precision = int(sys.argv[3])
        xrange = int(sys.argv[4])
        n_runs = int(sys.argv[5])
        n_gpus = int(sys.argv[6])
        output = True
        print(str(rows) + ";" + str(cols) + ";" + str(precision) + ";" + str(xrange) +
              ";" + str(n_runs) + ";" + str(n_gpus))

    setNumRuns(n_runs)
    center_x = -0.65
    center_y = 0.0

    if warmup:
        test_mandelbrot(rows, cols, precision, center_x, center_y, xrange, False)

    start = timeit.default_timer()
    for run in range(0, getNumRuns()):
        test_mandelbrot(rows, cols, precision, center_x, center_y, xrange, output)
    stop = timeit.default_timer()

    terminateSkeletons()

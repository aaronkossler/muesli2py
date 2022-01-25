import sys
import timeit
from io import StringIO
import shutil
from build.muesli import *


def write_ppm(out_img, filename):
    rows = out_img.getRows()
    cols = out_img.getCols()

    ofs = StringIO()
    ofs.write("P6\n" + str(cols) + " " + str(rows) + "\n255\n")

    for y in range(0, rows):
        for x in range(0, cols):
            p = out_img.get(y*cols+x)
            ofs.write(str(p.r) + " " + str(p.g) + " " + str(p.b) + "  ")

    with open(filename, "w") as fd:
        ofs.seek(0)
        shutil.copyfileobj(ofs, fd)


class Iterate:
    def __init__(self, m_iters, cx, cy, z, rows, cols):
        self.maxIters = m_iters
        self.zoom = z
        self.center_x = cx
        self.center_y = cy
        self.maxAbs = 4.0
        self.l = self.center_x - (cols / (self.zoom * 2))
        self.r = self.center_x + (cols / (self.zoom * 2))
        self.t = self.center_y - (rows / (self.zoom * 2))
        self.b = self.center_y + (rows / (self.zoom * 2))
        self.dx = (self.r - self.l) / cols
        self.dy = (self.b - self.t) / rows

    def operate(self, row, col, p):
        iters = 0
        real = self.l + col * self.dx
        tmp_real = real
        imag = self.t + row * self.dy
        tmp_imag = imag
        while (real * real) + (imag * imag) <= self.maxAbs and iters < self.maxIters:
            next_real = (real * real) - (imag * imag) + tmp_real
            imag = (2 * real * imag) + tmp_imag
            real = next_real
            iters += 1
        if iters < self.maxIters:
            p.r = ((iters & 63) << 1)
            p.g = ((iters << 3) & 255)
            p.b = ((iters >> 8) & 255)
        return p


def test_mandelbrot(rows, cols, max_iters, center_x, center_y, zoom, output):
    p = Pixel()
    mandelbrot = Mandelbrot(rows, cols, p)

    iterate = Iterate(max_iters, center_x, center_y, zoom, rows, cols)
    mandelbrot.mapIndexInPlaceM(iterate.operate)
    if output:
        write_ppm(mandelbrot, "mandelbrot.ppm")


# main
initSkeletons(False)

rows, cols, n_runs, n_gpus = 1000, 1000, 1, 1
max_iters, zoom = 1000, 800
output, warmup = 1, 0

if len(sys.argv) < 7:
    if isRootProcess:
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
    print(str(rows) + ";" + str(cols) + ";" + str(max_iters) + ";" + str(zoom) + ";" + str(n_runs) + ";" + str(n_gpus))

# setNumRuns(n_runs)
center_x = -0.73
center_y = 0.0

if warmup:
    test_mandelbrot(rows, cols, max_iters, center_x, center_y, zoom, False)

start = timeit.default_timer()
for run in range(0, n_runs):
    test_mandelbrot(rows, cols, max_iters, center_x, center_y, zoom, output)
stop = timeit.default_timer()

terminateSkeletons()

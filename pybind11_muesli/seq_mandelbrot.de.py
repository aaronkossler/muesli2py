import sys
import timeit
import array
import time


def ppm(width, height, maxval, image):
    ppm_header = f'P6 {width} {height} {maxval}\n'
    with open('seq_mandelbrot.ppm', 'wb') as f:
        f.write(bytearray(ppm_header, 'ascii'))
        image.tofile(f)


def cal_pixel(image, row, col, iter, l, t, dx, dy):
    iters = 0
    index = 3 * (row * cols + col)
    real = l + col * dx
    imag = t + row * dy
    tmpReal = real
    tmpImag = imag
    while ((real * real) + (imag * imag) <= 4.0) and (iters < iter):
        nextReal = (real * real) - (imag * imag) + tmpReal
        imag = (2 * real * imag) + tmpImag
        real = nextReal
        iters += 1

    if iters < iter:
        image[index] = ((iters & 63) << 1)
        image[index+1] = ((iters << 3) & 255)
        image[index+2] = ((iters >> 8) & 255)


def test_mandelbrot(rows, cols, max_iters, center_x, center_y, zoom, output):
    l = center_x - float(cols / (zoom * 2))
    r = center_x + float(cols / (zoom * 2))
    t = center_y - float(rows / (zoom * 2))
    b = center_y + float(rows / (zoom * 2))

    dx = (r - l) / cols
    dy = (b - t) / rows
    image = array.array('B', [0, 0, 0] * cols * rows)
    for x in range(rows * cols):
        col = x % cols
        row = int(x / cols)
        cal_pixel(image, row, col, max_iters, l, t, dx, dy)

    #ppm(cols, rows, 255, image)


# main
if __name__ == "__main__":

    rows, cols, n_runs, n_gpus = 1000, 1000, 1, 1
    max_iters, zoom = 1000, 800
    output, warmup = 1, 0

    if len(sys.argv) < 7:
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
        print(str(rows) + ";" + str(cols) + ";" + str(max_iters) + ";" + str(zoom) + ";" + str(n_runs) + ";" + str(
            n_gpus))

    center_x = -0.73
    center_y = 0.0

    start = time.time()
    for run in range(n_runs):
        test_mandelbrot(rows, cols, max_iters, center_x, center_y, zoom, output)
    stop = time.time()

    print(str(rows) + ";" + str(cols) + ";" + str(max_iters) + ";" + str(zoom) + ";" + str(n_runs) + ";" + str(
        n_gpus) + ";" + str((stop - start)/n_runs) + ";")

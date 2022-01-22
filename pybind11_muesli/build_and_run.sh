cd build || { echo 'cd failed' ; exit 1 ; }
cmake .. || { echo 'cmake failed' ; exit 1 ; }
make || { echo 'make failed' ; exit 1 ; }
mpirun -np 2 python3.9 ../test.py

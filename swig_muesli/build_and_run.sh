cd muesli || { echo 'cd failed' ; exit 1 ; }
./build_muesli.sh || { echo 'build muesli failed' ; exit 1 ; }
cd ..
mpirun -np 2 python3 main.py

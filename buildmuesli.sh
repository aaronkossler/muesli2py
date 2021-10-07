cd Muesli
python3 setup.py build_clib
python3 setup.py build
cp build/lib.linux-x86_64-3.8/muesli.cpython-38-x86_64-linux-gnu.so ../
cd ..
#mpirun -np 2 
python3 main.py

cd Muesli
#python3 setup.py build_clib
python3 setup.py build
# Above needs some overhead to configure path etc. to include mpi.h mpicc can also build the module
#mpicc -pthread -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -I/usr/local/lib/python3.8/dist-packages/numpy/core/include -I/usr/include/python3.8 -c muesli.cpp -o build/temp.linux-x86_64-3.8/muesli.o

cp build/lib.linux-x86_64-3.8/muesli.cpython-38-x86_64-linux-gnu.so ../
cd ..
mpirun -np 2 python3 main.py 
#python3 main.py

# ... Yes we build with shell scripts :tada:
# Alternatively to setup_py manual compilation is possible
#gcc -O2 -fPIC -c example.c -I/usr/include/python3.8
#gcc -O2 -fPIC -c example_wrap.c -I/usr/include/python3.8
#gcc -shared example.o example_wrap.o -o _example.so
for name in 'muesli_utils'
do
    cd $name || { echo 'Going to directory ' $name ' failed' ; exit 1; }
    swig -python $name.i || { echo 'building swig for ' $name ' failed' ; exit 1; }
    python3 setup_$name.py build_clib || { echo 'build_clib for ' $name ' failed' ; exit 1; }
    python3 setup_$name.py build_ext --inplace || { echo 'build_ext --inplace for ' $name ' failed' ; exit 1; }
    cd ../
done
for name in 'da'
do
    cd $name || { echo 'Going to directory ' $name ' failed' ; exit 1; }
    swig -c++ -python $name.i || { echo 'building swig for ' $name ' failed' ; exit 1; }
    CC=g++ python3 setup_$name.py build_clib || { echo 'build_clib for ' $name ' failed' ; exit 1; }
    CC=g++ python3 setup_$name.py build_ext --inplace || { echo 'build_ext --inplace for ' $name ' failed' ; exit 1; }
    cd ../
done

#mpirun -np 2 python3 main.py 
#python main.py
#python3 main.py

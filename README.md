# ALPHA muesli2py
muesli2py is developed to supply the functionalities of muesli in python code. 

## Minimum Requirements
For a minimal prototype the following features are essential:
- [x] configure setup.py to support 
    - [x] mpi
    - [ ] cuda OPTIONAL!([probably a good example](https://github.com/rmcgibbo/npcuda-example/blob/master/cython/setup.py))
    - [x] (openacc)
    - [x] openmp (Threading Conflict with GIL)
- [x] support for DA/DM (distributed arrays - distributed matrices)
    - [x] research alternatives to include classes (swig (support for parallelization frameworks?), Python class) 
       - [x] swig works (see swig_muesli)
       - [x] pybind11 works (see pybind11_muesli) --> framework of choice
    - [x] native arrays
      - [x] multi-constructors 
      - [x] missing templates 
    - [x] nparray ([C-API](https://numpy.org/doc/stable/user/c-info.html), [writing extension modules](https://numpy.org/doc/stable/user/c-info.how-to-extend.html#writing-an-extension-module))
- [x] map + variations
  - [x] how can userfunctions be pased as arguments? --> functional.h in pybind11
- [ ] ...

Features which are not essential for a first prototype
- [ ] DA/DM 
    - [ ] support of more than 2 dimensions  
- [ ] other skeletons

## Usage
The prototype works similar to the original muesli library. To make use of the library, code has to be wrapped with the functions "initSkeletons" and "terminateSkeletons". Example code, which demonstrates all the funcitonality can be found in the testDA.py and testDM.py files.


## Supporting Documentation
- [Parsing arguments between C and python](https://docs.python.org/3/c-api/arg.html)
- [Object Structures](https://docs.python.org/3/c-api/structures.html)
- [Pybind11 Documentation](https://pybind11.readthedocs.io/en/stable/index.html)

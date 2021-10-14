from distutils.core import setup, Extension
import os

from setuptools import setup, Extension, Command
from Cython.Distutils import build_ext
from Cython.Build import cythonize
import numpy as np
from os.path import join as pjoin

mpi_compile_args = os.popen("mpic++ --showme:compile").read().strip().split(' ')
mpi_link_args = os.popen("mpic++ --showme:link").read().strip().split(' ')


def find_in_path(name, path):
    """Find a file in a search path"""

    # Adapted fom http://code.activestate.com/recipes/52224
    for dir in path.split(os.pathsep):
        binpath = pjoin(dir, name)
        if os.path.exists(binpath):
            return os.path.abspath(binpath)
    return None


try:
    numpy_include = np.get_include()
except AttributeError:
    numpy_include = np.get_numpy_include()

nvcc = find_in_path('nvcc', os.environ['PATH'])
if isinstance(nvcc, str):
    print('CUDA')
    # setup(name='PackageName',
    #       author='Nina Herrmann',
    #       version='1.0',
    #       description='This is a package for Muesli',
    #       ext_modules=cythonize(cuda_setup.get_module()),
    #       cmdclass={'build_ext': cuda_setup.custom_build_ext()}
    #       )
    #sources=['/home/n_herr03/Research/mueslicollection/Muesli2Py/swig_muesli'
    #                                             '/muesli_utils/muesli_utils_wrap.c',
    #                                             '/home/n_herr03/Research/mueslicollection/Muesli2Py/swig_muesli'
    #                                             '/muesli_utils/muesli_utils.c'],
else:
    module = Extension('_muesli_utils', sources=['muesli_utils/muesli_utils_wrap.c',
                                                 'muesli_utils/muesli_utils.c'],
                       include_dirs=[np.get_include()],
                       library_dirs=['/usr/include/boost/'],
                       # '/usr/include/boost/chrono'
                       libraries=[],
                       extra_compile_args=(["-fopenmp"] + mpi_compile_args),
                       extra_link_args=(["-fopenmp"] + mpi_link_args)
                       )
    damodule = Extension('_da', sources=['da/da_wrap.c', 'da/da.cxx'],
                       include_dirs=[np.get_include()],
                       library_dirs=['/usr/include/boost/'],
                       # '/usr/include/boost/chrono'
                       libraries=[],
                       extra_compile_args=(["-fopenmp"] + mpi_compile_args),
                       extra_link_args=(["-fopenmp"] + mpi_link_args)
                       )
    setup(name='muesli_utils',
          author='Nina Herrmann',
          version='1.0',
          description='This is a package for Muesli',
          ext_modules=[module, damodule],
          py_modules=["muesli_utils"]
          )

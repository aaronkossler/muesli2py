from distutils.core import setup, Extension
import os
from setuptools import setup, Extension, Command
from Cython.Distutils import build_ext
from Cython.Build import cythonize
import numpy as np

# TODO use this
mpi_compile_args = os.popen("mpic++ --showme:compile").read().strip().split(' ')
mpi_link_args    = os.popen("mpic++ --showme:link").read().strip().split(' ')

module = Extension('muesli', sources=['muesli.cpp'],
                   include_dirs=[np.get_include()],
                   library_dirs=['/usr/include/boost/'],
                   libraries=['/usr/include/boost/chrono'],
                   extra_compile_args=(["-fopenmp"] + mpi_compile_args),
                   extra_link_args=(["-fopenmp"] + mpi_link_args)
                  )

setup(name='PackageName',
      version='1.0',
      description='This is a package for myModule',
      ext_modules=cythonize(module
                            #extra_compile_args = ["-I."],
                            #extra_link_args = ["-L./usr/lib/liblapack.dylib"]
                            )
      )

#!/bin/bash
##
## This file needs to be adapted to your needs
##
#SBATCH --export NONE
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1 
#SBATCH --cpus-per-task 32
##  no GPUs
#SBATCH --partition broadwell
#SBATCH --time 5:00:00
#SBATCH --exclusive

#SBATCH --job-name Muesli3-CPU-mandelbrot-parallel
#SBATCH --output /scratch/tmp/n_herr03/mandelbrot/cpuparallel.csv
#SBATCH --error /scratch/tmp/n_herr03/mandelbrot/cpuparallel.error
#SBATCH --mail-type ALL
#SBATCH --mail-user n_herr03@uni-muenster.de

cd ~/muesli2py/pybind11_muesli
module load CMake/3.15.3
module load palma/2021a  GCC/10.3.0  OpenMPI/4.1.1
module load SciPy-bundle/2021.05

# export I_MPI_DEBUG=3
# export I_MPI_FABRICS=shm:ofa
# alternativ: Ethernet statt Infiniband: export I_MPI_FABRICS=shm:tcp
#(argv[1]);
#    cols = atoi(argv[2]);
#    maxIters = atoi(argv[3]);
#    zoom = atoi(argv[4]);
#    nRuns = atoi(argv[5]);
#    nGPUs = atoi(argv[6]);
for row in 1000 2000 3000 4000 5000; do
    for maxIters in 1000 2000 3000 4000 5000; do
        for zoom in 800 1000 1200 14000; do
        for nRuns in 30; do
        for nGPUs in 1; do
    		    for file in ~/muesli3/build/bin/*cpu; do
      			mpirun -np 32 python3 $file $row $row $maxIters $zoom $nRuns $nGPU 0 0
    		    done
        done
        done
    done
    done
done



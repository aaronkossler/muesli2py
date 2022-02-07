#!/bin/bash
##
## This file needs to be adapted to your needs
##
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1 
#SBATCH --cpus-per-task 1
##  no GPUs
#SBATCH --partition broadwell
#SBATCH --time 20:00:00
#SBATCH --exclusive

#SBATCH --job-name MPI-mandelbrot-parallel
#SBATCH --output /scratch/tmp/n_herr03/aaron/1nodedata.csv
#SBATCH --error /scratch/tmp/n_herr03/aaron/1nodeerror.dat
#SBATCH --mail-type ALL
#SBATCH --mail-user n_herr03@uni-muenster.de

export OMP_NUM_THREADS=1
ml purge
cd /home/n/n_herr03/muesli2py/pybind11_muesli/
module load palma/2020b  
#module load intel/2021a 
module load foss/2020b
# 21.05
module load SciPy-bundle/2020.11
module load CMake/3.18.4
ml
./build.sh
# vorlufig, bis MPI ber Infiniband funktioniert
export I_MPI_DEBUG=3
# export I_MPI_FABRICS=shm:ofa   nicht verfgbar
# alternativ: Ethernet statt Infiniband: 
# export I_MPI_FABRICS=shm:tcp
#export OMP_NUM_THREADS=1
#(argv[1]);
#    cols = atoi(argv[2]);
#    maxIters = atoi(argv[3]);
#    zoom = atoi(argv[4]);
#    nRuns = atoi(argv[5]);
#    nGPUs = atoi(argv[6]);
for row in 1000 2000 3000 4000 5000; do
    for maxIters in 1000 2000 3000 4000 5000; do
        for zoom in 800 1000 1200 1400; do
            for nRuns in 30; do
                for nGPUs in 1; do
		    for np in 1 2 4 8; do
      			mpirun -np $np python3 mandelbrot.de.py $row $row $maxIters $zoom $nRuns $nGPU 0 0
		    done
                done
            done
        done
    done
done


#for row in 1000 2000 3000 4000 5000; do
  #  for maxIters in 1000 2000 3000 4000 5000; do
  #      for zoom in 800 1000 1200 14000; do
 #           for nRuns in 30; do
  #              for nGPUs in 1; do
		 #   for np in 1 2 4 8; do
 #     			srun -n 1 python3 mandelbrot.de.py $row $row $maxIters $zoom $nRuns $nGPU 0 0
		  #  done
#                done
#            done
#        done
#    done
#done
#srun -n 1 python3 mandelbrot.de.py $row $row $maxIters $zoom $nRuns $nGPU 0 0
#ml

#!/bin/bash
#SBATCH --job-name=parallel_job_test # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=4                   # number of processes = 4
#SBATCH --cpus-per-task=4            # Number of CPU cores per process
#SBATCH --mem=600mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Project            # Partition name: Project or Debug (Debug is default)
#SBATCH --output=parallel10000-16core.out    # Output file: paralle1_20.out
mpirun -np 1 ./parallel 10000
mpirun -np 2 ./parallel 10000
mpirun -np 3 ./parallel 10000
mpirun -np 4 ./parallel 10000
mpirun -np 5 ./parallel 10000
mpirun -np 6 ./parallel 10000
mpirun -np 7 ./parallel 10000
mpirun -np 8 ./parallel 10000
mpirun -np 9 ./parallel 10000
mpirun -np 10 ./parallel 10000
mpirun -np 11 ./parallel 10000
mpirun -np 12 ./parallel 10000
mpirun -np 13 ./parallel 10000
mpirun -np 14 ./parallel 10000
mpirun -np 15 ./parallel 10000
mpirun -np 16 ./parallel 10000
mpirun -np 17 ./parallel 10000
mpirun -np 18 ./parallel 10000
mpirun -np 19 ./parallel 10000
mpirun -np 20 ./parallel 10000

#!/bin/bash
#SBATCH --job-name=parallel_job_test # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=4                   # number of processes = 4
#SBATCH --cpus-per-task=5            # Number of CPU cores per process
#SBATCH --mem=600mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Project            # Partition name: Project or Debug (Debug is default)
#SBATCH --output=parallel1000000-20core1.out    # Output file: paralle1_20.out
mpirun -np 1 ./parallel 1000000
mpirun -np 2 ./parallel 1000000
mpirun -np 3 ./parallel 1000000
mpirun -np 4 ./parallel 1000000
mpirun -np 5 ./parallel 1000000
mpirun -np 6 ./parallel 1000000
mpirun -np 7 ./parallel 1000000
mpirun -np 8 ./parallel 1000000
mpirun -np 9 ./parallel 1000000
mpirun -np 10 ./parallel 1000000
mpirun -np 11 ./parallel 1000000
mpirun -np 12 ./parallel 1000000
mpirun -np 13 ./parallel 1000000
mpirun -np 14 ./parallel 1000000
mpirun -np 15 ./parallel 1000000
mpirun -np 16 ./parallel 1000000
mpirun -np 17 ./parallel 1000000
mpirun -np 18 ./parallel 1000000
mpirun -np 19 ./parallel 1000000
mpirun -np 20 ./parallel 1000000
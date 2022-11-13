#!/bin/bash
#SBATCH --job-name=parallel_job_test # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=4                   # number of processes = 4
#SBATCH --cpus-per-task=2            # Number of CPU cores per process
#SBATCH --mem=600mb                  # Total memory limit
#SBATCH --time=01:00:00              # Time limit hrs:min:sec
#SBATCH --partition=Debug            # Partition name: Project or Debug (Debug is default)
#SBATCH --output=parallel4_20.out    # Output file: paralle1_20.out
mpirun -np 4 ./parallel 20

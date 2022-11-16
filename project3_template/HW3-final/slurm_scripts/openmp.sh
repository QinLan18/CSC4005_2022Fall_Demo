#!/bin/bash
#SBATCH --job-name=your_job_name # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # number of processes = 1 
#SBATCH --cpus-per-task=20      # Number of CPU cores allocated to each process
#SBATCH --partition=Project            # Partition name: Project or Debug (Debug is default)
#SBATCH --output openmp.out         ## filename of the output
cd /nfsmnt/118010246/HW3
./openmp 200 100 1
./openmp 200 100 2
./openmp 200 100 4
./openmp 200 100 8
./openmp 200 100 16
./openmp 200 100 32

./openmp 500 100 1
./openmp 500 100 2
./openmp 500 100 4
./openmp 500 100 8
./openmp 500 100 16
./openmp 500 100 32

./openmp 1000 100 1
./openmp 1000 100 2
./openmp 1000 100 4
./openmp 1000 100 8
./openmp 1000 100 16
./openmp 1000 100 32


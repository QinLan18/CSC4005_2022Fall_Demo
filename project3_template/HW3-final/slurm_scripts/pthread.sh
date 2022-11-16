#!/bin/bash
#SBATCH --job-name=your_job_name # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # number of processes = 1 
#SBATCH --cpus-per-task=20      # Number of CPU cores allocated to each process
#SBATCH --partition=Project            # Partition name: Project or Debug (Debug is default)
#SBATCH --output pthread.out         ## filename of the output
cd /nfsmnt/118010246/HW3

./pthread 200 100 1
./pthread 200 100 2
./pthread 200 100 4
./pthread 200 100 8
./pthread 200 100 12
./pthread 200 100 16

./pthread 500 100 1
./pthread 500 100 2
./pthread 500 100 4
./pthread 500 100 8
./pthread 500 100 12
./pthread 500 100 16

./pthread 1000 100 1
./pthread 1000 100 2
./pthread 1000 100 4
./pthread 1000 100 8
./pthread 1000 100 12
./pthread 1000 100 16


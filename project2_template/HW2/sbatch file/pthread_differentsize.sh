#!/bin/bash
#SBATCH --job-name=your_job_name # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # number of processes = 1 
#SBATCH --cpus-per-task=20      # Number of CPU cores allocated to each process
#SBATCH --partition=Project            # Partition name: Project or Debug (Debug is default)
#SBATCH --output=pthread_different_size.out
cd /nfsmnt/118010246/HW2
./pthread 1000 1000 100 1 1
./pthread 1000 1000 100 2 1
./pthread 1000 1000 100 4 1
./pthread 1000 1000 100 8 1
./pthread 1000 1000 100 16 1
./pthread 1000 1000 100 24 1
./pthread 1000 1000 100 32 1

./pthread 2000 2000 100 1 1
./pthread 2000 2000 100 2 1
./pthread 2000 2000 100 4 1
./pthread 2000 2000 100 8 1
./pthread 2000 2000 100 16 1
./pthread 2000 2000 100 24 1
./pthread 2000 2000 100 32 1

./pthread 3000 3000 100 1 1
./pthread 3000 3000 100 2 1
./pthread 3000 3000 100 4 1
./pthread 3000 3000 100 8 1
./pthread 3000 3000 100 16 1
./pthread 3000 3000 100 24 1
./pthread 3000 3000 100 32 1

./pthread 4000 4000 100 1 1
./pthread 4000 4000 100 2 1
./pthread 4000 4000 100 4 1
./pthread 4000 4000 100 8 1
./pthread 4000 4000 100 16 1
./pthread 4000 4000 100 24 1
./pthread 4000 4000 100 32 1

./pthread 5000 5000 100 1 1
./pthread 5000 5000 100 2 1
./pthread 5000 5000 100 4 1
./pthread 5000 5000 100 8 1
./pthread 5000 5000 100 16 1
./pthread 5000 5000 100 24 1
./pthread 5000 5000 100 32 1

./pthread 6000 6000 100 1 1
./pthread 6000 6000 100 2 1
./pthread 6000 6000 100 4 1
./pthread 6000 6000 100 8 1
./pthread 6000 6000 100 16 1
./pthread 6000 6000 100 24 1
./pthread 6000 6000 100 32 1

./pthread 7000 7000 100 1 1
./pthread 7000 7000 100 2 1
./pthread 7000 7000 100 4 1
./pthread 7000 7000 100 8 1
./pthread 7000 7000 100 16 1
./pthread 7000 7000 100 24 1
./pthread 7000 7000 100 32 1

./pthread 8000 8000 100 1 1
./pthread 8000 8000 100 2 1
./pthread 8000 8000 100 4 1
./pthread 8000 8000 100 8 1
./pthread 8000 8000 100 16 1
./pthread 8000 8000 100 24 1
./pthread 8000 8000 100 32 1
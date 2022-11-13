#!/bin/bash
#SBATCH --job-name=your_job_name # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # number of processes = 1 
#SBATCH --cpus-per-task=20      # Number of CPU cores allocated to each process
#SBATCH --partition=Project            # Partition name: Project or Debug (Debug is default)
#SBATCH --output=pthread_dynamic.out
cd /nfsmnt/118010246/HW2
./pthread 1000 1000 100 1 1
./pthread 1000 1000 100 4 1
./pthread 1000 1000 100 20 1
./pthread 1000 1000 100 40 1
./pthread 1000 1000 100 80 1
./pthread 1000 1000 100 120 1
./pthread 1000 1000 100 160 1
./pthread 1000 1000 100 240 1
./pthread 1000 1000 100 320 1
./pthread 1000 1000 100 1600 1
./pthread 1000 1000 100 2400 1
./pthread 1000 1000 100 3200 1

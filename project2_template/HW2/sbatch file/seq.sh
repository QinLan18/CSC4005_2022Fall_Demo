#!/bin/bash
#SBATCH --job-name=your_job_name # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # number of processes = 1 
#SBATCH --cpus-per-task=20      # Number of CPU cores allocated to each process
#SBATCH --partition=Project            # Partition name: Project or Debug (Debug is default)
#SBATCH --output=sequential_differentsize.out
cd /nfsmnt/118010246/HW2
./seq 1000 1000 100
./seq 2000 2000 100
./seq 3000 3000 100
./seq 4000 4000 100
./seq 5000 5000 100
./seq 6000 6000 100
./seq 7000 7000 100
./seq 8000 8000 100



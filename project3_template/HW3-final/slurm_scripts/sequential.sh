#!/bin/bash
#SBATCH --job-name=Assignment3Seq # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # number of processes = 1 
#SBATCH --cpus-per-task=20      # Number of CPU cores allocated to each process
#SBATCH --partition=Project            # Partition name: Project or Debug (Debug is default)
#SBATCH --output sequential.out         ## filename of the output
cd /nfsmnt/118010246/HW3

./seq 200 100
./seq 500 100
./seq 1000 100

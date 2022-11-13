#!/bin/bash
#SBATCH --job-name=your_job_name # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # number of processes = 1 
#SBATCH --cpus-per-task=20      # Number of CPU cores allocated to each process
#SBATCH --partition=Project            # Partition name: Project or Debug (Debug is default)
#SBATCH --output=sequential.out
cd /nfsmnt/118010246/HW2
./seq 1000 1000 100
./seq 1000 1000 200
./seq 1000 1000 300
./seq 1000 1000 400
./seq 1000 1000 500

./seq 5000 5000 100
./seq 5000 5000 200
./seq 5000 5000 300
./seq 5000 5000 400
./seq 5000 5000 500

./seq 10000 10000 100
./seq 10000 10000 200
./seq 10000 10000 300
./seq 10000 10000 400
./seq 10000 10000 500



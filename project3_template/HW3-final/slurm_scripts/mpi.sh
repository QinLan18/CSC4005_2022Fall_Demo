#!/bin/bash
#SBATCH --job-name=your_job_name # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=20                   # number of processes = 20
#SBATCH --cpus-per-task=1      # Number of CPU cores allocated to each process (please use 1 here, in comparison with pthread)
#SBATCH --partition=Project            # Partition name: Project or Debug (Debug is default)
#SBATCH --output mpi.out         ## filename of the output
cd /nfsmnt/118010246/HW3
mpirun -np 1 ./mpi 1000 100
mpirun -np 2 ./mpi 1000 100
mpirun -np 4 ./mpi 1000 100
mpirun -np 8 ./mpi 1000 100
mpirun -np 16 ./mpi 1000 100
mpirun -np 32 ./mpi 1000 100
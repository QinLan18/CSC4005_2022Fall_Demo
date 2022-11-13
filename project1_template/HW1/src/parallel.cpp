#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

void Merge_low(int* recv, int* tempA, int* tempB, int local_n);
void Merge_high(int* recv, int* tempA, int* tempB, int local_n);

//argv: mpirun -np 4 ./parallel 20
int main(int argc, char**argv){
    int rank, p; //rank, process
    int *global_A, recv[1000000],*tempA,*tempB;
    int global_n = 1, local_n;
    int oddpartner, evenpartner;
    int root_process = 0, i;
    double start, finish;
    
    MPI_Status status;


    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if(rank == root_process){
        global_n = atoi(argv[1]);       //get the number of elements from arguments
        
        local_n = global_n / p;     //number of elements in local list
        
        
        global_A = (int*) malloc(sizeof(int)*global_n);        //generate random elements
        for(i = 0; i < global_n; i++){
            global_A[i] = rand() % 1000000;     
        }
        
        ofstream output(to_string(global_n)+string("input.in"));        //ouput the random list
        for(i = 0; i < global_n; i++){
            output << global_A[i] << endl;
            
        }
        // printf("Generated random list.\n");
        // printf("Input: \n");
        // for(i = 0; i < global_n; i++){
        //     printf("%d ",global_A[i]);
        // }
        //  printf("\n");

    }
    else{
        global_A = NULL;
    }
    
    MPI_Bcast(&local_n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(global_A, local_n, MPI_INT, &recv, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // printf(" process %d received data: ", rank);
    // for( i = 0; i < local_n; i++){
    //     printf("%d ", recv[i]);
    // }
    // printf("\n");

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    
    sort(recv, recv+local_n); //sort local_n numbers


    //begin even-odd sort

    if(rank%2==0){ //even phase
        evenpartner = rank + 1;
        if(evenpartner == p) evenpartner = MPI_PROC_NULL;
        oddpartner = rank - 1;
    }
    else{ //odd phase
        oddpartner = rank + 1;
        if(oddpartner == p) oddpartner = MPI_PROC_NULL;
        evenpartner = rank - 1;
    }

    int phase;
    tempA = (int*) malloc(local_n*sizeof(int));
    tempB = (int*) malloc(local_n*sizeof(int));
    for(phase = 0; phase < p; phase++){
        if(phase%2==0)  //even phase
        {
            if(evenpartner >= 0){ 
                MPI_Sendrecv(recv, local_n, MPI_INT, evenpartner, 1, tempA,local_n,MPI_INT, evenpartner, 1, MPI_COMM_WORLD, &status);
                if(rank % 2 != 0) //odd rank
                //recv should has the largest local_n elements from recv and its evenpartner, since it is odd rank in even phase(the right one)
                    Merge_high(recv, tempA, tempB, local_n);             
                else //even rank               
                    Merge_low(recv, tempA, tempB, local_n);               
            }
        }
        else{   
            if(oddpartner>=0)//odd phase
            {
                MPI_Sendrecv(recv, local_n, MPI_INT, oddpartner, 1, tempA, local_n, MPI_INT, oddpartner, 1, MPI_COMM_WORLD, &status);
                if(rank%2!=0) //odd rank
                    Merge_low(recv, tempA, tempB, local_n);
                else
                    Merge_high(recv, tempA, tempB, local_n);
            }

        }
        
    }
    MPI_Gather(recv, local_n, MPI_INT, global_A, local_n, MPI_INT, 0, MPI_COMM_WORLD);


    MPI_Barrier(MPI_COMM_WORLD);
    finish = MPI_Wtime();
    MPI_Finalize();
    //output the result
    if(rank == root_process){
        // printf("Student ID: 118010246\n");
        // printf("Name: Qin Lan\n");
        // printf("Assignment 1");
        
        // printf("\n");
        // printf("Run Time : %f seconds\n", finish-start);
        printf("%f", finish-start);
        // printf("Input Size: %d \n", global_n);
        // printf("Process Number: %d \n", p);

        ofstream output(to_string(p)+"."+ to_string(global_n)+string(".parallel.out"));
        for( i = 0; i < global_n; i++){
            output << global_A[i] << endl;
        }
        // printf("Result: \n");
        // for(i = 0; i < global_n; i++){
        //     printf("%d ",global_A[i]);
        // }
          printf("\n");
    }
    return 0;

}

void Merge_high(int* recv, int* tempA, int* tempB, int local_n){
    int i,j,k;
    
    for(i = 0; i < local_n; i ++){
        tempB[i] = recv[i];
    }
    for(i= j = k = local_n; k >= 0; k --){
        if(j == -1 || (i >= 0 && tempB[i] >= tempA[j]))
            recv[k] = tempB[i--];
        else
            recv[k] = tempA[j--];
    }

}
void Merge_low(int* recv, int* tempA, int* tempB, int local_n){
    int i,j,k;
    
    for(i = 0; i < local_n; i ++){
        tempB[i] = recv[i];
    }
    for(i= j = k = 0; k < local_n; k ++){
        if(j == local_n || ( i < local_n && tempB[i] < tempA[j]))
            recv[k] = tempB[i++];
        else
            recv[k] = tempA[j++];
    }
}
#include "asg2.h"
#include <stdio.h>
#include <mpi.h>
#include <algorithm>


int rank;
int world_size;
int size = 1000;
int dynamic_flag = 0;
struct Row{
    int index;
    float content[1];
};


int getLength(const int&size, const int &world_size, const int &rank){
	if(size<(world_size-1))
		return (rank-1)<size;
	return (size-rank+1)/world_size+((size-rank+1)%world_size > 0);
}
void sequentialCompute(Point* p){
	/* compute for all points one by one */
	// p = data;
	for (int index = 0; index < total_size; index++){
		compute(p);
		p++;
	}
}

void master(int world_size, Point* p ) {
	
	if(world_size==1){ // sequential version
		printf("Only one process, do sequential compute:\n");
		sequentialCompute(p);
		return;
	}
	MPI_Bcast(p, sizeof(Point), MPI_BYTE, 0, MPI_COMM_WORLD);

    if(!dynamic_flag){ // static version
        int * recvcounts = static_cast <int*> (malloc(world_size*sizeof(int)));
        int * results = static_cast<int*>(malloc(total_size*sizeof(int)));
        int* displs = static_cast<int*>(malloc(world_size*sizeof(int)));

        recvcounts[0] = 0;
        int count = 0;
        for(int i = 1; i < world_size; i++){
            count += recvcounts[i-1]; //size of each row: sizeof(int)*size;
            recvcounts[i] = getLength(size, world_size, i)*sizeof(int)*size;
            displs[i] = count;
        }
        MPI_Gatherv(NULL, 0 , MPI_BYTE, results, recvcounts, displs, MPI_BYTE, 0, MPI_COMM_WORLD);
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                p->color=results[i*size+j];
                p++;
            }
	    }
        free(recvcounts);
        free(displs);
        free(results);
    }
    else{ // dynamic version
        int curr_row = world_size; //
        int recv_row = 0;
        while(recv_row < size - 1){
            // probe for receive 
            MPI_Status status;
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            recv_row++;
            int src = status.MPI_SOURCE;
            struct Row* row = static_cast<struct Row*>(malloc(sizeof(struct Row) + sizeof(int)*size));
            MPI_Recv(row, sizeof(struct Row)+sizeof(int)*size, MPI_BYTE, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            int send_row_index = curr_row++; 
            MPI_Send(&send_row_index,1, MPI_INT, src, 0, MPI_COMM_WORLD);
            // p += row->index*size;
            for(int j=0; j < size; j++){
                p->color = row->content[j];
                p++;
            }
                
            free(row);
        }
    }

}


void slave(int rank) {

	Point* p = static_cast<Point*>(malloc(sizeof(Point)));
    // Point* p = new Point[total_size];
    MPI_Bcast(p, sizeof(Point), MPI_BYTE, 0, MPI_COMM_WORLD);
    if(!dynamic_flag){
        int length = getLength(size, world_size, rank);
        int * my_partition = static_cast<int*>(malloc(length*sizeof(int)*size));
        int start = 0;
        for(int i=1; i < rank; i++)
            start += getLength(size,world_size,i);
        // p += start*size;
        for(int i =0; i < length; i++){
            for(int j = 0; j < size; j++){
                p->x = j;
                p->y = i+start;
                compute(p);
                my_partition[i*size+j] = p->color;
                // p++;
            }
        }
        MPI_Gatherv(my_partition, length*sizeof(int)*size,MPI_BYTE,NULL,NULL,NULL,MPI_BYTE,0,MPI_COMM_WORLD);
        free(my_partition);
    }else{
        if(rank-1 < size){
            struct Row* row = static_cast<struct Row*>(malloc(sizeof(struct Row)+sizeof(int)*size));
            row->index = rank-1;
            MPI_Status status;
            int recv_row_index;
            while(row->index<size){
                // p += row->index*size;
                for(int j = 0; j < size; j++){
                    p->y = j;
                    p->x = row->index;
                    compute(p);
                    row->content[j] = p->color;
                    // p++;
                }
                MPI_Send(row, sizeof(struct Row)+sizeof(int)*size, MPI_BYTE, 0,0,MPI_COMM_WORLD);
                MPI_Recv(&recv_row_index, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                row->index = recv_row_index;
            }
            free(row);
        }
    }
    MPI_Bcast(p,sizeof(Point), MPI_BYTE, 0, MPI_COMM_WORLD);
	
}


int main(int argc, char *argv[]) {
	if ( argc == 5 ) {
		X_RESN = atoi(argv[1]);
		Y_RESN = atoi(argv[2]);
		max_iteration = atoi(argv[3]);
        dynamic_flag = atoi(argv[4]);
        size = X_RESN;
	} else {
		X_RESN = 1000;
		Y_RESN = 1000;
		max_iteration = 100;
        dynamic_flag = 0;
	}

	if (rank == 0) {
		#ifdef GUI
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
		glutInitWindowSize(800, 800); 
		glutInitWindowPosition(0, 0);
		glutCreateWindow("MPI");
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(0, X_RESN, 0, Y_RESN);
		glutDisplayFunc(plot);
		#endif
	}

	/* computation part begin */
	MPI_Init(&argc, &argv);
    	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	
    if(rank == 0){
		initData();
        Point *p = data;
		t1 = std::chrono::high_resolution_clock::now();
        master(world_size,p);
        t2 = std::chrono::high_resolution_clock::now();  
		time_span = t2 - t1;
        MPI_Bcast(p,sizeof(Point),MPI_BYTE,0,MPI_COMM_WORLD);
	}else{
        slave(rank);
    }
	

	if (rank == 0){
		printf("Student ID: 118010246\n"); // replace it with your student id
		printf("Name: Qin Lan\n"); // replace it with your name
		printf("Assignment 2 MPI\n");
        printf("Dynamic state: %d\n",dynamic_flag);
		printf("Run Time: %f seconds\n", time_span.count());
		printf("Problem Size: %d * %d, %d\n", X_RESN, Y_RESN, max_iteration);
		printf("Process Number: %d\n", world_size);
        printf("\n");
        // printf("%f\n", time_span.count());
	}

	MPI_Finalize();
	/* computation part end */

	if (rank == 0){
		#ifdef GUI
		glutMainLoop();
		#endif
	}
    
	return 0;
}


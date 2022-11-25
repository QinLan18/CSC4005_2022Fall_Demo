#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <mpi.h>

#ifdef GUI
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "./headers/physics.h"
#include "./headers/logger.h"


int n_body;
int n_iteration;


int rank;
int world_size;

std::chrono::duration<double> total_time;

void generate_data(double *m, double *x,double *y,double *vx,double *vy, int n) {
    // TODO: Generate proper initial position and mass for better visualization
    srand((unsigned)time(NULL));
    for (int i = 0; i < n; i++) {
        m[i] = rand() % max_mass + 1.0f;
        // x[i] = 2000.0f + rand() % (bound_x / 4);
        // y[i] = 2000.0f + rand() % (bound_y / 4);
        x[i] = rand() % bound_x;
        y[i] = rand() % bound_y;
        vx[i] = 0.0f;
        vy[i] = 0.0f;
    }
}
void check_wall(double *x, double *y, double *vx, double *vy, int index){
    if(x[index] <= radius2){
        vx[index] = -vx[index];
        x[index] = radius2 + err;
    }else if(x[index] >= bound_x - radius2){
        vx[index] = -vx[index];
        x[index] = bound_x - radius2 - err;
    }
    if(y[index] <= radius2){
        vy[index] = -vy[index];
        y[index] = radius2 + err;
    }else if(y[index] >= bound_y - radius2){
        vy[index] = -vy[index];
        y[index] = bound_y - radius2 - err;
    }
}

void update_position(int index, double *x, double *y, double *vx, double *vy, int n) {
    //update position 
    check_wall(x,y,vx,vy,index);
    x[index] += vx[index] * dt;
    y[index] += vy[index] * dt;
}


void update_velocity(int index, double *m, double *x, double *y, double *vx, double *vy, int n) {
    //calculate force and acceleration, update velocity
    double ax = 0.0f;
    double ay = 0.0f;
    for(int j = 0; j < n; j++){
        // int signal = 1;
        if(index == j) continue;
        double dx = x[j] - x[index];
        double dy = y[j] - y[index];
        double d_square = dx * dx + dy * dy;
        
        //collision, v = -v, don't calculate force
        if(d_square <= 4*radius2*radius2) {
            d_square = 4*radius2 * radius2;

            // vx[i] = -vx[i];
            // vy[i] = -vy[i];
            // break;
        }
        // double d = sqrt(d_square);
        
        ax +=  gravity_const * m[j] * dx / (pow(d_square + err, 1.5));
        ay +=  gravity_const * m[j] * dy / (pow(d_square + err, 1.5));
    }
    vx[index] += ax * dt;
    vy[index] += ay * dt;
}


// void slave(){
//     // TODO: MPI routine
//     double* local_m;
//     double* local_x;
//     double* local_y;
//     double* local_vx;
//     double* local_vy;
//     // TODO End
// }



void master(int rank, int world_size) {
    double* total_m = new double[n_body];
    double* total_x = new double[n_body];
    double* total_y = new double[n_body];
    double* total_vx = new double[n_body];
    double* total_vy = new double[n_body];
    void* mpi_buffer=malloc(sizeof(double)*1000000);

	MPI_Buffer_attach(mpi_buffer,sizeof(double)*1000000);

    generate_data(total_m, total_x, total_y, total_vx, total_vy, n_body);
    int local_size = n_body/world_size;
    Logger l = Logger("mpi", n_body, bound_x, bound_y);

    for (int i = 0; i < n_iteration; i++){
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        // send data to other processes
        for(int j = 0; j < world_size; j++){
            if(j!= rank){
                MPI_Bsend(total_m + local_size*rank,sizeof(total_m)*local_size,MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD);
                MPI_Bsend(total_x + local_size*rank,8*local_size,MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD);
                MPI_Bsend(total_y + local_size*rank,8*local_size,MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD);
                MPI_Bsend(total_vx + local_size*rank,8*local_size,MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD);
                MPI_Bsend(total_vy + local_size*rank,8*local_size,MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD);
            }
        }
        // receive data from other processes
        for(int j = 0; j < world_size; j++){
            if(j != rank){
                MPI_Status status;
                MPI_Recv(total_m+local_size*rank,sizeof(total_m)*local_size, MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD,&status);
                MPI_Recv(total_x+local_size*rank,8*local_size, MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD,&status);
                MPI_Recv(total_y+local_size*rank,8*local_size, MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD,&status);
                MPI_Recv(total_vx+local_size*rank,8*local_size, MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD,&status);
                MPI_Recv(total_vy+local_size*rank,8*local_size, MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD,&status);
            }
        }
        
<<<<<<< HEAD
        //do computing
        for(int j = local_size*rank; j < local_size*(rank+1);j++){
            update_velocity(j,total_m,total_x,total_y,total_vx,total_vy,n_body);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        for(int j = local_size*rank; j < local_size*(rank+1);j++){
            update_position(j,total_x,total_y,total_vx,total_vy,n_body);
        }
        MPI_Barrier(MPI_COMM_WORLD);

          
=======
        // TODO End
>>>>>>> ec13d78b3723489de14cf9c8e9d2a9ffa8793bee

        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = t2 - t1;
        l.save_frame(total_x, total_y);
        total_time+=time_span;
        printf("Iteration %d, elapsed time: %.3f\n", i, time_span);

        l.save_frame(total_x, total_y);

        #ifdef GUI
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(1.0f, 0.0f, 0.0f);
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        double xi;
        double yi;
        for (int i = 0; i < n_body; i++){
            xi = total_x[i];
            yi = total_y[i];
            glVertex2f(xi, yi);
        }
        glEnd();
        glFlush();
        glutSwapBuffers();
        #else

        #endif
    }
    if(rank != 0){
        MPI_Send(total_m + local_size*rank,sizeof(total_m)*local_size,MPI_DOUBLE,0,rank,MPI_COMM_WORLD);
        MPI_Send(total_x + local_size*rank,sizeof(total_m)*local_size,MPI_DOUBLE,0,rank,MPI_COMM_WORLD);
        MPI_Send(total_y + local_size*rank,sizeof(total_m)*local_size,MPI_DOUBLE,0,rank,MPI_COMM_WORLD);
        MPI_Send(total_vx + local_size*rank,sizeof(total_m)*local_size,MPI_DOUBLE,0,rank,MPI_COMM_WORLD);
        MPI_Send(total_vy + local_size*rank,sizeof(total_m)*local_size,MPI_DOUBLE,0,rank,MPI_COMM_WORLD);
    }
    if(rank == 0){
        for(int i = 1; i < world_size; i++){
            MPI_Status status;
            MPI_Recv(total_m+local_size, sizeof(total_m)*local_size,MPI_DOUBLE,i,i,MPI_COMM_WORLD,&status);
            MPI_Recv(total_x+local_size, sizeof(total_m)*local_size,MPI_DOUBLE,i,i,MPI_COMM_WORLD,&status);
            MPI_Recv(total_y+local_size, sizeof(total_m)*local_size,MPI_DOUBLE,i,i,MPI_COMM_WORLD,&status);
            MPI_Recv(total_vx+local_size, sizeof(total_m)*local_size,MPI_DOUBLE,i,i,MPI_COMM_WORLD,&status);
            MPI_Recv(total_vy+local_size, sizeof(total_m)*local_size,MPI_DOUBLE,i,i,MPI_COMM_WORLD,&status);
        }
    
    } 

    

    delete[] total_m;
    delete[] total_x;
    delete[] total_y;
    delete[] total_vx;
    delete[] total_vy;

}




int main(int argc, char *argv[]) {
    n_body = atoi(argv[1]);
    n_iteration = atoi(argv[2]);

	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
	// if (my_rank == 0) {
		#ifdef GUI
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
		glutInitWindowSize(800, 800); 
		glutInitWindowPosition(0, 0);
		glutCreateWindow("N Body Simulation MPI Implementation");
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(0, bound_x, 0, bound_y);
		#endif
        // master(my_rank, world_size);
	// } else {
    //     slave();
    // }

    double* total_m = new double[n_body];
    double* total_x = new double[n_body];
    double* total_y = new double[n_body];
    double* total_vx = new double[n_body];
    double* total_vy = new double[n_body];
    void* mpi_buffer=malloc(sizeof(double)*1000000);

	MPI_Buffer_attach(mpi_buffer,sizeof(double)*1000000);

    generate_data(total_m, total_x, total_y, total_vx, total_vy, n_body);
    int local_size = n_body/world_size;
    Logger l = Logger("mpi", n_body, bound_x, bound_y);

    for (int i = 0; i < n_iteration; i++){
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        // send data to other processes
        for(int j = 0; j < world_size; j++){
            if(j!= rank){
                MPI_Bsend(total_m + local_size*rank,sizeof(total_m)*local_size,MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD);
                MPI_Bsend(total_x + local_size*rank,8*local_size,MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD);
                MPI_Bsend(total_y + local_size*rank,8*local_size,MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD);
                MPI_Bsend(total_vx + local_size*rank,8*local_size,MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD);
                MPI_Bsend(total_vy + local_size*rank,8*local_size,MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD);
            }
        }
        // receive data from other processes
        for(int j = 0; j < world_size; j++){
            if(j != rank){
                MPI_Status status;
                MPI_Recv(total_m+local_size*rank,sizeof(total_m)*local_size, MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD,&status);
                MPI_Recv(total_x+local_size*rank,8*local_size, MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD,&status);
                MPI_Recv(total_y+local_size*rank,8*local_size, MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD,&status);
                MPI_Recv(total_vx+local_size*rank,8*local_size, MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD,&status);
                MPI_Recv(total_vy+local_size*rank,8*local_size, MPI_DOUBLE,j,i*10+rank,MPI_COMM_WORLD,&status);
            }
        }
        
        //do computing
        for(int j = local_size*rank; j < local_size*(rank+1);j++){
            update_velocity(j,total_m,total_x,total_y,total_vx,total_vy,n_body);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        for(int j = local_size*rank; j < local_size*(rank+1);j++){
            update_position(j,total_x,total_y,total_vx,total_vy,n_body);
        }
        MPI_Barrier(MPI_COMM_WORLD);

          

        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = t2 - t1;
        total_time += time_span;
        l.save_frame(total_x, total_y);

        printf("Iteration %d, elapsed time: %.3f\n", i, time_span);

        #ifdef GUI
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(1.0f, 0.0f, 0.0f);
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        double xi;
        double yi;
        for (int i = 0; i < n_body; i++){
            xi = total_x[i];
            yi = total_y[i];
            glVertex2f(xi, yi);
        }
        glEnd();
        glFlush();
        glutSwapBuffers();
        #else

        #endif
    }
    if(rank != 0){
        MPI_Send(total_m + local_size*rank,sizeof(total_m)*local_size,MPI_DOUBLE,0,rank,MPI_COMM_WORLD);
        MPI_Send(total_x + local_size*rank,sizeof(total_m)*local_size,MPI_DOUBLE,0,rank,MPI_COMM_WORLD);
        MPI_Send(total_y + local_size*rank,sizeof(total_m)*local_size,MPI_DOUBLE,0,rank,MPI_COMM_WORLD);
        MPI_Send(total_vx + local_size*rank,sizeof(total_m)*local_size,MPI_DOUBLE,0,rank,MPI_COMM_WORLD);
        MPI_Send(total_vy + local_size*rank,sizeof(total_m)*local_size,MPI_DOUBLE,0,rank,MPI_COMM_WORLD);
    }
    if(rank == 0){
        for(int i = 1; i < world_size; i++){
            MPI_Status status;
            MPI_Recv(total_m+local_size*i, sizeof(total_m)*local_size,MPI_DOUBLE,i,i,MPI_COMM_WORLD,&status);
            MPI_Recv(total_x+local_size*i, sizeof(total_m)*local_size,MPI_DOUBLE,i,i,MPI_COMM_WORLD,&status);
            MPI_Recv(total_y+local_size*i, sizeof(total_m)*local_size,MPI_DOUBLE,i,i,MPI_COMM_WORLD,&status);
            MPI_Recv(total_vx+local_size*i, sizeof(total_m)*local_size,MPI_DOUBLE,i,i,MPI_COMM_WORLD,&status);
            MPI_Recv(total_vy+local_size*i, sizeof(total_m)*local_size,MPI_DOUBLE,i,i,MPI_COMM_WORLD,&status);
        }
    
    } 

    

    delete total_m;
    delete total_x;
    delete total_y;
    delete total_vx;
    delete total_vy;

	if (rank == 0){
		printf("Student ID: 118010246\n"); // replace it with your student id
		printf("Name: Qin Lan\n"); // replace it with your name
		printf("Assignment 3: N Body Simulation MPI Implementation\n");
        printf("Total time: %f; Average time: %f\n", total_time.count(), (total_time/(double)(n_iteration)).count());
	}

	MPI_Finalize();

	return 0;
}


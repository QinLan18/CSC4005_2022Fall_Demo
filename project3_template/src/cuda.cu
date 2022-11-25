#include <cuda.h>
#include <cuda_runtime.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#ifdef GUI
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "./headers/physics.h"
#include "./headers/logger.h"


int block_size = 512;


int n_body;
int n_iteration;
std::chrono::duration<double> total_time;


__global__ void update_position(double *x, double *y, double *vx, double *vy, int n) {
    //TODO: update position 
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i < n) {
    if(x[i] <= radius2){
        vx[i] = -vx[i];
        x[i] = radius2 + err;
    }else if(x[i] >= bound_x - radius2){
        vx[i] = -vx[i];
        x[i] = bound_x - radius2 - err;
    }
    if(y[i] <= radius2){
        vy[i] = -vy[i];
        y[i] = radius2 + err;
    }else if(y[i] >= bound_y - radius2){
        vy[i] = -vy[i];
        y[i] = bound_y - radius2 - err;
    }
    x[i] += vx[i] * dt;
    y[i] += vy[i] * dt;
    }
}

__global__ void update_velocity(double *m, double *x, double *y, double *vx, double *vy, int n) {
    //TODO: calculate force and acceleration, update velocity
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i < n) { 
        double ax = 0.0f;
        double ay = 0.0f;
        for(int j = 0; j < n; j++){
            // int signal = 1;
            if(index == j) continue;
            double dx = x[j] - x[i];
            double dy = y[j] - y[i];
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
        vx[i] += ax * dt;
        vy[i] += ay * dt; 
    }
}


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



void master() {
    double* m = new double[n_body];
    double* x = new double[n_body];
    double* y = new double[n_body];
    double* vx = new double[n_body];
    double* vy = new double[n_body];

    generate_data(m, x, y, vx, vy, n_body);

    Logger l = Logger("cuda", n_body, bound_x, bound_y);

    double *device_m;
    double *device_x;
    double *device_y;
    double *device_vx;
    double *device_vy;

    cudaMalloc(&device_m, n_body * sizeof(double));
    cudaMalloc(&device_x, n_body * sizeof(double));
    cudaMalloc(&device_y, n_body * sizeof(double));
    cudaMalloc(&device_vx, n_body * sizeof(double));
    cudaMalloc(&device_vy, n_body * sizeof(double));

    cudaMemcpy(device_m, m, n_body * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(device_x, x, n_body * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(device_y, y, n_body * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(device_vx, vx, n_body * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(device_vy, vy, n_body * sizeof(double), cudaMemcpyHostToDevice);

    int n_block = n_body / block_size + 1;
     
    for (int i = 0; i < n_iteration; i++){
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        update_velocity<<<n_block, block_size>>>(device_m, device_x, device_y, device_vx, device_vy, n_body);
        update_position<<<n_block, block_size>>>(device_x, device_y, device_vx, device_vy, n_body);

<<<<<<< HEAD
        cudaMemcpy(x, device_x, n_body*sizeof(double), cudaMemcpyDeviceToHost);
        cudaMemcpy(y, device_y, n_body*sizeof(double), cudaMemcpyDeviceToHost);

        
=======
        cudaMemcpy(x, device_x, n_body * sizeof(double), cudaMemcpyDeviceToHost);
        cudaMemcpy(y, device_y, n_body * sizeof(double), cudaMemcpyDeviceToHost);
>>>>>>> ec13d78b3723489de14cf9c8e9d2a9ffa8793bee

        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = t2 - t1;
        total_time += time_span;
        
        printf("Iteration %d, elapsed time: %.3f\n", i, time_span);
<<<<<<< HEAD
        l.save_frame(x, y);
=======

        l.save_frame(x, y);

>>>>>>> ec13d78b3723489de14cf9c8e9d2a9ffa8793bee
        #ifdef GUI
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(1.0f, 0.0f, 0.0f);
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        double xi;
        double yi;
        for (int i = 0; i < n_body; i++){
            xi = x[i];
            yi = y[i];
            glVertex2f(xi, yi);
        }
        glEnd();
        glFlush();
        glutSwapBuffers();
        #else

        #endif

    }

    cudaFree(device_m);
    cudaFree(device_x);
    cudaFree(device_y);
    cudaFree(device_vx);
    cudaFree(device_vy);

    delete[] m;
    delete[] x;
    delete[] y;
    delete[] vx;
    delete[] vy;
    
}


int main(int argc, char *argv[]){
    
    n_body = atoi(argv[1]);
    n_iteration = atoi(argv[2]);

    #ifdef GUI
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(500, 500);
    glutCreateWindow("N Body Simulation CUDA Implementation");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gluOrtho2D(0, bound_x, 0, bound_y);
    #endif

    master();

    printf("Student ID: 118010246\n"); // replace it with your student id
    printf("Name: Qin Lan\n"); // replace it with your name
    printf("Assignment 3: N Body Simulation CUDA Implementation\n");
    printf("Total time: %f; Average time: %f\n", total_time.count(), (total_time/(double)(n_iteration)).count());
    

    return 0;

}



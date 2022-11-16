#include <omp.h>
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


int n_body;
int n_iteration;

int n_omp_threads;
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

void update_position(double *x, double *y, double *vx, double *vy, int index) {
    
    //update position 
    check_wall(x,y,vx,vy,index);
    x[index] += vx[index] * dt;
    y[index] += vy[index] * dt;

}

void update_velocity(double *m, double *x, double *y, double *vx, double *vy, int index) {
    //TODO: calculate force and acceleration, update velocity
    double ax = 0.0f;
    double ay = 0.0f;
    for(int j = 0; j < n_body; j++){
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


void master() {
    double* m = new double[n_body];
    double* x = new double[n_body];
    double* y = new double[n_body];
    double* vx = new double[n_body];
    double* vy = new double[n_body];

    generate_data(m, x, y, vx, vy, n_body);

    Logger l = Logger("sequential", n_body, bound_x, bound_y);

    for (int i = 0; i < n_iteration; i++){
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        
        //TODO: choose better threads configuration
        omp_set_num_threads(n_omp_threads);
        #pragma omp parallel for
        for (int i = 0; i < n_body; i++) {
            update_velocity(m, x, y, vx, vy, i);
        }

        omp_set_num_threads(8);
        #pragma omp parallel for
        for (int i = 0; i < n_body; i++) {
            update_position(x, y, vx, vy, i);
        }

       

        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = t2 - t1;
        total_time += time_span;
         l.save_frame(x, y);
        printf("Iteration %d, elapsed time: %.3f\n", i, time_span);

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

    delete m;
    delete x;
    delete y;
    delete vx;
    delete vy;
    
}


int main(int argc, char *argv[]){
    
    n_body = atoi(argv[1]);
    n_iteration = atoi(argv[2]);
    n_omp_threads = atoi(argv[3]);

    #ifdef GUI
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(500, 500);
    glutCreateWindow("N Body Simulation Sequential Implementation");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gluOrtho2D(0, bound_x, 0, bound_y);
    #endif
    master();

    printf("Student ID: 118010246\n"); // replace it with your student id
    printf("Name: QinLan\n"); // replace it with your name
    printf("Assignment 3: N Body Simulation OpenMP Implementation\n");
    printf("Total time: %f; Average time: %f\n", total_time.count(), (total_time/(double)(n_iteration)).count());
    return 0;

}



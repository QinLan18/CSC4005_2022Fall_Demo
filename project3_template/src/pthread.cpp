#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <pthread.h>

#ifdef GUI
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "./headers/physics.h"
#include "./headers/logger.h"

int n_thd; // number of threads

int n_body;
int n_iteration;
pthread_mutex_t mutex;
pthread_cond_t processing, iter_fin;
bool finish = false;
int mutex_jobs = 0, n_done = 0;
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
    //TODO: calculate force and acceleration, update velocity
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


typedef struct {
    //TODO: specify your arguments for threads
    //int a;
    //int b;
    //TODO END
    double * m;
    double *x;
    double *y;
    double *vx;
    double *vy;

} Args;


void* worker(void* args) {
    //TODO: procedure in each threads

    Args* my_arg = (Args*) args;
    double*m = my_arg->m;
    double*x = my_arg->x;
    double*y = my_arg->y;
    double*vx = my_arg->vx;
    double*vy = my_arg->vy;
    // int a = my_arg->a;
    // int b = my_arg->b;

    // TODO END
    while(true){
        pthread_mutex_lock(&mutex);
        while(!finish && mutex_jobs <= 0)
            pthread_cond_wait(&processing, &mutex);
        int i = --mutex_jobs;
        pthread_mutex_unlock(&mutex);
        if(finish) break;
        update_velocity(i,m,x,y,vx,vy,n_body);
        update_position(i,x,y,vx,vy,n_body);
        pthread_mutex_lock(&mutex);
        n_done++;
        if(n_done >= n_body) pthread_cond_signal(&iter_fin);
        pthread_mutex_unlock(&mutex);
    }

    

}


void master(){
    double* m = new double[n_body];
    double* x = new double[n_body];
    double* y = new double[n_body];
    double* vx = new double[n_body];
    double* vy = new double[n_body];

    generate_data(m, x, y, vx, vy, n_body);

    Logger l = Logger("pthread", n_body, bound_x, bound_y);
    pthread_t workers[n_thd];
    Args args[n_thd];
    for(int i = 0; i < n_thd; i++){
        args[i].x = x;
        args[i].y = y;
        args[i].m = m;
        args[i].vx = vx;
        args[i].vy = vy;
        
    }
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&iter_fin, NULL);
    pthread_cond_init(&processing, NULL);

    // mutex_jobs = n_body, n_done = 0;
    for(int i = 0; i < n_thd; i++){
        pthread_create(&workers[i], NULL, worker, &args[i]);
    }


    for (int i = 0; i < n_iteration; i++){
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        pthread_mutex_lock(&mutex);
        mutex_jobs = n_body, n_done = 0;
        pthread_cond_broadcast(&processing);
        pthread_cond_wait(&iter_fin, &mutex);
        pthread_mutex_unlock(&mutex);


        

        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = t2 - t1;
        total_time+=time_span;
        printf("Iteration %d, elapsed time: %.3f\n", i, time_span);
        l.save_frame(x, y);

        l.save_frame(x, y);

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
    finish = true;
    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&processing);
    pthread_mutex_unlock(&mutex);
    for(int j = 0; j < n_thd; j++){
        pthread_join(workers[j], NULL);
    }
    delete m;
    delete x;
    delete y;
    delete vx;
    delete vy;


}


int main(int argc, char *argv[]) {
    n_body = atoi(argv[1]);
    n_iteration = atoi(argv[2]);
    n_thd = atoi(argv[3]);

    #ifdef GUI
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Pthread");
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, bound_x, 0, bound_y);
    #endif
    master();
    printf("Student ID: 118010246\n"); // replace it with your student id
    printf("Name: Qin Lan\n"); // replace it with your name
    printf("Assignment 3: N Body Simulation Pthread Implementation\n");
    printf("Total time: %f; Average time: %f\n", total_time.count(), (total_time/(double)(n_iteration)).count());
	return 0;
}


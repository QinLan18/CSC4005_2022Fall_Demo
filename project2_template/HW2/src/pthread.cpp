#include "asg2.h"
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;

int n_thd; // number of threads
int dynamic_flag = 0;
int size = 1000;
int curr_row = 0;


typedef struct {
    Point* p;
    int thread_num;
    int tid;
    
} Args;

int getLength(const int &size, const int&thread_num, const int &tid){
    if(size < (thread_num)) return (tid)<size;
    return (size-tid)/thread_num + ((size-tid)%thread_num>0);
}

void* worker(void* args) {
    
    Args* my_arg = (Args*) args;
    int thread_num = my_arg->thread_num;
    int tid = my_arg->tid;
    Point* p = my_arg->p;
    if(!dynamic_flag){
        int length = getLength(Y_RESN,thread_num,tid);
        int start = 0;
        for(int i = 0; i < tid; i++) start += getLength(Y_RESN, thread_num, i);
        for(int i = start; i < start+length;i++){
            for(int j = 0; j < X_RESN; j++){
                p->x = j;
                p->y = i;
                compute(p);
                p++;
            }
        }
    }
    else{
        int my_row;
        pthread_mutex_lock(&mutex);
        while(curr_row<Y_RESN-1){
            my_row = curr_row++;
            pthread_mutex_unlock(&mutex);
            for(int j = 0; j < X_RESN; j++){
                p->x = j;
                p->y = my_row;
                compute(p);
                p++;
            }
            pthread_mutex_lock(&mutex);
            
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;


}

void master(int X_RESN, int Y_RESN,int max_iteration,int size){

}


int main(int argc, char *argv[]) {

	if ( argc == 6 ) {
		X_RESN = atoi(argv[1]);
		Y_RESN = atoi(argv[2]);
		max_iteration = atoi(argv[3]);
        n_thd = atoi(argv[4]);
        dynamic_flag = atoi(argv[5]);
	} else {
		X_RESN = 1000;
		Y_RESN = 1000;
		max_iteration = 100;
        n_thd = 4;
	}

    #ifdef GUI
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Pthread");
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, X_RESN, 0, Y_RESN);
	glutDisplayFunc(plot);
    #endif

    /* computation part begin */
    

    initData();
    Point * p = data;
    t1 = std::chrono::high_resolution_clock::now();
    //TODO: assign jobs
    pthread_mutex_init(&mutex, NULL);

    pthread_t thds[n_thd]; // thread pool
    Args args[n_thd]; // arguments for all threads
    for (int thd = 0; thd < n_thd; thd++){
        
        args[thd].thread_num = n_thd;
        args[thd].tid = thd;
        args[thd].p = p;
    }
    for (int thd = 0; thd < n_thd; thd++) pthread_create(&thds[thd], NULL, worker, &args[thd]);
    for (int thd = 0; thd < n_thd; thd++) pthread_join(thds[thd], NULL);

    pthread_mutex_destroy(&mutex);
    //TODO END

    // master(X_RESN,Y_RESN,max_iteration,size);

    t2 = std::chrono::high_resolution_clock::now();  
    time_span = t2 - t1;
    /* computation part end */

    printf("Student ID: 118010246\n"); // replace it with your student id
    printf("Name: Qin Lan\n"); // replace it with your name
    printf("Assignment 2 Pthread\n");
    printf("Run Time: %f seconds\n", time_span.count());
    printf("Problem Size: %d * %d, %d\n", X_RESN, Y_RESN, max_iteration);
    printf("Thread Number: %d\n", n_thd);
    printf("\n");
    
    #ifdef GUI
	glutMainLoop();
    #endif

	return 0;
}


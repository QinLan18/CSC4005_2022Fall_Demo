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


int size; // problem size


int my_rank;
int world_size;
MPI_Status status;
int n_iteration = 10;


void initialize(float *data) {
    // intialize the temperature distribution
    int len = size * size;
    for (int i = 0; i < len; i++) {
        data[i] = wall_temp;
    }
}


void generate_fire_area(bool *fire_area){
    // generate the fire area
    int len = size * size;
    for (int i = 0; i < len; i++) {
        fire_area[i] = 0;
    }

    float fire1_r2 = fire_size * fire_size;
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            int a = i - size / 2;
            int b = j - size / 2;
            int r2 = 0.5 * a * a + 0.8 * b * b - 0.5 * a * b;
            if (r2 < fire1_r2) fire_area[i * size + j] = 1;
        }
    }

    float fire2_r2 = (fire_size / 2) * (fire_size / 2);
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            int a = i - 1 * size / 3;
            int b = j - 1 * size / 3;
            int r2 = a * a + b * b;
            if (r2 < fire2_r2) fire_area[i * size + j] = 1;
        }
    }
}


void update(float *data, float *new_data, int begin, int end) {
    // TODO: update the temperature of each point, and store the result in `new_data` to avoid data racing
    for(int i = begin; i < end; i++){
        for(int j = 0; j < size; j ++){
            if(i == begin || i == end) continue;
            int idx = i * size + j;
            float up = data[idx - size];
            float down = data[idx + size];
            float left = data[idx - 1];
            float right = data[idx + 1];
            float new_val = (up + down + left + right) / 4;
            new_data[idx] = new_val;
        }
    }
    
}


void maintain_fire(float *data, bool* fire_area, int begin, int end) {
    // TODO: maintain the temperature of fire
    int len = (end - begin)*size;
    for (int i = begin*size; i < end * size; i++){
        if (fire_area[i]) data[i] = fire_temp;
    }
}\


void maintain_wall(float *data, int begin, int end) {
    // TODO: maintain the temperature of the wall
    int data_length = ( end - begin )* size;
    for (int i = 0; i < size; i++){
        if (begin==0) data[i] = wall_temp;
        if(end==size) data[data_length - i] = wall_temp;
        data[i * size] = wall_temp;
        data[i * size + 1] = wall_temp;
    }
}


#ifdef GUI
void data2pixels(float *data, GLubyte* pixels, int begin, int end){
    // convert rawdata (large, size^2) to pixels (small, resolution^2) for faster rendering speed
    float factor_data_pixel = (float) size / resolution;
    float factor_temp_color = (float) 255 / fire_temp;
    for (int x = 0; x < resolution; x++){
        for (int y = 0; y < resolution; y++){
            int idx = x * resolution + y;
            int idx_pixel = idx * 3;
            int x_raw = x * factor_data_pixel;
            int y_raw = y * factor_data_pixel;
            int idx_raw = y_raw * size + x_raw;
            float temp = data[idx_raw];
            int color =  ((int) temp / 5 * 5) * factor_temp_color;
            pixels[idx_pixel] = color;
            pixels[idx_pixel + 1] = 255 - color;
            pixels[idx_pixel + 2] = 255 - color;
        }
    }
}

void plot(GLubyte* pixels){
    // visualize temprature distribution
    #ifdef GUI
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(resolution, resolution, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glutSwapBuffers();
    #endif
}
#endif



void slave(){
    // TODO: MPI routine (one possible solution, you can use another partition method)
    int my_begin_row_id = size * my_rank / (world_size);
    int my_end_row_id = size * (my_rank + 1) / world_size;
    int local_size = my_end_row_id-my_begin_row_id;

    // TODO: Initialize a storage for temperature distribution of this area
    float* local_data = new float[local_size*size];
    bool* fire_area = new bool[size * size];
    // float* local_data_odd;
    // float* local_data_even;
    // local_data_odd = new float[local_size*size];
    // local_data_even = new float[local_size*size];

    // TODO: Receive initial temperature distribution of this area from master
    MPI_Recv(local_data,local_size*size*sizeof(float), MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
    printf("slave %d receive initial temp.", my_rank);
    MPI_Recv(fire_area, size*size*sizeof(float), MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);

    // TODO: Initialize a storage for local pixels (pls refer to sequential version for initialization of GLubyte)
    #ifdef GUI
    GLubyte* local_pixels = new GLubyte[resolution * resolution * 3 / world_size];
    #endif

    /*
    bool cont = true;
    int count = 1;
    while (count <= n_iteration) {
        // TODO: computation part
        update(local_data,local_data,my_begin_row_id,my_end_row_id);
        maintain_fire(local_data,fire_area, my_begin_row_id, my_end_row_id);
        maintain_wall(local_data, my_begin_row_id,my_end_row_id);
        
        // TODO: after computation, send border row to neighbours
        //end row to neighbor
        // if(my_rank < world_size - 1) {
        //     MPI_Send(local_data+(local_size-1)*size, size*sizeof(float), MPI_FLOAT, my_rank+1,0,MPI_COMM_WORLD);
        // }
        // //begin row to neighbor
        // if(my_rank > 0){
        //     MPI_Send(local_data, size*sizeof(float), MPI_FLOAT, my_rank-1,0,MPI_COMM_WORLD);
        // }
        //send result to master
        MPI_Send(local_data, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
        count ++;

        #ifdef GUI
        // TODO: conver raw temperature to pixels (much smaller than raw data)
        data2pixels(local_data, local_pixels);
        // TODO: send pixels to master (you can use MPI_Byte to transfer anything to master, then you won't need to declare MPI Type :-) )
        MPI_Send(local_pixels, resolution*resolution*3/world_size*sizeof(MPI_BYTE),MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
        #endif

    }
    */

    #ifdef GUI
    data2pixels(local_data, local_pixels);
    #endif

    // TODO: Remember to delete[] local_data and local_pixcels.
    delete[] local_data;
    #ifdef GUI
    delete[] local_pixcels;
    #endif
}



void master() {
    // TODO: MPI routine (one possible solution, you can use another partition method)
    float* data_odd = new float[size * size];
    float* data_even = new float[size * size];
    bool* fire_area = new bool[size * size];

    initialize(data_odd);
    generate_fire_area(fire_area);

    #ifdef GUI
    GLubyte* pixels;
    pixels = new GLubyte[resolution * resolution * 3];
    #endif

    int count = 1;
    double total_time = 0;

    printf("initialized.\n");

    // TODO: Send initial distribution to each slave process
    
   
    for(int i = 0; i < world_size; i++){
        int my_begin_row_id = size * i / (world_size);
        int my_end_row_id = size * (i + 1) / world_size;
        int local_size = my_end_row_id-my_begin_row_id;
        printf("distrubited.\n");
        MPI_Send(data_odd+my_begin_row_id*size, 1 ,MPI_FLOAT, i, 0, MPI_COMM_WORLD);
        // MPI_Send(fire_area,1,MPI_FLOAT, i, 0, MPI_COMM_WORLD);
        // MPI_Sendrecv(data_odd,local_size*size*(sizeof(float)), MPI_FLOAT, i, 0, )
        printf("%d send row (%d) - (%d) to %d \n", my_rank, my_begin_row_id, my_end_row_id,i);
    }
    
    
    /*

    while (count <= n_iteration) {
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        // TODO: Computation of my part
        if(count % 2 == 1){
            MPI_Gather(data_odd, 1, MPI_FLOAT, data_even, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
            printf("gather data in iteration %d\n", count);
        }
        else{
            MPI_Gather(data_even, 1, MPI_FLOAT, data_odd, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
            printf("gather data in iteration %d\n", count);
        }
        
        // TODO: Send border row to neighbours

        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        double this_time = std::chrono::duration<double>(t2 - t1).count();
        total_time += this_time;
        printf("Iteration %d, elapsed time: %.6f\n", count, this_time);
        count++;

        #ifdef GUI
        if (count % 2 == 1) {
            // TODO: Gather pixels of slave processes
            MPI_Recv();
            data2pixels(data_even, pixels);
        } else {
            // TODO: Gather pixels of slave processes
            data2pixels(data_odd, pixels);
        }
        plot(pixels);
        #endif
    }
    */

    delete[] data_odd;
    delete[] data_even;
    delete[] fire_area;

    #ifdef GUI
    delete[] pixels;
    #endif
}




int main(int argc, char *argv[]) {
    size = atoi(argv[1]);

	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);


	if (my_rank == 0) {
        #ifdef GUI
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
        glutInitWindowPosition(0, 0);
        glutInitWindowSize(window_size, window_size);
        glutCreateWindow("Heat Distribution Simulation MPI Implementation");
        gluOrtho2D(0, resolution, 0, resolution);
        #endif
        master();
	} else {
        slave();
    }

	if (my_rank == 0){
		printf("Student ID: 118010246\n"); 
		printf("Name: Lan Qin\n");
		printf("Assignment 4: Heat Distribution Simulation MPI Implementation\n");
	}

	MPI_Finalize();

	return 0;
}


#include <cstdlib>
#include <fstream>
#include <iostream>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace chrono;




int main (int argc, char **argv){

    int num_elements; // number of elements to be sorted
    num_elements = atoi(argv[1]); // convert command line argument to num_elements

    int elements[num_elements]; // store elements
    int sorted_elements[num_elements]; // store sorted elements

    ifstream input(to_string(num_elements) + string("input.in"));
    int element;
    int i = 0;
    while (input >> element) {
        elements[i] = element;
        i++;
    }
    cout << "actual number of elements:" << i << std::endl;

    
    
    auto t1 = system_clock::now(); // record time

    sort(elements, elements+num_elements);


    auto t2 = system_clock::now(); 
    for (int i = 0; i < num_elements; i++) {
        sorted_elements[i] = elements[i];
    } 
    auto time_span = duration_cast<microseconds>(t2 - t1);
    std::cout << "Student ID: " << "118010246" << std::endl; // replace it with your student id
    std::cout << "Name: " << "Qin Lan" << std::endl; // replace it with your name
    std::cout << "Assignment 1" << std::endl;
    std::cout << "Run Time: " << time_span.count() << " microseconds" << std::endl;
    std::cout << "Input Size: " << num_elements << std::endl;
    std::cout << "Process Number: " << 1 << std::endl;   
    
    ofstream output(to_string(num_elements) +"sequential.out", ios_base::out);
    for (int i = 0; i < num_elements; i++) {
        output << sorted_elements[i] << std::endl;
    }
    
    return 0;
}

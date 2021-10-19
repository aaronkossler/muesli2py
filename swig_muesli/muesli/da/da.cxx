/* File: example.c */

#include "da.h"
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <omp.h>
#include <mpi.h>

da::da(int c_size){
    size = c_size;
    elements = new int[size];
}
da::da(int c_size, int value){
    size = c_size;
    elements = new int[size];
    fill(value);
}

da::~da(){
  printf("Destructor\n");
}

int da::get_size() {
    return size;
}
void da::fill(int value) {
    for (int i = 0; i < size; i++) elements[i] = value;
}
void da::printarray() {
    printf("[");
    for (int i = 0; i< size; i++) printf("%d;", elements[i]);
    printf("]");
}

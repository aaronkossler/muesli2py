/* File: example.c */

#include "da.h"
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <omp.h>
#include <mpi.h>

dista::dista(){
  printf("Constructor\n");
}

dista::~dista(){
  printf("Destructor\n");
}

int dista::get_size() {
    return size;
}
void dista::set_size(int set_size) {
    size = set_size;
}

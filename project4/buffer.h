#ifndef _BUFFER_H_DEFINED_
#define _BUFFER_H_DEFINED_

#include <pthread.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

typedef int buffer_item;

#define BUFFER_SIZE 5

// Global Variables
sem_t empty, full;
pthread_mutex_t mutex;
buffer_item buffer[BUFFER_SIZE] = {-1,-1,-1,-1,-1};
bool simulation_running = true;
int producer_index = 0;
int consumer_index = 0;
int buffer_empty_count = 0;
int buffer_full_count = 0;
int filled_buffer_count = 0;
int empty_buffer_count = 0;
bool print_steps = false;


void print_buffer();

void buffer_insert_item( buffer_item item );

void buffer_remove_item();

void* producer(void *args);

void *consumer(void* args);

void buffer_init();

bool is_prime(int num);

#endif 

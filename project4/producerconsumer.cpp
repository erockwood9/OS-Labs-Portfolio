#include "buffer.h"

//***********************************************************************
//
// Main Function
//
// Entry point for the producer/consumer simulation. Parses command-line
// arguments, starts producer and consumer threads, runs the simulation
// for the requested time, then signals shutdown and gathers per-thread
// statistics before printing a final report.
//
//***********************************************************************

int main(int argc, char* argv[]) {

    buffer_init();

    // Parse command line arguments
    int num_producers = atoi(argv[3]);
    int num_consumers = atoi(argv[4]);
    int max_sleep_time = atoi(argv[2]);
    int simulation_time = atoi(argv[1]);
    if (std::string(argv[5]) == "yes")
         print_steps = true;


    // Create producer and consumer threads

    std::cout << "Starting Threads" << std::endl;
    if (print_steps == true) print_buffer();
    pthread_t producer_thread[num_producers], consumer_thread[num_consumers];
    for (int i = 0; i < num_producers; i++)
    {
        pthread_create(&producer_thread[i], nullptr, producer, &max_sleep_time);
    }

    for (int i = 0; i < num_consumers; i++)
    {
        pthread_create(&consumer_thread[i], nullptr, consumer, &max_sleep_time);
    }

    // Wait for both threads to finish
    usleep(simulation_time*1000000);
    simulation_running = false; // Signal threads to stop
    /* Wake any threads blocked on semaphores so they can exit */
    for (int i = 0; i < num_producers; ++i) sem_post(&empty);
    for (int i = 0; i < num_consumers; ++i) sem_post(&full);

    void* produced_counts[num_producers];
    void* consumed_counts[num_consumers];
    for (int i = 0; i < num_producers; i++)
    {
        pthread_join(producer_thread[i], &produced_counts[i]);
    }

    for (int i = 0; i < num_consumers; i++)
    {
        pthread_join(consumer_thread[i], &consumed_counts[i]);
    }

    // Clean up mutex and condition variables
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    // Display stats
    std::cout << "PRODUCER / CONSUMER SIMULATION COMPLETE" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "Simulation Time:                            " << simulation_time << std::endl;
    std::cout << "Maximim Thread Sleep Time:                  " << max_sleep_time << std::endl;
    std::cout << "Number of Producer Threads:                 " << num_producers << std::endl;
    std::cout << "Number of Consumer Threads:                 " << num_consumers << std::endl;
    std::cout << "Size of Buffer                              5" << std::endl << std::endl;

    int total_produced = 0;
    for (int i = 0; i < num_producers; i++) {
        total_produced += *((int*)produced_counts[i]);
    }
    std::cout << "Total Number of Items Produced: " << total_produced << std::endl;
    for (int i = 0; i < num_producers; i++) {
        std::cout << "    Producer " << i+1 << ": " << *((int*)produced_counts[i]) << std::endl;
    }
    std::cout << std::endl;

    int total_consumed = 0;
    for (int i = 0; i < num_consumers; i++) {
        total_consumed += *((int*)consumed_counts[i]);
    }
    std::cout << "Total Number of Items Consumed:  " << total_consumed << std::endl;
    for (int i = 0; i < num_consumers; i++) {
        std::cout << "    Consumer " << i+1 << ": " << *((int*)consumed_counts[i]) << std::endl;
    }
    std::cout << std::endl;
    // sem_getvalue(&full, &buffer_full_count);
    std::cout << "Number of Items Remaining in Buffer         " << buffer_full_count << std::endl;
    std::cout << "Number of Times Buffer was Full             " << filled_buffer_count << std::endl;
    std::cout << "Number of Times Buffer was Empty            " << empty_buffer_count << std::endl;
    std::cout << " " << std::endl;

    return 0;
}

//***********************************************************************
//
// producer
//
// Producer thread function: repeatedly sleeps for a random interval
// (bounded by `max_sleep_time`), generates a random item, waits for an
// empty slot (`sem_wait(&empty)`), takes the mutex to update the shared
// buffer via `buffer_insert_item`, and signals `full` when done.
//
//***********************************************************************

void* producer(void *max_sleep_time) {
    int *produced_count = new int(0);
    while (simulation_running) {
        unsigned int seed = pthread_self();
        int rand = rand_r(&seed) % *static_cast<int*>(max_sleep_time) + 1;
        usleep(rand*1000000);
        buffer_item num = rand_r(&seed) % 100 + 1;
        sem_getvalue(&empty, &buffer_empty_count);
        if (buffer_empty_count == 0 && print_steps == true) {
            std::cout << "All buffers full. Producer " << pthread_self() << " waits" << std::endl;
        }
        // Block until an empty slot is available. This is where the
        // producer will wait if the buffer is full.
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer_insert_item(num);
        (*produced_count)++;
        pthread_mutex_unlock(&mutex);
        // Signal that a new item is available to consumers.
        sem_post(&full);
        // Update diagnostic counters after posting to reflect the
        // new semaphore state; this is not used for synchronization. 
        sem_getvalue(&full, &buffer_full_count);
        if (buffer_full_count == BUFFER_SIZE) {
            filled_buffer_count++;
        }
        if (print_steps == true)
            print_buffer();
    }
    return produced_count;
}

//***********************************************************************
//
// consumer
//
// Consumer thread function: sleeps for a random interval, waits for an
// occupied slot, takes the mutex to remove an item
// from the shared buffer via `buffer_remove_item`, and signals `empty`
// when done.
//
//***********************************************************************

void* consumer(void *max_sleep_time) {
    int *consumed_count = new int(0);
    while (simulation_running) {
        unsigned int seed = pthread_self();
        int rand = rand_r(&seed) % *static_cast<int*>(max_sleep_time) + 1;
        usleep(rand*1000000);
        sem_getvalue(&full, &buffer_full_count);
        if (buffer_full_count == 0 && print_steps == true) {
            std::cout << "All buffers are empty. Consumer " << pthread_self() << " waits" << std::endl;
        }
        // Block until an item is available for consumption.
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        buffer_remove_item();
        (*consumed_count)++;
        pthread_mutex_unlock(&mutex);
        // Signal that a slot became empty after removing an item.
        sem_post(&empty);
        // Update diagnostics and optionally print the buffer. 
        sem_getvalue(&empty, &buffer_empty_count);
        if (buffer_empty_count == BUFFER_SIZE) {
            empty_buffer_count++;
        }
        sem_getvalue(&full, &buffer_full_count);
        if (print_steps == true)
            print_buffer();
    }
    return consumed_count;
}

void buffer_init() {
    // Initialize semaphores and mutex.
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);
}

//***********************************************************************
//
// buffer_insert_item
//
// Insert `num` into the buffer at `producer_index` and advance
// the index. This function does not perform synchronization itself and
// must be called while holding the mutex.
//
//***********************************************************************
void buffer_insert_item( buffer_item num )
{
    buffer[producer_index] = num;
    if (print_steps == true)
    {
        std::cout << "Producer " << pthread_self() << " writes " << num << std::endl;
    }
    producer_index = (producer_index + 1) % BUFFER_SIZE;
    return;
}

//***********************************************************************
//
// buffer_remove_item
//
// Logically remove the item at `consumer_index` by advancing the index.
// This function prints the removed item when `print_steps` is true.
//
// Return Value
// void                      no return value
//
//***********************************************************************
void buffer_remove_item()
{
    if (print_steps == true)
    {
        std::cout << "Consumer " << pthread_self() << " reads " << buffer[consumer_index];
        if (is_prime(buffer[consumer_index])) {
            std::cout << "   * * * PRIME * * *";
        }
        std::cout << std::endl;
    }
    consumer_index = (consumer_index + 1) % BUFFER_SIZE;
    return;
}

//***********************************************************************
//
// is_prime
//
// Check whether `num` is a prime number using a simple division
// algorithm.
//
//***********************************************************************
bool is_prime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i <= num / 2; i++) {
        if (num % i == 0) return false;
    }
    return true;
}

//***********************************************************************
//
// print_buffer
//
// Diagnostic helper that prints the current state of the
// buffer. The function samples the `full` semaphore for a
// quick count of occupied slots; this is only used for display and is
// not relied on for synchronization correctness.
//
//***********************************************************************
void print_buffer() {
    sem_getvalue(&full, &buffer_full_count);
    std::cout << "(Buffers Occupied: " << buffer_full_count << ")" << std::endl;
    std::cout << "Buffers: ";
    for (int i = 0; i < BUFFER_SIZE; i++) {
        std::cout << buffer[i] << "   ";
    }
    std::cout << std::endl << "         ";
    for (int i = 0; i <BUFFER_SIZE; i++) {
        std::cout << "---  "; 
    }
    std::cout << std::endl << "         ";
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (i == producer_index && i == consumer_index) {
            std::cout << "WR  ";
        }
        else if (i == producer_index) {
            std::cout << " W   ";
        }
        else if (i == consumer_index) {
            std::cout << " R   ";
        }
        else {
            std::cout << "     ";
        }
    }
    std::cout << std::endl << std::endl;
}
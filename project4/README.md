# Producer/Consumer Simulation with Threads and Semaphores

This project implements a **multi-threaded producer/consumer simulation** in C++ using POSIX threads (`pthread`) and semaphores (`sem_t`). It demonstrates concurrent access to a shared buffer with proper synchronization using mutexes and semaphores.

---

## Features

- **Configurable Simulation**  
  Command-line arguments allow setting:
  - Simulation duration
  - Maximum thread sleep time
  - Number of producer and consumer threads
  - Optional step-by-step printing

- **Circular Buffer Implementation**  
  - Fixed-size buffer (`BUFFER_SIZE = 5`)  
  - Tracks number of full and empty occurrences  
  - Each producer inserts items and each consumer removes them

- **Synchronization**
  - Uses **mutex** for mutual exclusion
  - Uses **semaphores** to track empty/full slots
  - Ensures safe concurrent access to shared buffer

- **Diagnostics**
  - Optional step-by-step buffer printouts
  - Marks items that are prime numbers
  - Final simulation report shows:
    - Total items produced/consumed
    - Per-thread statistics
    - Buffer usage statistics

---

## Build

Compile with pthread and semaphore support:

```bash
g++ -pthread -o producerconsumer producerconsumer.cpp buffer.h
```
## Usage

```bash
./producerconsumer <simulation_time> <max_sleep_time> <num_producers> <num_consumers> <print_steps>
```
- simulation_time — total simulation duration in seconds
- max_sleep_time — maximum sleep time per thread in seconds
- num_producers — number of producer threads
- num_consumers — number of consumer threads
- print_steps — "yes" to print buffer steps, "no" to disable
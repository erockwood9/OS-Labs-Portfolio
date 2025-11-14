# Disk Scheduling Simulator in C++

This program simulates multiple **disk scheduling algorithms** for servicing cylinder requests in a hard drive. The supported algorithms are:

- **FCFS** — First-Come-First-Serve  
- **SSTF** — Shortest-Seek-Time-First  
- **SCAN** — Elevator (head moves to end of disk and reverses)  
- **CSCAN** — Circular SCAN (head moves to end, then jumps to beginning)  
- **LOOK** — Like SCAN, but only goes as far as the last request  
- **CLOOK** — Circular LOOK, jumps from last request back to first  

The simulator generates a **random list of cylinder requests** and calculates the **total head movement** for each algorithm.

## Usage

Compile the program:

```bash
g++ diskscheduler.cpp -o diskscheduler
```

## Usage

```bash
./disk_scheduler <starting_head>
```
- Starting head value must be between 0 and 2999
# Fibonacci Generator with Threads

This program computes the first N Fibonacci numbers using a separate worker thread (POSIX threads). The results are stored in a global array and printed after the computation completes.

## Features

- Uses `pthread` to run a worker thread for Fibonacci computation.
- Accepts user input for how many numbers to generate (up to 90).
- Stores Fibonacci numbers in a global array.
- Demonstrates basic thread creation, joining, and attribute setup.

## Build

Compile with `gcc` and pthread support:

```bash
gcc -pthread -o fibthread fibthread.c

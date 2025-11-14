# Vowel and Consonant Word Printer with Threads

This program reads a line of text from the user and prints each word using two separate threads:

- **Vowel thread** prints words that start with a vowel.
- **Consonant thread** prints words that start with a consonant.

It demonstrates thread creation, synchronization using a mutex, and cooperative scheduling with `sched_yield()`.

## Features

- Uses POSIX threads (`pthread`) for concurrent word processing.
- Synchronizes access to shared data (`words` vector and `index`) with a mutex.
- Prints words in order, but separates vowel-starting and consonant-starting words across threads.

## Build

Compile with `g++` and pthread support:

```bash
g++ -pthread -o vowcons vowcons.cpp

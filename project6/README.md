# Page Replacement Simulator in Python

This project simulates **page replacement algorithms** for memory management. It supports the following policies:

- **FIFO** — First-In-First-Out  
- **LRU** — Least Recently Used  
- **OPT** — Optimal (evicts the page used farthest in the future)  
- **RAND** — Random replacement  

The simulator reads a **page reference string**, number of frames, and a list of algorithms to run, then outputs the number of page faults for each algorithm.

---

## Requirements

- Python 3.x

---

## Usage

Run the program and provide input via standard input:

```bash
python3 pagereplacement.py
```

## Example Input

```bash
7 0 1 2 0 3 0 4 2 3 0 3 2
3
FIFO
LRU
OPT
RAND
```

Note - When entering input manually, press Ctrl+D (Linux/macOS) or Ctrl+Z then Enter (Windows) to finish
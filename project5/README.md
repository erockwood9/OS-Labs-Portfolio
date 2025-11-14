# Banker's Algorithm Simulation in Python

This project implements the **Banker's Algorithm** in Python to demonstrate safe state detection and resource request handling in a multi-process system. The program reads input files describing:

- Number of processes and resources  
- Allocation matrix (current allocation of resources)  
- Maximum demand matrix (maximum resources each process may need)  
- Need matrix (computed as max - allocation)  
- Available vector (available units of each resource)  
- A single resource request to attempt

The program then:

- Prints all matrices and vectors  
- Checks whether the system is initially in a safe state  
- Attempts to grant a single request and reports if it can be safely granted

---

## Requirements

- Python 3.x

---

## Usage

Run the program from the command line and provide an input file:

```bash
python3 banker.py <s1.txt>

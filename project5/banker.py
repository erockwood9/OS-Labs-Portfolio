import sys

#***********************************************************************
#
# parse_input
#
# Read and parse the input file describing the number of processes,
# resources, the allocation matrix, the maximum demand matrix, the
# computed need matrix, the available vector, and a single request.
#
#***********************************************************************
def parse_input(file):  
    num_processes = int(file.readline().strip())
    file.readline()
    num_resources = int(file.readline().strip())
    file.readline()

    allocation_matrix = []
    for i in range(num_processes):
        line = list(map(int, file.readline().strip().split()))
        allocation_matrix.append(line)
    file.readline()

    max_matrix = []
    for i in range(num_processes):
        line = list(map(int, file.readline().strip().split()))
        max_matrix.append(line)
    file.readline()

    need_matrix = []
    for i in range(num_processes):
        need_row = []
        for j in range(num_resources):
            # Compute remaining need for each resource type as max - currently allocated.
            need_row.append(max_matrix[i][j] - allocation_matrix[i][j])
        need_matrix.append(need_row)

    available = list(map(int, file.readline().strip().split()))
    file.readline()
    line = file.readline().strip()
    request = (int(line[0]), list(map(int, line[2:].strip().split())))

    return num_processes, num_resources, allocation_matrix, max_matrix, need_matrix, available, request

#***********************************************************************
#
# banker
#
# Determine whether the system is in a safe state using the Banker's
# Algorithm. This routine attempts to find a safe sequence by
# simulating allocation of resources to processes whose needs can be
# satisfied by the current `available` vector.
#
#***********************************************************************
def banker(num_processes, num_resources, allocation_matrix, max_matrix, need_matrix, available):
    finish = [False] * num_processes
    safe_counter = 0

    # Attempt to find a safe sequence. The `finish` array marks processes
    # that can complete with the currently simulated available resources.
    # Each pass tries to find at least one unfinished process whose need
    # is <= available; if found, we "release" its allocation back into
    # available and mark it finished.
    # If an entire pass finds no such process, the system is unsafe.
    while safe_counter < num_processes:
        allocated_in_this_round = False
        for i in range(num_processes):
            if not finish[i]:
                # Check if this process can be satisfied by current available
                if all(need_matrix[i][j] <= available[j] for j in range(num_resources)):
                    # Simulate running the process to completion and
                    # returning its allocated resources to `available`.
                    for j in range(num_resources):
                        available[j] += allocation_matrix[i][j]
                    finish[i] = True
                    safe_counter += 1
                    allocated_in_this_round = True
        # No allocations possible this round => cannot progress to a safe state
        if not allocated_in_this_round:
            break

    # If every process could be finished in the simulation, we are safe.
    return safe_counter == num_processes

#***********************************************************************
#
# main
#
# Entry point for the Banker's Algorithm demonstration. This function
# parses the input file provided as a command-line argument, prints the
# initial matrices/vectors, checks system safety, attempts to satisfy a
# single request, and reports whether the request can be granted.
#
#***********************************************************************
def main():
    path = sys.argv[1]
    file = open(path, 'r')
    num_processes, num_resources, allocation_matrix, max_matrix, need_matrix, available, request = parse_input(file)
    
    print(f"There are {num_processes} processes in the system.\n")
    print(f"There are {num_resources} resources in the system.\n")

    matrix_col_label = '  '.join(chr(65 + i) for i in range(num_resources))
    print("The Allocation Matrix is... ")
    print("   ", matrix_col_label)
    for i in range(num_processes):
        print(f"{i}: {allocation_matrix[i]}")
    print()

    print("The Max Matrix is... ")
    print("   ", matrix_col_label)
    for i in range(num_processes):
        print(f"{i}: {max_matrix[i]}")
    print()

    print("The Need Matrix is... ")
    print("   ", matrix_col_label)
    for i in range(num_processes):
        print(f"{i}: {need_matrix[i]}")
    print()

    print("The Available Vector is...")
    print(" ",matrix_col_label)
    print(available, "\n")

    # Passes a copy of `available` because `banker` mutates the vector during its simulation.
    if banker(num_processes, num_resources, allocation_matrix, max_matrix, need_matrix, available.copy()) == False:
        print("THE SYSTEM IS NOT IN A SAFE STATE, CANNOT ATTEMPT REQUEST!\n")
        return
    print("THE SYSTEM IS IN A SAFE STATE!\n")

    print("The Request Vector is...")
    print("  ", matrix_col_label)
    print(f"{request[0]}:{request[1]}\n")

    # Tentatively grant the request by updating allocation, need, and
    # available. If the resulting state is safe, the request can be
    # granted; otherwise, it must be denied. 
    allocation_matrix[request[0]] = [x + y for x,y in zip(allocation_matrix[request[0]], request[1])]
    need_matrix[request[0]] = [x - y for x,y in zip(max_matrix[request[0]], allocation_matrix[request[0]])]
    available = [a - r for a, r in zip(available, request[1])]

    # Check safety after the hypothetical grant using bankers algorithm
    if banker(num_processes, num_resources, allocation_matrix, max_matrix, need_matrix, available.copy()) == False:
        print("THE REQUEST CANNOT BE GRANTE!D\n")
        return
    print("THE REQUEST CAN BE GRANTED! \n")

    print("The Available Vector is...")
    print(" ",matrix_col_label)
    print(available, "\n")


if __name__ == "__main__":
    main()
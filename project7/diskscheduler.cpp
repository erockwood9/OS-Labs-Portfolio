#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <string>

using namespace std;

//***********************************************************************
//
// fcfs
//
// First-Come-First-Serve disk scheduling. Services requests in
// the order they appear in the input list. Computes and returns the
// total head movement 
//
//***********************************************************************
int fcfs(vector<int>& cylinders, int head_pos)
{
    int total_movement = 0;
    for (int cylinder : cylinders) {
        total_movement += abs(cylinder - head_pos);
        head_pos = cylinder;
    }
    return total_movement;
}

//***********************************************************************
//
// sstf
//
// Shortest-Seek-Time-First scheduling. At each step choose the
// unvisited request closest to the current head position. This is a
// greedy algorithm that minimizes immediate seek distance.
//
//***********************************************************************
int sstf(vector<int> cylinders, int head_pos, int disk_size = 3000)
{
    int total_movement = 0;
    vector<bool> visited(cylinders.size(), false);

    for (size_t i = 0; i < cylinders.size(); ++i) {
        int closest_index = -1;
        int min_distance = disk_size;

        // Find closest unvisited request
        for (size_t j = 0; j < cylinders.size(); ++j) {
            if (!visited[j]) {
                int distance = abs(cylinders[j] - head_pos);
                if (distance < min_distance) {
                    min_distance = distance;
                    closest_index = j;
                }
            }
        }

        // Mark it served and accumulate movement
        visited[closest_index] = true;
        total_movement += abs(cylinders[closest_index] - head_pos);
        head_pos = cylinders[closest_index];
    }

    return total_movement;
}

//***********************************************************************
//
// scan
//
// SCAN algorithm: the head moves in one direction to the
// end of the disk servicing requests along the way, then reverses and
// services requests in the opposite direction. This implementation
// assumes the head initially moves to the right (increasing
// cylinder numbers).
//
//***********************************************************************
int scan(vector<int> cylinders, int head_pos, int disk_size = 3000)
{
    int total_movement = 0;
    vector<int> left, right;

    // Add the physical edges so the head goes to the end before
    // reversing (this models the head touching the disk boundary).
    right.push_back(disk_size - 1);
    left.push_back(0);

    // Partition requests relative to head position
    for (int c : cylinders) {
        if (c < head_pos)
            left.push_back(c);
        else
            right.push_back(c);
    }

    // Sort so we service in order along each direction
    sort(left.begin(), left.end(), greater<>());
    sort(right.begin(), right.end());

    // Move right first (increasing cylinder numbers)
    for (int c : right) {
        total_movement += abs(c - head_pos);
        head_pos = c;
    }
    // Then reverse direction and service left side
    for (int c : left) {
        total_movement += abs(c - head_pos);
        head_pos = c;
    }
    return total_movement;
}

//***********************************************************************
//
// cscan
//
// CSCAN algorithm: the head moves in one direction to
// the end of the disk servicing requests, then jumps back to the
// beginning without servicing requests on the return trip . This implementation
// models the jump by adding the distance from the current head to the
// beginning.
//
//***********************************************************************
int cscan(vector<int> cylinders, int head_pos, int disk_size = 3000)
{
    int total_movement = 0;
    vector<int> left, right;

    // Add the physical edges for correctness when moving to the end
    right.push_back(disk_size - 1);
    left.push_back(0);

    for (int c : cylinders) {
        if (c < head_pos)
            left.push_back(c);
        else
            right.push_back(c);
    }

    sort(left.begin(), left.end());
    sort(right.begin(), right.end());

    // Service requests to the right first (toward disk_size-1)
    for (int c : right) {
        total_movement += abs(c - head_pos);
        head_pos = c;
    }

    // Jump to beginning. The cost of the jump is from the
    // current head position back to cylinder 0.
    total_movement += head_pos;
    head_pos = 0;

    // Then service requests that were on the left side in order
    for (int c : left) {
        total_movement += abs(c - head_pos);
        head_pos = c;
    }
    return total_movement;
}

//***********************************************************************
//
// look
//
// LOOK algorithm: similar to SCAN but the head only goes as far as the
// furthest request in each direction.
//
//***********************************************************************
int look(vector<int> cylinders, int head_pos)
{
    int total_movement = 0;
    vector<int> left, right;

    // Partition requests relative to head position
    for (int c : cylinders) {
        if (c < head_pos)
            left.push_back(c);
        else
            right.push_back(c);
    }

    // Service to the right first, then to the left; do not include
    // disk edges because LOOK stops at the last request.
    sort(left.begin(), left.end(), greater<>());
    sort(right.begin(), right.end());

    for (int c : right) {
        total_movement += abs(c - head_pos);
        head_pos = c;
    }

    for (int c : left) {
        total_movement += abs(c - head_pos);
        head_pos = c;
    }

    return total_movement;
}

//***********************************************************************
//
// clook
//
// C-LOOK algorithm: The head moves in one direction
// servicing requests up to the furthest request, then jumps to the
// smallest request without traversing the unused track area.
//
//***********************************************************************
int clook(vector<int> cylinders, int head_pos)
{
    int total_movement = 0;
    vector<int> left, right;

    for (int c : cylinders) {
        if (c < head_pos)
            left.push_back(c);
        else
            right.push_back(c);
    }

    sort(left.begin(), left.end());
    sort(right.begin(), right.end());

    // Service to the right first
    for (int c : right) {
        total_movement += abs(c - head_pos);
        head_pos = c;
    }

    if (!left.empty()) {
        // Jump from the last serviced right request to the first left
        // request; this models the circular jump but does not include
        // outer disk edges.
        total_movement += abs(right.back() - left.front());
        head_pos = left.front();

        for (int c : left) {
            total_movement += abs(c - head_pos);
            head_pos = c;
        }
    }

    return total_movement;
}

//***********************************************************************
//
// main
//
// Entry point for the disk scheduler simulator.
//
// This program expects a single command-line argument:
//   argv[1] - starting head position (integer cylinder number)
//
//  - Generates a random list of cylinder requests (default 3000
//    requests in the range [0, disk_size-1]).
//  - Runs each disk scheduling algorithm (FCFS, SSTF, SCAN, CSCAN,
//    LOOK, CLOOK) on the same generated request list, starting from
//    the specified starting head position.
//  - Prints the total head movement for
//    each algorithm. 
//
//***********************************************************************
int main(int argc, char* argv[])
{
    if (argc < 2) {
        cerr << "Must provide starting head position as argument.\n";
        return 1;
    }

    int starting_head = stoi(argv[1]);
    const int size = 3000;
    vector<int> cylinders(size);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, size - 1);

    for (int& x : cylinders)
        x = dist(gen);

    cout << "Starting head position: " << starting_head << "\n\n";

    cout << "FCFS total movement:  " << fcfs(cylinders, starting_head) << "\n";
    cout << "SSTF total movement:  " << sstf(cylinders, starting_head) << "\n";
    cout << "SCAN total movement:  " << scan(cylinders, starting_head) << "\n";
    cout << "CSCAN total movement: " << cscan(cylinders, starting_head) << "\n";
    cout << "LOOK total movement:  " << look(cylinders, starting_head) << "\n";
    cout << "CLOOK total movement: " << clook(cylinders, starting_head) << "\n";

    return 0;
}

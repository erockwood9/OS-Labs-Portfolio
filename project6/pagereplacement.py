import sys
import random

#***********************************************************************
#
# fifo
#
# First-in-first-out page replacement implementation. Iterates through
# the page reference list `pages` and keeps a FIFO queue in `frames`.
# On a page fault, the oldest frame is evicted when the frame list is
# full; otherwise the new page is appended. Returns the number of
# page faults encountered.
#
#***********************************************************************
def fifo(pages, num_frames):
    frames = []
    faults = 0
    for page in pages:
        if page not in frames:
            faults += 1
            if len(frames) < num_frames:
                frames.append(page)
            else:
                frames.pop(0)
                frames.append(page)
    return faults

#***********************************************************************
#
# lru
#
# Least-Recently-Used page replacement implementation. Tracks the
# last-used index for each page in `last_used`. On a page fault, evicts
# the page with the smallest last-used index (least recently used) when
# frames are full. Returns the number of page faults.
#
#***********************************************************************
def lru(pages, num_frames):
    frames = []
    last_used = {}
    faults = 0
    for i, page in enumerate(pages):
        if page not in frames:
            faults += 1
            if len(frames) < num_frames:
                frames.append(page)
            else:
                # Evict least recently used
                lru_page = min(frames, key=lambda p: last_used.get(p, -1))
                frames.remove(lru_page)
                frames.append(page)
        last_used[page] = i
    return faults

#***********************************************************************
#
# opt
#
# Optimal page replacement implementation. For each page
# fault when frames are full, chooses to evict the page whose next use
# in the future is farthest away (or never). This is implemented by
# scanning the remainder of the reference string to find next-use
# positions. Returns the number of page faults.
#
#***********************************************************************
def opt(pages, num_frames):
    frames = []
    faults = 0
    for i, page in enumerate(pages):
        if page not in frames:
            faults += 1
            if len(frames) < num_frames:
                frames.append(page)
            else:
                # Choose page not used for longest time in future
                future_use = {}
                for f in frames:
                    try:
                        future_use[f] = pages[i+1:].index(f)
                    except ValueError:
                        future_use[f] = float('inf')
                victim = max(future_use, key=future_use.get)
                frames.remove(victim)
                frames.append(page)
    return faults

#***********************************************************************
#
# rand
#
# Random replacement policy. On a page fault with full frames, a random
# page is chosen for eviction. Returns the number of page
# faults. Uses the `random` module.
#
#***********************************************************************
def rand(pages, num_frames):
    frames = []
    faults = 0
    for page in pages:
        if page not in frames:
            faults += 1
            if len(frames) < num_frames:
                frames.append(page)
            else:
                victim = random.choice(frames)
                frames.remove(victim)
                frames.append(page)
    return faults

#***********************************************************************
#
# main
#
# Entry point for the page replacement simulator. Reads input from
# standard input, parses the page reference string, number of frames,
# and requested algorithms, prints a brief header, and runs the
# selected page replacement algorithms (FIFO, LRU, OPT, RAND).
#
#***********************************************************************
def main():
    # Read all input from stdin
    print("When done entering input, press Ctrl+D to finish.\n")
    lines = [line.strip() for line in sys.stdin if line.strip()]

    # Parse reference string
    pages = list(map(int, lines[0].split()))
    num_frames = int(lines[1])
    algorithms = [line.upper() for line in lines[2:]]

    # Output header
    print("Page Reference String:")
    for i in range(0, len(pages), 20):
        print(" ".join(map(str, pages[i:i+20])))
    print(f"Number of Frames: {num_frames}")

    # Run requested algorithms
    for alg in algorithms:
        if alg == "FIFO":
            faults = fifo(pages, num_frames)
        elif alg == "LRU":
            faults = lru(pages, num_frames)
        elif alg == "OPT":
            faults = opt(pages, num_frames)
        elif alg == "RAND":
            faults = rand(pages, num_frames)
        else:
            continue  # Skip unknown algorithms
        print(f"{alg}: {faults}")

if __name__ == "__main__":
    main()

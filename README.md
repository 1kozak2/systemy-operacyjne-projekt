# Dining Philosophers Problem - C++ Implementation (Using Mutexes and a Semaphore)

## Theoretical Problem Description

The Dining Philosophers problem is a classical synchronization problem illustrating challenges in concurrent programming, specifically:
- **N philosophers** sit around a circular table.
- Each philosopher alternates between **thinking** and **eating**.
- There are **N forks** total, one between each pair of philosophers.
- To eat, a philosopher must pick up **both** the left fork and the right fork.
- No two philosophers can hold the same fork at the same time.

Key challenges:
- **Deadlock**: where each philosopher has one fork and is waiting indefinitely for the other.
- **Starvation**: a philosopher repeatedly misses opportunities to eat because neighbors hog resources.

This simulation demonstrates safe resource allocation and concurrency control.

---

## What Is Multithreading?

**Multithreading** is a programming method where multiple sequences of execution (threads) run concurrently within the same program. It allows tasks to proceed in parallel on multicore machines, boosting performance and responsiveness.

In this simulation:
- Each philosopher is a **separate thread**.
- The threads share fork objects, requiring coordination to avoid race conditions.
- Proper synchronization ensures that no two philosophers use the same fork simultaneously and avoids deadlock.

Threads are managed via `std::thread` from the C++ standard library.

---

## Implementation Overview

This project implements the Dining Philosophers problem using **C++17**, leveraging:
- **`std::thread`** for concurrency (one thread per philosopher).
- **`std::mutex`** for fork synchronization (one mutex per fork).
- **`std::counting_semaphore`** (from C++20) to further prevent deadlock by allowing only `N-1` philosophers to attempt picking up forks simultaneously.

Each philosopher thread **thinks** and **eats** multiple times, with random delays to simulate real processing. We use color-coded console output to visualize each philosopher’s actions.

---

## How to Compile and Run

### Requirements
- A C++20-compatible compiler (for `<semaphore>`).  
- Alternatively, a C++17 compiler with a backport/implementation of `<semaphore>` or a custom semaphore library.

### Compilation
```bash
# Linux / macOS
g++ -std=c++20 -pthread -o filozofowie filozofowie.cpp

# Windows (MinGW)
g++ -std=c++20 -o filozofowie.exe filozofowie.cpp
```
*(Replace `-std=c++20` with the relevant C++ standard if you have a custom semaphore backport.)*

### Execution
```bash
./filozofowie <number_of_filozofowie>

# Example:
./filozofowie 5
```
- Minimum: 2 philosophers required.
- Each philosopher alternates between thinking and eating 3 times.

---

## Threads and Their Roles

- **Philosopher threads**: Each philosopher runs in a separate thread.  
- The main thread spawns all philosopher threads, then waits for them to finish (`join`).

Each philosopher:
1. **Thinks** (delays to simulate computation).
2. Declares intention to eat.
3. **Acquires semaphore** to ensure no more than N-1 philosophers simultaneously try to pick forks.
4. **Locks (acquires) the left and right fork** (using `std::mutex` on each).
5. **Eats** (further delay).
6. **Unlocks (releases) the forks**.
7. **Releases the semaphore** so another philosopher can attempt to eat.
8. Repeats several times before finishing.

---

## Critical Sections and Synchronization

1. **Fork Synchronization — `std::mutex`**  
   - We have an array of `std::mutex`, one per fork.  
   - A philosopher must **lock** both forks before eating.  
   - **Deadlock prevention**:  
     - A global **counting semaphore** is used to ensure not all N philosophers try to lock forks simultaneously.  
     - Additionally, the last philosopher in the sequence sometimes picks forks in reversed order to avoid circular lock dependencies.

2. **Semaphore for Deadlock Avoidance**  
   - A **`std::counting_semaphore`** is initialized to `NUM_PHILOSOPHERS - 1`.  
   - Each philosopher **acquires** one permit before locking any forks, ensuring at least one philosopher is always blocked if all forks are busy.  
   - Once finished eating, the philosopher **releases** a permit, allowing another philosopher to proceed.

3. **Output Synchronization**  
   - A separate `std::mutex` (`print_mutex`) controls access to `std::cout`.  
   - This prevents interleaved or garbled console output from concurrent threads.

---

## Summary

This C++ implementation showcases:
- **Multithreading** via `std::thread` (one thread per philosopher).
- **Mutex-based** resource protection for forks, preventing race conditions.
- **Counting semaphore** to prevent deadlock by limiting simultaneous fork requests to `N-1`.
- **Console logging** with color codes for clarity, guarded by a separate mutex.

It demonstrates a practical solution to the Dining Philosophers problem using **standard C++ concurrency tools** (`std::mutex` and `std::counting_semaphore`) while avoiding deadlock and starvation.
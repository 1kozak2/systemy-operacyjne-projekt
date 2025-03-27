# Dining Philosophers Problem - C++ Implementation

## Theoretical Problem Description

The Dining Philosophers problem is a classical synchronization problem in computer science, often used to illustrate the challenges of avoiding deadlock and resource starvation in concurrent programming.

The scenario is described as follows:
- **N philosophers** sit around a circular table.
- Each philosopher alternates between two actions: **thinking** and **eating**.
- A **fork** is placed between each pair of philosophers, for a total of **N forks**.
- To eat, a philosopher must pick up **both the left and right fork**.
- No two philosophers may hold the same fork at the same time.

Key challenges:
- **Deadlock**: if all philosophers pick up their left fork at the same time and wait for the right one, they could be stuck indefinitely.
- **Starvation**: a philosopher may be unable to eat if neighbors monopolize the forks.

This problem demonstrates issues with resource allocation and concurrency in operating systems and parallel programming.

---

## What Is Multithreading?

**Multithreading** is a programming paradigm where multiple threads (independent sequences of execution) run concurrently within a single program. It enables tasks to be performed in parallel, especially on multicore processors, enhancing responsiveness and performance.

In this simulation:
- Each philosopher is an independent **thread**.
- Threads share data (forks) and must coordinate access to avoid inconsistencies.
- Proper synchronization is essential to prevent race conditions and deadlocks.

Threads are created using `std::thread` from the C++ standard library.

---

## Implementation Overview

This project implements the Dining Philosophers problem using **C++17**, `std::thread` for multithreading, and a manually coded **SpinLock** for fork synchronization.

A dynamic array of `SpinLock` objects (`std::unique_ptr<SpinLock[]>`) represents forks shared between philosophers. Each philosopher runs in a separate thread, alternately thinking and eating, with delays to simulate real processing.

Color-coded output visually represents each philosopher's state.

---

## How to Compile and Run

### Requirements
- A C++17-compatible compiler (e.g., g++, clang++)

### Compilation
```bash
# Linux / macOS
g++ -std=c++17 -pthread -o filozofowie filozofowie.cpp

# Windows (MinGW)
g++ -std=c++17 -o filozofowie.exe filozofowie.cpp
```

### Execution
```bash
./filozofowie <number_of_filozofowie>

# Example:
./filozofowie 5
```
- Minimum: 2 philosophers required
- Each philosopher thinks and eats 3 times

---

## Threads and Their Roles

Each **philosopher is represented by a thread**:
- Created using:
```cpp
std::vector<std::thread> philosophers;
```
- Each thread performs:
  - Think (delayed simulation)
  - Announce hunger
  - Acquire left and right forks (SpinLocks)
  - Eat (delayed simulation)
  - Release forks
  - Repeat 3 times

The main thread launches philosopher threads and waits for all to finish using `.join()`.

---

## Critical Sections and Synchronization

### Fork Synchronization — Custom SpinLock

Forks are shared resources. To ensure exclusive access, each fork is protected by a **SpinLock**:
```cpp
class SpinLock {
    std::atomic_flag flag;
public:
    SpinLock();
    void lock();
    void unlock();
};
```
- Uses `std::atomic_flag` for lock-free busy waiting
- Prevents race conditions without using `std::mutex`

Philosophers acquire both forks before eating:
```cpp
forks[left].lock();
forks[right].lock();
```
To avoid deadlock, the **last philosopher** picks up forks in reverse order:
```cpp
if (id == NUM_PHILOSOPHERS - 1) std::swap(left, right);
```

### Output Synchronization — `std::mutex`

To ensure output is readable and not interleaved, a `std::mutex` is used **only** for synchronizing `std::cout`:
```cpp
std::mutex print_mutex;
void safe_print(const std::string& msg);
```
This does not interfere with fork logic and meets project constraints.

---

## Summary

This C++ implementation demonstrates multithreading and manual synchronization of shared resources using custom spinlocks. It avoids standard synchronization primitives for fork management and uses clear, color-coded console output for tracing execution.

✅ Key Features:
- Realistic simulation of the Dining Philosophers problem
- Deadlock prevention via asymmetric fork acquisition
- Dynamic thread creation
- Custom spinlocks (no `std::mutex` for forks)
- Safe, color-coded logging using `std::mutex` for console

This project fulfills all technical and theoretical requirements and is suitable for academic submission.

---


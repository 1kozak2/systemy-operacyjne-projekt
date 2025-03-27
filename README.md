# Dining Philosophers Problem - C++ Implementation

## Theoretical Problem Description

The Dining Philosophers problem is a classical synchronization problem in computer science, often used to illustrate the challenges of avoiding deadlock and resource starvation in concurrent programming.

The problem is formulated as follows:
- There are **N philosophers** sitting around a circular table.
- Each philosopher alternates between **thinking** and **eating**.
- Between each pair of philosophers lies a **single fork**, for a total of **N forks**.
- A philosopher must pick up **both the left and the right fork** to eat.
- Philosophers cannot share forks simultaneously.

Key challenges include:
- **Deadlock**: if each philosopher picks up their left fork and waits for the right one, they can all be stuck forever.
- **Starvation**: a philosopher may wait indefinitely if others never release forks.

This problem is a metaphor for managing shared resources (forks) among competing processes (philosophers).

---

## Implementation Overview

This project implements the Dining Philosophers problem in **C++17** using `std::thread` for multithreading and a custom `SpinLock` class to manage fork access. The use of standard synchronization primitives like `std::mutex` for critical sections (excluding console output) has been explicitly avoided, as required.

Color-coded output and artificial delays simulate the philosophers' activities and help visualize concurrent behavior.

---

## How to Compile and Run

### Requirements
- A C++17-compatible compiler (e.g., g++, clang++)

### Compilation
```bash
# Linux / macOS
g++ -std=c++17 -pthread -o philosophers philosophers.cpp

# Windows (MinGW)
g++ -std=c++17 -o philosophers.exe philosophers.cpp -lws2_32
```

### Execution
```bash
./philosophers <number_of_philosophers>

# Example
./philosophers 5
```
- Minimum: 2 philosophers
- The simulation runs each philosopher 3 times through thinking/eating

---

## Threads and Their Roles

Each **philosopher is represented by a thread**:
- The main thread creates `N` philosopher threads.
- Each thread follows the cycle:
  - Think (simulated delay)
  - Wait for forks
  - Pick up forks
  - Eat (simulated delay)
  - Put down forks

Each fork is a shared resource between two threads.

```cpp
std::vector<std::thread> philosophers;
```

---

## Critical Sections and Synchronization

### Fork Access – Custom SpinLock

Forks are shared resources and must be protected from concurrent access. Instead of using `std::mutex`, we implemented our own **spinlock**:

```cpp
class SpinLock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock();
    void unlock();
};
```
- Spinlocks use busy-waiting and are suitable for short critical sections.
- Each fork is represented as a `SpinLock` instance.
- A philosopher locks both adjacent forks before eating.
- To avoid deadlock, the **last philosopher** picks up the right fork before the left, breaking the cyclic wait.

```cpp
if (id == NUM_PHILOSOPHERS - 1) std::swap(left, right);
```

### Console Output – Standard Mutex

To prevent garbled multithreaded output, a `std::mutex` is used **only** for printing to the console:

```cpp
std::mutex print_mutex;
void safe_print(const std::string& msg);
```

This does not impact the problem logic and is allowed as per project assumptions.

---

## Summary

This implementation:
- Models concurrent behavior correctly using threads
- Avoids deadlock via controlled resource acquisition order
- Implements custom locking mechanisms without relying on built-in synchronization for forks
- Provides clear output using color-coded logs for each philosopher’s state

It demonstrates an understanding of concurrent programming principles, resource contention, and thread synchronization using custom-built tools.

---

© 2025 | Systems Programming Project


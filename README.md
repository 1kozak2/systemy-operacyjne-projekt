# Dining Philosophers (C++)

## 🧠 Project Description

This project is a multithreaded simulation of the classical **Dining Philosophers** problem, implemented in C++ using `std::thread` and a custom synchronization primitive (`SpinLock`).

The goal was to solve the problem without relying on any built-in mutex or semaphore mechanisms. Instead, we implemented thread synchronization manually and safely controlled access to shared resources (forks).

---

## 🧪 How to Run

```bash
g++ -std=c++17 -pthread -o filozofowie filozofowie.cpp
./filozofowie 5

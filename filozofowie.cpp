#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <atomic>
#include <mutex>
#include <sstream>

// ANSI colors
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// Custom spinlock implementation
class SpinLock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            // busy-wait
        }
    }
    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

std::vector<SpinLock> forks;
int NUM_PHILOSOPHERS;
std::mutex print_mutex;

void safe_print(const std::string& msg) {
    std::lock_guard<std::mutex> guard(print_mutex);
    std::cout << msg;
}

void philosopher(int id) {
    int left = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;

    for (int i = 0; i < 3; ++i) {
        {
            std::ostringstream msg;
            msg << CYAN << "Philosopher " << id << ": thinking..." << RESET << "\n";
            safe_print(msg.str());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 + rand() % 1000));

        {
            std::ostringstream msg;
            msg << BLUE << "Philosopher " << id << ": wants to eat." << RESET << "\n";
            safe_print(msg.str());
        }

        if (id == NUM_PHILOSOPHERS - 1) std::swap(left, right);

        forks[left].lock();
        {
            std::ostringstream msg;
            msg << YELLOW << "Philosopher " << id << ": picked up left fork " << left << RESET << "\n";
            safe_print(msg.str());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        forks[right].lock();
        {
            std::ostringstream msg;
            msg << YELLOW << "Philosopher " << id << ": picked up right fork " << right << RESET << "\n";
            safe_print(msg.str());
        }

        {
            std::ostringstream msg;
            msg << GREEN << "Philosopher " << id << ": eating." << RESET << "\n";
            safe_print(msg.str());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1500 + rand() % 1000));

        forks[right].unlock();
        forks[left].unlock();
        {
            std::ostringstream msg;
            msg << MAGENTA << "Philosopher " << id << ": put down forks." << RESET << "\n";
            safe_print(msg.str());
        }
    }
    {
        std::ostringstream msg;
        msg << RED << "Philosopher " << id << ": finished." << RESET << "\n";
        safe_print(msg.str());
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_philosophers>\n";
        return 1;
    }

    NUM_PHILOSOPHERS = std::atoi(argv[1]);
    if (NUM_PHILOSOPHERS < 2) {
        std::cerr << "There must be at least 2 philosophers.\n";
        return 1;
    }

    srand(static_cast<unsigned>(time(nullptr)));

    forks.resize(NUM_PHILOSOPHERS);

    std::vector<std::thread> philosophers;
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers.emplace_back(philosopher, i);
    }

    for (auto& p : philosophers) {
        p.join();
    }

    {
        std::ostringstream msg;
        msg << GREEN << "All philosophers have finished eating." << RESET << "\n";
        safe_print(msg.str());
    }
    return 0;
}

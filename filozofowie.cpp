#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <atomic>
#include <mutex>
#include <sstream>
#include <ctime>         // for time()
#include <memory>        // for std::unique_ptr
#include <vector>        // <-- IMPORTANT: required for std::vector

// ANSI colors
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// A custom spinlock using std::atomic_flag
class SpinLock {
    std::atomic_flag flag;

public:
    // Default constructor initializes the flag
    SpinLock() : flag(ATOMIC_FLAG_INIT) {}

    // Delete copy and move constructors (cannot copy or move atomic_flag)
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
    SpinLock(SpinLock&&) = delete;
    SpinLock& operator=(SpinLock&&) = delete;

    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            // busy-wait
        }
    }
    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

static std::unique_ptr<SpinLock[]> forks; // dynamically allocated array of SpinLock
static int NUM_PHILOSOPHERS;             // number of philosophers
static std::mutex print_mutex;           // for thread-safe printing

// Thread-safe print function
void safe_print(const std::string& msg) {
    std::lock_guard<std::mutex> guard(print_mutex);
    std::cout << msg;
}

// The philosopher's routine
void philosopher(int id) {
    int left = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;

    for (int i = 0; i < 3; ++i) {
        // Thinking
        {
            std::ostringstream msg;
            msg << CYAN << "Philosopher " << id << ": thinking..." << RESET << "\n";
            safe_print(msg.str());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 + rand() % 1000));

        // Wants to eat
        {
            std::ostringstream msg;
            msg << BLUE << "Philosopher " << id << ": wants to eat." << RESET << "\n";
            safe_print(msg.str());
        }

        // To avoid deadlock, last philosopher picks forks in reversed order
        if (id == NUM_PHILOSOPHERS - 1) {
            std::swap(left, right);
        }

        // Pick up left fork
        forks[left].lock();
        {
            std::ostringstream msg;
            msg << YELLOW << "Philosopher " << id << ": picked up left fork "
                << left << RESET << "\n";
            safe_print(msg.str());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Pick up right fork
        forks[right].lock();
        {
            std::ostringstream msg;
            msg << YELLOW << "Philosopher " << id << ": picked up right fork "
                << right << RESET << "\n";
            safe_print(msg.str());
        }

        // Eating
        {
            std::ostringstream msg;
            msg << GREEN << "Philosopher " << id << ": eating." << RESET << "\n";
            safe_print(msg.str());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1500 + rand() % 1000));

        // Put down forks
        forks[right].unlock();
        forks[left].unlock();
        {
            std::ostringstream msg;
            msg << MAGENTA << "Philosopher " << id << ": put down forks." << RESET << "\n";
            safe_print(msg.str());
        }
    }

    // Finished
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

    // Seed random
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Allocate our spinlock array on the heap
    forks = std::make_unique<SpinLock[]>(NUM_PHILOSOPHERS);

    // Launch philosopher threads
    std::vector<std::thread> philosophers;
    philosophers.reserve(NUM_PHILOSOPHERS);

    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers.emplace_back(philosopher, i);
    }

    // Join all threads
    for (auto& p : philosophers) {
        p.join();
    }

    // Final message
    {
        std::ostringstream msg;
        msg << GREEN << "All philosophers have finished eating." << RESET << "\n";
        safe_print(msg.str());
    }

    return 0;
}

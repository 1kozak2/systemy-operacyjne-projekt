#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <atomic>
#include <mutex>
#include <sstream>
#include <ctime>       
#include <memory>        // for std::unique_ptr
#include <vector>     
#include <semaphore>     // C++20 counting_semaphore
#include <climits>       




// ANSI colors
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// Array of fork mutexes (dynamically allocated)
static std::counting_semaphore<INT_MAX> table_sem(0);
static std::unique_ptr<std::mutex[]> forks;

// A counting semaphore to limit concurrency to N-1 philosophers

// Number of philosophers
static int NUM_PHILOSOPHERS = 0;

// Mutex for thread-safe printing
static std::mutex print_mutex;
// Thread-safe print function
void safe_print(const std::string& msg) {
    std::lock_guard<std::mutex> guard(print_mutex);
    std::cout << msg;
}

// Philosopher routine
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

        // Acquire the semaphore so we never have all N philosophers picking forks
        table_sem.acquire();

        // To avoid deadlock, last philosopher picks forks in reversed order
        if (id == NUM_PHILOSOPHERS - 1) {
            std::swap(left, right);
        }

        // Lock the left fork
        forks[left].lock();
        {
            std::ostringstream msg;
            msg << YELLOW << "Philosopher " << id << ": picked up fork " 
                << left << RESET << "\n";
            safe_print(msg.str());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Lock the right fork
        forks[right].lock();
        {
            std::ostringstream msg;
            msg << YELLOW << "Philosopher " << id << ": picked up fork " 
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
            msg << MAGENTA << "Philosopher " << id << ": ate, put down forks." << RESET << "\n";
            safe_print(msg.str());
        }
        table_sem.release();
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
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Allocate the array of fork mutexes
    forks = std::make_unique<std::mutex[]>(NUM_PHILOSOPHERS);

    // Initialize the semaphore count to NUM_PHILOSOPHERS - 1
    // so that only up to N-1 philosophers can try to eat at once
    table_sem.release(NUM_PHILOSOPHERS - 1);

    // Create and start philosopher threads
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

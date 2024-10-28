#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <vector>

// Stack implemented using std::list
std::list<int> stack;
std::mutex stackMutex;

// Output file stream
std::ofstream outputFile("output.txt");

// Push an element onto the stack
void push(int value) {
    std::lock_guard<std::mutex> lock(stackMutex);

    stack.push_front(value);
    outputFile << "Pushed: " << value << ". Stack: ";
    for (int val : stack) {
        outputFile << val << " ";
    }
    outputFile << std::endl;
}

// Pop an element from the stack
int pop() {
    std::lock_guard<std::mutex> lock(stackMutex);

    if (stack.empty()) {
        outputFile << "Stack underflow" << std::endl;
        return -1; // Return a sentinel value for empty stack
    }

    int poppedValue = stack.front();
    stack.pop_front();

    outputFile << "Popped: " << poppedValue << ". Stack: ";
    for (int val : stack) {
        outputFile << val << " ";
    }
    outputFile << std::endl;

    return poppedValue;
}

// Function executed by each thread
void testStack(int threadId) {
    std::srand(std::time(nullptr) + threadId); // Seed random number generator with a unique value

    for (int i = 0; i < 1; ++i) { // EDIT HOW MANY TIMES TO PUSH/POP HERE
        for (int j = 0; j < 3; ++j) {
            int value = std::rand() % 100; // Generate a random value to push
            push(value);
        }

        for (int j = 0; j < 3; ++j) {
            pop();
        }
    }
}

int main() {
    const int NUM_THREADS = 200; // NUMBER OF THREADS SET TO 200
    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS);

    // Create threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(testStack, i);
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    // Display final stack elements
    std::lock_guard<std::mutex> lock(stackMutex);
    outputFile << "Final stack elements: ";
    for (int val : stack) {
        outputFile << val << " ";
    }
    outputFile << std::endl;

    // Close the output file
    outputFile.close();

    return 0;
}

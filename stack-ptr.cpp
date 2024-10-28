#include <iostream>
#include <list>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <fstream>

// Stack implemented with list library 
std::list<int> stack;
pthread_mutex_t stackMutex = PTHREAD_MUTEX_INITIALIZER;

// Output file stream
std::ofstream outputFile("output.txt");

void push(int value) {
    pthread_mutex_lock(&stackMutex);

    stack.push_front(value);
    outputFile << "Pushed: " << value << ". Stack: ";
    for (int val : stack) {
        outputFile << val << " ";
    }
    outputFile << std::endl;

    pthread_mutex_unlock(&stackMutex);
}

int pop() {
    pthread_mutex_lock(&stackMutex);

    if (stack.empty()) {
        outputFile << "Stack underflow" << std::endl;
        pthread_mutex_unlock(&stackMutex);
        return -1; // Return a sentinel value for empty stack
    }

    int poppedValue = stack.front();
    stack.pop_front();

    outputFile << "Popped: " << poppedValue << ". Stack: ";
    for (int val : stack) {
        outputFile << val << " ";
    }
    outputFile << std::endl;

    pthread_mutex_unlock(&stackMutex);
    return poppedValue;
}

void* testStack(void* arg) {
    int threadId = *((int*)arg);
    std::srand(std::time(nullptr) + threadId); // Seed random number generator with a unique value

    for (int i = 0; i < 3; ++i) {                                                             // EDIT HOW MANY TIMES TO PUSH/POP HERE, IN THIS CASE WE ARE ASKED FOR 3 
        // Push 3 values
        for (int j = 0; j < 3; ++j) {
            int value = std::rand() % 100; // Generate a random value to push
            push(value);
        }
        
        // Pop 3 values
        for (int j = 0; j < 3; ++j) {
            pop();
        }
    }
    return nullptr;
}

int main() {
    const int NUM_THREADS = 200;                                                               // NUMBER OF THREADS SET TO 200
    pthread_t threads[NUM_THREADS];
    int threadIds[NUM_THREADS];

    // Create threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        threadIds[i] = i;
        if (pthread_create(&threads[i], nullptr, testStack, &threadIds[i])) {
            std::cerr << "Error creating thread " << i << std::endl;
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Display final stack elements
    pthread_mutex_lock(&stackMutex);
    outputFile << "Final stack elements: ";
    for (int val : stack) {
        outputFile << val << " ";
    }
    outputFile << std::endl;
    pthread_mutex_unlock(&stackMutex);

    // Close the output file
    outputFile.close();  

    // Destroy the mutex
    pthread_mutex_destroy(&stackMutex);
    return 0;
}

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <cstdlib>

/*

Code was brought from C to C++ 

GROUP MEMBERS:
Fnu Mahsa
Amanda Chen
Aidan Ross
Melissa Liu
Kristian Losenara

*/

// Linked list node 
typedef int value_t;
struct StackNode {
    value_t data;
    StackNode* next;
};

// Mutex for synchronizing access to the stack
pthread_mutex_t stack_mutexlock = PTHREAD_MUTEX_INITIALIZER;

// Stack function declarations
void push(value_t v, StackNode*& top);
value_t pop(StackNode*& top);
bool is_empty(StackNode* top);
void* testStack(void* arg);

// Output file stream
std::ofstream outputFile("output.txt");

int main() {
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open output file" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Initialize stack
    StackNode* top = nullptr;

    pthread_t threads[200];          //Threads set to 200 here

    // Create 200 concurrent threads using our testStack function
    for (int i = 0; i < 200; i++) {
        if (pthread_create(&threads[i], nullptr, testStack, &top) != 0) {
            std::cerr << "Failed to create thread" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < 200; i++) {
        pthread_join(threads[i], nullptr);
    }

    outputFile.close();

    // Destroy the mutex
    pthread_mutex_destroy(&stack_mutexlock);

    return 0;
}

// Thread function for intermixing push and pop operations
void* testStack(void* arg) {
    StackNode** top = (StackNode**)arg;

    for (int i = 0; i < 500; i++) {
        pthread_mutex_lock(&stack_mutexlock);

        // Intermix 3 push and 3 pop operations
        value_t randValue1 = rand() % 100 + 1;         //Rand() gives us a number, mod 100 keeps it between [0,99], and + 1 gives us [0, 100]
        value_t randValue2 = rand() % 100 + 1;
        value_t randValue3 = rand() % 100 + 1;

        push(randValue1, *top);
        push(randValue2, *top);
        push(randValue3, *top);

        outputFile << "Thread " << pthread_self() << " pushed: " 
                   << randValue1 << ", " << randValue2 << ", " << randValue3 << std::endl;

        value_t pop1 = pop(*top);
        value_t pop2 = pop(*top);
        value_t pop3 = pop(*top);

        outputFile << "Thread " << pthread_self() << " popped: " 
                   << pop1 << ", " << pop2 << ", " << pop3 << std::endl;

        pthread_mutex_unlock(&stack_mutexlock);
    }

    return nullptr;  //Since arg is void*
}

// Stack function definitions (written from C to C++)
void push(value_t v, StackNode*& top) {
    StackNode* new_node = new StackNode();
    //Added a case for failure
    if (new_node == nullptr) {
        std::cerr << "Failed to allocate memory" << std::endl;
        exit(EXIT_FAILURE);
    }

    new_node->data = v;
    new_node->next = top;
    top = new_node;
}

value_t pop(StackNode*& top) {
    if (is_empty(top)) return (value_t)0;
    value_t data = top->data;
    StackNode* temp = top;
    top = top->next;
    delete temp;
    return data;
}

bool is_empty(StackNode* top) {
    return top == nullptr;
}

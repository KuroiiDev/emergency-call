#ifndef STORE_HPP
#define STORE_HPP

#include "types.hpp"

const int MAX_CASES = 500;

struct Queue {
    QueueNode* front;
    QueueNode* rear;
    int        count;
};

struct Stack {
    StackNode* top;
    int        count;
};

extern Queue  callQueue;    
extern Stack  historyStack;  
extern Case   allCases[MAX_CASES]; 
extern int    totalCases;    
extern int    nextId;

void initStore();

#endif

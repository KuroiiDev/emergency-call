#ifndef CALL_HPP
#define CALL_HPP

#include "../core/store.hpp"

void enqueue(Queue& q, Case c);
Case dequeue(Queue& q);
bool isQueueEmpty(Queue& q);
void cmdCall();
void cmdQueue();

#endif

#ifndef HISTORY_HPP
#define HISTORY_HPP

#include "../core/store.hpp"

void pushStack(Stack& s, Case c);
Case popStack(Stack& s);
Case peekStack(Stack& s);
bool isStackEmpty(Stack& s);
void cmdHistory();

#endif

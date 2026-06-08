#include "store.hpp"

Queue  callQueue     = {nullptr, nullptr, 0};
Stack  historyStack  = {nullptr, 0};
Case   allCases[MAX_CASES];
int    totalCases    = 0;
int    nextId        = 1;

void initStore() {
    callQueue    = {nullptr, nullptr, 0};
    historyStack = {nullptr, 0};
    totalCases   = 0;
    nextId       = 1;
}

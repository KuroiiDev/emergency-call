#include "help.hpp"
#include <iostream>
using namespace std;

void cmdHelp() {
    cout << "\n  \033[1;36m=== AVAILABLE COMMANDS ===\033[0m\n\n";
    cout << "  \033[1;33mcall\033[0m          Register a new emergency call\n";
    cout << "  \033[1;33mqueue\033[0m         Show current call queue\n";
    cout << "  \033[1;33mdispatch\033[0m      Handle next call in queue\n";
    cout << "  \033[1;33mhistory\033[0m       View & manage handled cases\n";
    cout << "  \033[1;33msearch <id>\033[0m   Search case by ID (all cases)\n";
    cout << "  \033[1;33mreport\033[0m        Generate daily report\n";
    cout << "  \033[1;33mhelp\033[0m          Show this help message\n";
    cout << "  \033[1;33mexit\033[0m          Exit the system\n\n";

    cout << "  \033[0;90m─── Big O Reference ─────────────────────────────────\033[0m\n";
    cout << "  \033[0;90menqueue / dequeue    : O(1)\033[0m\n";
    cout << "  \033[0;90mpush / pop / peek    : O(1)\033[0m\n";
    cout << "  \033[0;90msearch (merge sort)  : O(n log n)\033[0m\n";
    cout << "  \033[0;90msearch (binary)      : O(log n)\033[0m\n";
    cout << "  \033[0;90mreport (parallel)    : O(n) wall-clock\033[0m\n";
    cout << "  \033[0;90m─────────────────────────────────────────────────────\033[0m\n\n";
}

#include "history.hpp"
#include "../utils/display.hpp"
#include <iostream>
#include <cstdio>
using namespace std;

void pushStack(Stack& s, Case c) {
    StackNode* node = new StackNode;
    node->data = c;
    node->next = s.top;
    s.top = node;
    s.count++;
}

Case popStack(Stack& s) {
    Case empty;
    empty.id = -1;
    if (s.top == nullptr) return empty;

    StackNode* temp = s.top;
    Case data = temp->data;
    s.top = s.top->next;
    delete temp;
    s.count--;
    return data;
}

Case peekStack(Stack& s) {
    Case empty;
    empty.id = -1;
    if (s.top == nullptr) return empty;
    return s.top->data;
}

bool isStackEmpty(Stack& s) {
    return s.count == 0;
}
void cmdHistory() {
    cout << "\n";

    if (isStackEmpty(historyStack)) {
        cout << "  \033[0;33mNo handled cases yet.\033[0m\n\n";
        return;
    }

    cout << "  \033[1;36m=== HANDLED CASES ===\033[0m\n";

    Case top = peekStack(historyStack);
    cout << "  Last handled: \033[1;37m#" << top.id << " — " << top.name
         << "\033[0m | " << top.category << " | " << top.time << "\n\n";

    int choice = -1;
    while (choice != 0) {
        cout << "  Options:\n";
        cout << "    \033[1;33m[1]\033[0m View full history\n";
        cout << "    \033[1;33m[2]\033[0m Undo last (remove from history)\n";
        cout << "    \033[1;33m[0]\033[0m Back\n";
        cout << "  Select: ";
        cin >> choice;

        if (choice == 1) {
            cout << "\n  \033[0;90mStack (top → bottom):\033[0m\n";
            cout << "  \033[0;90m─────────────────────────────────────────\033[0m\n";

            StackNode* cur = historyStack.top;
            int no = 1;
            while (cur != nullptr) {
                printf("  \033[0;37m[%d] #%d | %s | %s | %s | \033[1;32mhandled\033[0m\n",
                    no++,
                    cur->data.id,
                    cur->data.name.c_str(),
                    cur->data.category.c_str(),
                    cur->data.time.c_str()
                );
                cur = cur->next;
            }
            cout << "  \033[0;90m─────────────────────────────────────────\033[0m\n\n";

        } else if (choice == 2) {
            if (isStackEmpty(historyStack)) {
                cout << "\n  \033[0;33mHistory is already empty.\033[0m\n\n";
            } else {
                Case removed = popStack(historyStack);
                cout << "\n  \033[1;32m✓ Removed from history:\033[0m\n";
                printCaseDetail(removed);
                cout << "\n";

                if (isStackEmpty(historyStack)) {
                    cout << "  \033[0;33mNo more handled cases.\033[0m\n\n";
                    break;
                } else {
                    top = peekStack(historyStack);
                    cout << "  Last handled: \033[1;37m#" << top.id << " — " << top.name
                         << "\033[0m | " << top.category << " | " << top.time << "\n\n";
                }
            }
        } else if (choice != 0) {
            cout << "\n  \033[0;31mInvalid option.\033[0m\n\n";
        }
    }
    cout << "\n";
}

#include "call.hpp"
#include "../utils/display.hpp"
#include <iostream>
#include <string>
using namespace std;

void enqueue(Queue& q, Case c) {
    QueueNode* node = new QueueNode;
    node->data = c;
    node->next = nullptr;

    if (q.front == nullptr) {
        q.front = q.rear = node;
    } else {
        q.rear->next = node;
        q.rear = node;
    }
    q.count++;
}

Case dequeue(Queue& q) {
    Case empty;
    empty.id = -1;
    if (q.front == nullptr) return empty;

    QueueNode* temp = q.front;
    Case data = temp->data;
    q.front = q.front->next;
    if (q.front == nullptr) q.rear = nullptr;
    delete temp;
    q.count--;
    return data;
}

bool isQueueEmpty(Queue& q) {
    return q.count == 0;
}
void cmdCall() {
    cout << "\n  \033[1;31m=== NEW EMERGENCY CALL ===\033[0m\n\n";

    Case newCase;
    newCase.id = nextId++;
    newCase.status = "waiting";

    cout << "  Caller name   : ";
    cin.ignore();
    getline(cin, newCase.name);
    cout << "  Phone number  : ";
    getline(cin, newCase.phone);

    cout << "  Emergency type:\n";
    for (int i = 0; i < DEFAULT_CAT_COUNT; i++) {
        cout << "    \033[1;33m[" << (i + 1) << "]\033[0m " << DEFAULT_CATEGORIES[i] << "\n";
    }
    cout << "    \033[0;90m[other]\033[0m Custom category\n";
    cout << "  Select: ";

    string input;
    getline(cin, input);

    bool isFixed = false;
    if (input.length() == 1 && input[0] >= '1' && input[0] <= '5') {
        int idx = input[0] - '1';
        newCase.category = DEFAULT_CATEGORIES[idx];
        isFixed = true;
    }
    if (!isFixed) {
        if (input.empty()) {
            newCase.category = "Unknown";
        } else {
            newCase.category = input;
        }
    }

    newCase.time = getCurrentTime();

    enqueue(callQueue, newCase);
    allCases[totalCases++] = newCase;

    cout << "\n  \033[1;32m✓ Call registered successfully.\033[0m\n";
    printCaseDetail(newCase);
    cout << "\n";
}

void cmdQueue() {
    cout << "\n";
    if (callQueue.count == 0) {
        cout << "  \033[0;33mQueue is empty.\033[0m\n\n";
        return;
    }

    cout << "  \033[1;36m=== CALL QUEUE (" << callQueue.count << " waiting) ===\033[0m\n\n";
    printf("  \033[1;37m%3s  | %-4s | %-16s | %-12s | %-8s\033[0m\n",
        "#", "ID", "Name", "Category", "Time");
    cout << "  \033[0;90m────+──────+──────────────────+──────────────+──────────\033[0m\n";

    QueueNode* cur = callQueue.front;
    int no = 1;
    while (cur != nullptr) {
        printf("  \033[0;37m%3d  | #%-3d | %-16s | %-12s | %s\033[0m\n",
            no++,
            cur->data.id,
            cur->data.name.substr(0, 16).c_str(),
            cur->data.category.substr(0, 12).c_str(),
            cur->data.time.c_str()
        );
        cur = cur->next;
    }
    cout << "\n";
}
